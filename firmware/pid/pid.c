/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief General PID implementation for AVR.
 *
 * Discrete PID controller implementation. Set up by giving P/I/D terms
 * to Init_PID(), and uses a struct PID_DATA to store internal values.
 *
 * - File:               pid.c
 * - Compiler:           IAR EWAAVR 4.11A
 * - Supported devices:  All AVR devices can be used.
 * - AppNote:            AVR221 - Discrete PID controller
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name$
 * $Revision: 456 $
 * $RCSfile$
 * $Date: 2006-02-16 12:46:13 +0100 (to, 16 feb 2006) $
 *****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <common/settings.h>
#include "pid.h"

/*! \brief Initialisation of PID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param p_factor  Proportional term.
 *  \param i_factor  Integral term.
 *  \param d_factor  Derivate term.
 *  \param pid  Struct with PID status.
 */
void pidInit(struct Settings *settings, struct PID_DATA *pid)
// Set up PID controller parameters
{
	// Start values for PID controller
	memset(pid, 0, sizeof(struct PID_DATA));
	// Tuning constants for PID loop
	pid->P_Factor = settings->pFactor;
	pid->I_Factor = settings->iFactor;
	pid->D_Factor = settings->dFactor;
	// Limits to avoid overflow
	pid->maxError = MAX_INT / (pid->P_Factor + 1);
	pid->maxD_Error = MAX_INT / (pid->D_Factor + 1);
	pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}


/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid_st  PID status struct.
 */
uint8_t pidController(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st) {
	int16_t error, p_term, d_term;
	int32_t i_term, ret, temp;

	error = setPoint - processValue;

	// Calculate Pterm and limit error overflow
	if (error > pid_st->maxError) {
		p_term = MAX_INT;
	} else if (error < -pid_st->maxError) {
		p_term = -MAX_INT;
	} else {
		p_term = pid_st->P_Factor * error;
	}

	// Calculate Iterm and limit integral runaway
	temp = pid_st->sumError + error;
	if (temp > pid_st->maxSumError) {
		i_term = MAX_I_TERM;
		pid_st->sumError = pid_st->maxSumError;
	} else if (temp < -pid_st->maxSumError) {
		i_term = -MAX_I_TERM;
		pid_st->sumError = -pid_st->maxSumError;
	} else {
		pid_st->sumError = temp;
		i_term = pid_st->I_Factor * pid_st->sumError;
	}

	// Calculate Dterm and limit error overflow
	uint8_t index = pid_st->processValuesIndex++ % HISTORY_SIZE;
	error = pid_st->processValuesHistory[index] - processValue;
	if (error > pid_st->maxD_Error) {
		d_term = MAX_INT;
	} else if (error < -pid_st->maxD_Error) {
		d_term = -MAX_INT;
	} else {
		d_term = pid_st->D_Factor * error;
	}	
	pid_st->processValuesHistory[index] = processValue;

	//normalize
	i_term >>= 8;
	ret = (p_term + i_term + d_term) >> 7;
	if (ret > 0) {
		ret = 0;
	} else if (ret < -MAX_OUT) {
		ret = -MAX_OUT;
	}

	return ((uint8_t) -ret);
}

/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void pidResetIntegrator(struct PID_DATA* pid_st) {
	pid_st->sumError = 0;
}
