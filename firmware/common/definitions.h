#ifndef CAMERACOOLER_DEFINITIONS_H
#define CAMERACOOLER_DEFINITIONS_H

//usb feature report ids
#define REPORT_ID_RUNTIME_INFO 1
#define REPORT_ID_SETTINGS     2
#define REPORT_ID_COOLER_STATE 3

//dew point calculation coefficients
#define DEW_POINT_A 17.27
#define DEW_POINT_B 23770.0

//adc definitions
#define MAX_ADC_VALUE    1023.0
#define ROOM_TEMP        298.15 //room temp in Kelvins

#endif //CAMERACOOLER_DEFINITIONS_H
