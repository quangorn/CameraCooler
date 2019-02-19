/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for pid.c.
 *
 * - File:               pid.h
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

#ifndef PID_H
#define PID_H

#include "stdint.h"

/*! \brief PID Status
 *
 * Setpoints and data used by the PID control algorithm
 */
typedef struct PID_DATA {
	//! Last process value, used to find derivative of process value.
	int16_t lastProcessValue;
	//! Summation of errors, used for integrate calculations
	int32_t sumError;
	//! The Proportional tuning constant, multiplied with SCALING_FACTOR
	int16_t P_Factor;
	//! The Integral tuning constant, multiplied with SCALING_FACTOR
	int16_t I_Factor;
	//! The Derivative tuning constant, multiplied with SCALING_FACTOR
	int16_t D_Factor;
	//! Maximum allowed error, avoid overflow
	int16_t maxError;
	//! Maximum allowed sumerror, avoid overflow
	int32_t maxSumError;
} pidData_t;

/*! \brief Maximum values
 *
 * Needed to avoid sign/overflow problems
 */
// Maximum value of variables
#define MAX_INT         INT16_MAX
#define MAX_I_TERM      0x800000
#define MAX_OUT         0xFF

void pidInit(struct Settings *settings, struct PID_DATA *pid);

uint8_t pidController(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st);

void pidResetIntegrator(struct PID_DATA *pid_st);

#endif
