#ifndef SMTC_H_
#define SMTC_H_

#include <stdint.h>


#define RETRY_TIMES	10
#define CALIBRATION_KEY 0xaa
#define RESET_CALIBRATION_KEY	0x55 
#define WDT_RESET_SIGNATURE 	0xCA
#define WDT_MAX_OFF_INTERVAL_S 4147200 //48 days
#define WDT_RESET_COUNT_SIGNATURE	0xBE
#define TEMP_DATA_SIZE 2
#define MV_DATA_SIZE 2
#define TEMP_SCALE_FACTOR ((float)10)
#define MV_SCALE_FACTOR ((float)100)

enum
{
	TCP_VAL1_ADD = 0,
	TCP_VAL2_ADD = TCP_VAL1_ADD + TEMP_DATA_SIZE,
	TCP_VAL3_ADD = TCP_VAL2_ADD + TEMP_DATA_SIZE,
	TCP_VAL4_ADD = TCP_VAL3_ADD + TEMP_DATA_SIZE,
	TCP_VAL5_ADD = TCP_VAL4_ADD + TEMP_DATA_SIZE,
	TCP_VAL6_ADD = TCP_VAL5_ADD + TEMP_DATA_SIZE,
	TCP_VAL7_ADD = TCP_VAL6_ADD + TEMP_DATA_SIZE,
	TCP_VAL8_ADD = TCP_VAL7_ADD + TEMP_DATA_SIZE,
	TCP_TYPE1 = TCP_VAL8_ADD + TEMP_DATA_SIZE,
	TCP_TYPE2,
	TCP_TYPE3,
	TCP_TYPE4,
	TCP_TYPE5,
	TCP_TYPE6,
	TCP_TYPE7,
	TCP_TYPE8,
	DIAG_TEMPERATURE_MEM_ADD,
	DIAG_5V_MEM_ADD,
	I2C_MEM_WDT_RESET_ADD = DIAG_5V_MEM_ADD + 2,
	I2C_MEM_WDT_INTERVAL_SET_ADD,
	I2C_MEM_WDT_INTERVAL_GET_ADD = I2C_MEM_WDT_INTERVAL_SET_ADD + 2,
	I2C_MEM_WDT_INIT_INTERVAL_SET_ADD = I2C_MEM_WDT_INTERVAL_GET_ADD + 2,
	I2C_MEM_WDT_INIT_INTERVAL_GET_ADD = I2C_MEM_WDT_INIT_INTERVAL_SET_ADD + 2,
	I2C_MEM_WDT_RESET_COUNT_ADD = I2C_MEM_WDT_INIT_INTERVAL_GET_ADD + 2,
	I2C_MEM_WDT_CLEAR_RESET_COUNT_ADD = I2C_MEM_WDT_RESET_COUNT_ADD + 2,
	I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD,
	I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD = I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD + 4,
	REVISION_HW_MAJOR_MEM_ADD  = I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD + 4,
	REVISION_HW_MINOR_MEM_ADD,
	REVISION_MAJOR_MEM_ADD,
	REVISION_MINOR_MEM_ADD,

