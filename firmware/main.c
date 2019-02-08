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

//TODO: выставить минимально необходимое значение
#define USB_PACKET_SIZE 32

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include "i2c/i2c.h"
#include "bme280/bme280_user.h"

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include <adc/adc.h>
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM const char usbHidReportDescriptor[22] = {    /* USB report descriptor */
		0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
		0x09, 0x01,                    // USAGE (Vendor Usage 1)
		0xa1, 0x01,                    // COLLECTION (Application)
		0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
		0x75, USB_PACKET_SIZE,         //   REPORT_SIZE (N)
		0x95, USB_PACKET_SIZE,         //   REPORT_COUNT (N)
		0x09, 0x00,                    //   USAGE (Undefined)
		0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
		0xc0                           // END_COLLECTION
};
/* Since we define only one feature report, we don't use report-IDs (which
 * would be the first byte of the report). The entire report consists of 128
 * opaque data bytes.
 */

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar usbFunctionWrite(uchar *data, uchar len) {
	//eeprom_write_block(data, (uchar *)0 + currentAddress, len);
	if (data[0]) {
		LED_PORT_OUTPUT |= _BV(LED_BIT);
	} else {
		LED_PORT_OUTPUT &= ~_BV(LED_BIT);
	}
	return 1; /* return 1 if this was the last chunk */
}

/* ------------------------------------------------------------------------- */

uchar answer[USB_PACKET_SIZE];
struct bme280_data sensorData;

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *) data;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {    /* HID class request */
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			/* since we have only one report type, we can ignore the report-ID */
			*(int16_t*)answer = adcGetTemp();
			*(int16_t*)(answer + 2) = (int16_t)sensorData.temperature;
			*(uint16_t*)(answer + 4) = (uint16_t)(sensorData.humidity / 10);
			*(int16_t*)(answer + 6) = (int16_t)(sensorData.pressure / 10);
			usbMsgPtr = answer;
			return sizeof(answer);
		} else if (rq->bRequest == USBRQ_HID_SET_REPORT) {
			/* since we have only one report type, we can ignore the report-ID */
			return USB_NO_MSG;  /* use usbFunctionWrite() to receive data from host */
		}
	} else {
		/* ignore vendor type requests, we don't use any */
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

int main(void) {
	//wdt_enable(WDTO_1S);
	wdt_disable();
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
	int8_t bmeStatus = bmeInit();
//	if (bmeStatus != BME280_OK) {
//		sprintf(buf, "BME init failed: %d\r\n", bmeStatus);
//		usartTransmitString(buf);
//	}

	bmeStatus = bmeStartInNormalMode();
//	if (bmeStatus != BME280_OK) {
//		sprintf(buf, "BME start failed: %d\r\n", bmeStatus);
//		usartTransmitString(buf);
//	}

	DBG1(0x01, 0, 0);       /* debug output: main loop starts */
	int16_t iteration = 0;
	for (;;) {                /* main event loop */
		if ((iteration++) % 1000 == 0) {
			adcStartConversion();
			bmeStatus = bmeGetCurrentData(&sensorData);
			if (bmeStatus != BME280_OK) {
				sensorData.humidity = 0;
				sensorData.pressure = 0;
				sensorData.temperature = 0;
			}
		}
		//DBG1(0x02, 0, 0);   /* debug output: main loop iterates */
		//wdt_reset();
		usbPoll();
	}
	return 0;
}

/* ------------------------------------------------------------------------- */
