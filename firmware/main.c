#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "i2c/i2c.h"
#include "bme280/bme280_user.h"

#include <avr/pgmspace.h>
#include <adc/adc.h>
#include <common/runtime_info.h>
#include <common/settings.h>
#include <common/definitions.h>
#include <string.h>
#include <cooler/cooler.h>
#include <pid/pid.h>
#include <stdbool.h>
#include "usbdrv.h"

#define LED_PORT_DDR        DDRD
#define LED_PORT_OUTPUT     PORTD
#define LED_BIT             6

//!!!!! usbHidReportDescriptor size needs to be equal to USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH in usbconfig.h
PROGMEM const char usbHidReportDescriptor[42] = {    /* USB report descriptor */
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

		0x85, 0x03,                          //   REPORT_ID (3)
		0x95, 0x01,                          //   REPORT_COUNT (1)
		0x09, 0x01,                          //   USAGE (Vendor Usage 1)
		0xb2, 0x02, 0x01,                    //   FEATURE (Data,Var,Abs,Buf)

		0xc0                                 // END_COLLECTION
};

static struct RuntimeInfo runtimeInfo;
static struct Settings eepromSettings EEMEM;
static struct Settings settings;
static struct PID_DATA pidData;
static bool coolerState = true;

static uint8_t currentReportId;
static int8_t currentEepromWriteOffset;

void ledOn() {
	LED_PORT_OUTPUT |= 1 << LED_BIT;
}

void ledOff() {
	LED_PORT_OUTPUT &= ~(1 << LED_BIT);
}

void ledToggle() {
	LED_PORT_OUTPUT ^= 1 << LED_BIT;
}

void readSettingsFromEEPROM(struct Settings* targetSettings) {
	eeprom_read_block(targetSettings, &eepromSettings, sizeof(eepromSettings));
}

/* usbFunctionWrite() is called when the host sends a chunk of data to the
 * device. For more information see the documentation in usbdrv/usbdrv.h.
 */
uchar usbFunctionWrite(uchar *data, uchar len) {
	if (currentReportId == REPORT_ID_COOLER_STATE) {
		coolerState = data[1];
		coolerSetState(coolerState);
		return 1; //end of transfer
	}

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
		settings = newSettings;
		return 1; //last chunk
	}
	return 0;
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *) data;

	uint8_t reportId = rq->wValue.bytes[0];
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {    /* HID class request */
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			if (reportId == REPORT_ID_RUNTIME_INFO) {
				usbMsgPtr = (uchar *) &runtimeInfo;
				return sizeof(runtimeInfo);
			} else if (reportId == REPORT_ID_SETTINGS) {
				usbMsgPtr = (uchar *) &settings;
				return sizeof(settings);
			} else if (reportId == REPORT_ID_COOLER_STATE) {
				usbMsgPtr = (uchar *) &coolerState;
				return sizeof(coolerState);
			}
		} else if (rq->bRequest == USBRQ_HID_SET_REPORT) {
			currentReportId = reportId;
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
int main(void) {
	readSettingsFromEEPROM(&settings);
	usbInit();
	i2cInit();

	usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	_delay_ms(255); /* fake USB disconnect for > 250 ms */
	usbDeviceConnect();

	LED_PORT_DDR |= 1 << LED_BIT;   /* make the LED bit an output */
	sei();
	adcInit();
	bmeInit();
	coolerInit();
	coolerSetState(coolerState);
	pidInit(&settings, &pidData);
	bmeStartInNormalMode();
	uint16_t iteration = 1;
	for (;;) {                /* main event loop */
		if ((iteration & 0x1FFF) == 0) { //every 8192 tick
			adcReadNextSample();
		}
		if (iteration == 0) { //every 65536 tick (4 Hz)
			runtimeInfo.chipTemp = adcGetTemp(&settings);
			bmeGetCurrentData(&runtimeInfo);
			int16_t safeTargetTemp = runtimeInfo.dewPoint + settings.dewPointUnsafeZone;
			runtimeInfo.targetTemp = settings.targetTemp > safeTargetTemp ? settings.targetTemp : safeTargetTemp;

			if (coolerState) {
				runtimeInfo.coolerPower = pidController(runtimeInfo.targetTemp, runtimeInfo.chipTemp, &pidData);
			} else {
				runtimeInfo.coolerPower = 0;
			}
			if (runtimeInfo.coolerPower) {
				ledToggle();
			} else {
				ledOff();
			}
			coolerSetPower(runtimeInfo.coolerPower);
		}
		usbPoll();
		iteration++;
	}
	return 0;
}
#pragma clang diagnostic pop