	TCP_MV1_ADD,
	TCP_MV2_ADD = TCP_MV1_ADD + MV_DATA_SIZE,
	TCP_MV3_ADD = TCP_MV2_ADD + MV_DATA_SIZE,
	TCP_MV4_ADD = TCP_MV3_ADD + MV_DATA_SIZE,
	TCP_MV5_ADD = TCP_MV4_ADD + MV_DATA_SIZE,
	TCP_MV6_ADD = TCP_MV5_ADD + MV_DATA_SIZE,
	TCP_MV7_ADD = TCP_MV6_ADD + MV_DATA_SIZE,
	TCP_MV8_ADD = TCP_MV7_ADD + MV_DATA_SIZE,
	TCP_REINIT_COUNT = TCP_MV8_ADD + MV_DATA_SIZE,
	TCP_SPS1_ADD = TCP_REINIT_COUNT + 4,
	TCP_SPS2_ADD = TCP_SPS1_ADD + 2,
	TCP_CARD_TYPE = TCP_SPS2_ADD+2,
	TCP_RASP_VOLT,
	I2C_MODBUS_SETINGS_ADD = TCP_RASP_VOLT + 2, //5 bytes
	TCP_LEDS_FUNC = I2C_MODBUS_SETINGS_ADD + 5, //2 bytes
	TCP_LED_THRESHOLD1 = TCP_LEDS_FUNC + 2,
	TCP_LED_THRESHOLD2 = TCP_LED_THRESHOLD1 + 2,
	TCP_LED_THRESHOLD3 = TCP_LED_THRESHOLD2 + 2,
	TCP_LED_THRESHOLD4 = TCP_LED_THRESHOLD3 + 2,
	TCP_LED_THRESHOLD5 = TCP_LED_THRESHOLD4 + 2,
	TCP_LED_THRESHOLD6 = TCP_LED_THRESHOLD5 + 2,
	TCP_LED_THRESHOLD7 = TCP_LED_THRESHOLD6 + 2,
	TCP_LED_THRESHOLD8 = TCP_LED_THRESHOLD7 + 2,
	I2C_CALIB_RES = TCP_LED_THRESHOLD8 + 2,//float
	I2C_CALIB_CH = I2C_CALIB_RES + 4,//u8
	I2C_SENSORS_TYPE,
	I2C_MEM_ADS_SAMPLE_SWITCH,//u16
	I2C_THERMISTOR1_ADD = I2C_MEM_ADS_SAMPLE_SWITCH + 2,

	I2C_THERMISTOR_END_ADD = I2C_THERMISTOR1_ADD + 20,
	I2C_MAV_FILT_SIZE,
	SLAVE_BUFF_SIZE = 0xff


};

enum {
	TC_TYPE_B = 0,
	TC_TYPE_E,
	TC_TYPE_J,
	TC_TYPE_K,
	TC_TYPE_N,
	TC_TYPE_R,
	TC_TYPE_S,
	TC_TYPE_T,
};


#define CHANNEL_NR_MIN		1
#define TCP_CH_NR_MAX		8
#define TCP_THERMISTORS_NR_MAX 10

#define ERROR	-1
#define OK		0
#define FAIL	-1
#define ARG_ERR -2
#define ARG_CNT_ERR -3


#define SLAVE_OWN_ADDRESS_BASE 0x16

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;

typedef enum
{
	OFF = 0,
	ON,
	STATE_COUNT
} OutStateEnumType;

typedef struct
{
	const char *name;
	const int namePos;
	int (*pFunc)(int, char**);
	const char *help;
	const char *usage1;
	const char *usage2;
	const char *example;
} CliCmdType;

//const CliCmdType *gCmdArray[];

int doBoardInit(int stack);
int rtdHwTypeGet(int dev, int* hw);

//LED's
extern const CliCmdType CMD_READ_LED_MODE;
extern const CliCmdType CMD_WRITE_LED_MODE;
extern const CliCmdType CMD_READ_LED_TH;
extern const CliCmdType CMD_WRITE_LED_TH;

//RS485
extern const CliCmdType CMD_RS485_READ;
extern const CliCmdType CMD_RS485_WRITE;

//Watchdog
extern const CliCmdType CMD_WDT_RELOAD;
extern const CliCmdType CMD_WDT_SET_PERIOD;
extern const CliCmdType CMD_WDT_GET_PERIOD;
extern const CliCmdType CMD_WDT_SET_INIT_PERIOD;
extern const CliCmdType CMD_WDT_GET_INIT_PERIOD;
extern const CliCmdType CMD_WDT_SET_OFF_PERIOD;
extern const CliCmdType CMD_WDT_GET_OFF_PERIOD;
extern const CliCmdType CMD_WDT_GET_RESETS_COUNT;
extern const CliCmdType CMD_WDT_CLR_RESETS_COUNT;

#endif //SMTC_H_
