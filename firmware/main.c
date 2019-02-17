/* Name: main.c
 * Project: hid-data, example how to use HID for data transfer
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-11
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 */

/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.
*/

#define LED_PORT_DDR        DDRB
#define LED_PORT_OUTPUT     PORTB
#define LED_BIT             0

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include "i2c/i2c.h"
#include "bme280/bme280_user.h"

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include <adc/adc.h>
#include <common/runtime_info.h>
#include <common/settings.h>
#include <common/definitions.h>
#include <string.h>
#include <cooler/cooler.h>
#include <pid/pid.h>
#include <stdbool.h>
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

//!!!!! usbHidReportDescriptor size needs to be equal to USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH in usbconfig.h
PROGMEM const char usbHidReportDescriptor[33] = {    /* USB report descriptor */
		0x06, 0x00, 0xff,                    // USAGE_PAGE (Generic Desktop)
		0x09, 0x01,                          // USAGE (Vendor Usage 1)
		0xa1, 0x01,                          // COLLECTION (Application)
		0x15, 0x00,                          //   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00,                    //   LOGICAL_MAXIMUM (255)
		0x75, 0x08,                          //   REPORT_SIZE (8)

		0x85, 0x01,                          //   REPORT_ID (1)
		0x95, sizeof(struct RuntimeInfo),    //   REPORT_COUNT (N)
		0x09, 0x01,                          //   USAGE (Vendor Usage 1)
		0xb2, 0x02, 0x01,                    //   FEATURE (Data,Var,Abs,Buf)

		0x85, 0x02,                          //   REPORT_ID (2)
		0x95, sizeof(struct Settings),       //   REPORT_COUNT (N)
		0x09, 0x01,                          //   USAGE (Vendor Usage 1)
		0xb2, 0x02, 0x01,                    //   FEATURE (Data,Var,Abs,Buf)

		0xc0                                 // END_COLLECTION
};

static struct RuntimeInfo runtimeInfo;
static struct Settings eepromSettings EEMEM;
static struct Settings settings;
static struct PID_DATA pidData;

static int8_t currentEepromWriteOffset;

void readSettingsFromEEPROM(struct Settings* targetSettings) {
	eeprom_read_block(targetSettings, &eepromSettings, sizeof(eepromSettings));
}

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar usbFunctionWrite(uchar *data, uchar len) {
	int8_t bytesRemaining = sizeof(eepromSettings) - currentEepromWriteOffset;
	if(bytesRemaining <= 0) {
		return 1; //end of transfer
	}
	if (currentEepromWriteOffset == 0) {
		//first byte is report id
		data++;
		len--;
	}
	if (len > bytesRemaining) {
		len = (uchar)bytesRemaining;
	}
	eeprom_write_block(data, (uchar*)&eepromSettings  + currentEepromWriteOffset, len);
	currentEepromWriteOffset += len;
	if (currentEepromWriteOffset == sizeof(eepromSettings)) { //last write
		struct Settings newSettings;
		readSettingsFromEEPROM(&newSettings);
		if (settings.pFactor != newSettings.pFactor || settings.iFactor != newSettings.iFactor || settings.dFactor != newSettings.dFactor) {
			pidInit(&newSettings, &pidData);
		}
		//TODO: need we reset integrator on target temp change?
//		else if (settings.targetTemp != newSettings.targetTemp) {
//			pidResetIntegrator(&pidData);
//		}
		settings = newSettings;
		return 1; //last chunk
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *) data;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {    /* HID class request */
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			if (rq->wValue.bytes[0] == REPORT_ID_RUNTIME_INFO) {
				usbMsgPtr = (uchar *) &runtimeInfo;
				return sizeof(runtimeInfo);
			} else if (rq->wValue.bytes[0] == REPORT_ID_SETTINGS) {
				usbMsgPtr = (uchar *) &settings;
				return sizeof(settings);
			}
		} else if (rq->bRequest == USBRQ_HID_SET_REPORT && rq->wValue.bytes[0] == REPORT_ID_SETTINGS) {
			currentEepromWriteOffset = 0;
			return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
		}
	} else {
		/* ignore vendor type requests, we don't use any */
	}
	return 0;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "OCDFAInspection"
/* ------------------------------------------------------------------------- */

int main(void) {
	//wdt_enable(WDTO_1S);
	wdt_disable();
	readSettingsFromEEPROM(&settings);
	/* If you don't use the watchdog, replace the call above with a wdt_disable().
	 * On newer devices, the status of the watchdog (on/off, period) is PRESERVED
	 * OVER RESET!
	 */
	/* RESET status: all port bits are inputs without pull-up.
	 * That's the way we need D+ and D-. Therefore we don't need any
	 * additional hardware initialization.
	 */
	odDebugInit();
	DBG1(0x00, 0, 0);       /* debug output: main starts */
	usbInit();
	i2cInit();

	usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	_delay_ms(255); /* fake USB disconnect for > 250 ms */
	usbDeviceConnect();

	LED_PORT_DDR |= _BV(LED_BIT);   /* make the LED bit an output */
	sei();
	adcInit();
	bmeInit();
	coolerInit();
	pidInit(&settings, &pidData);
//	if (bmeStatus != BME280_OK) {
//		sprintf(buf, "BME init failed: %d\r\n", bmeStatus);
//		usartTransmitString(buf);
//	}

	bmeStartInNormalMode();
//	if (bmeStatus != BME280_OK) {
//		sprintf(buf, "BME start failed: %d\r\n", bmeStatus);
//		usartTransmitString(buf);
//	}

	DBG1(0x01, 0, 0);       /* debug output: main loop starts */
	uint16_t iteration = 0;
	for (;;) {                /* main event loop */
		if (iteration & 511) { //every 512 tick
			adcReadNextSample();
		}
		if ((iteration & 4095) == 0) { //every 4096 tick (every 1/27 s)
			runtimeInfo.chipTemp = adcGetTemp(&settings);
			bmeGetCurrentData(&runtimeInfo);
			int16_t safeTargetTemp = runtimeInfo.dewPoint + settings.dewPointUnsafeZone;
			runtimeInfo.targetTemp = settings.targetTemp > safeTargetTemp ? settings.targetTemp : safeTargetTemp;

			runtimeInfo.coolerPower = pidController(runtimeInfo.targetTemp, runtimeInfo.chipTemp, &pidData);
			coolerSetPower(runtimeInfo.coolerPower);
		}
		//DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
		//wdt_reset();
		usbPoll();
		iteration++;
	}
	return 0;
}
#pragma clang diagnostic pop

/* ------------------------------------------------------------------------- */
