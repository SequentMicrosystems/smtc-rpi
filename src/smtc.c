/*
 * smtc.c:
 *	Command-line interface to the Raspberry
 *	Pi's Thermocouple board.
 *	Copyright (c) 2016-2023 Sequent Microsystem
 *	<http://www.sequentmicrosystem.com>
 ***********************************************************************
 *	Author: Alexandru Burcea
 ***********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "smtc.h"
#include "comm.h"
#include "thread.h"
#include "wdt.h"
#include "led.h"
#include "rs485.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)3

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define THREAD_SAFE

void usage(void);
const char *tcTypes[TC_TYPE_T + 1] = {"B(0)", "E(1)", "J(2)", "K(3)", "N(4)",
	"R(5)", "S(6)", "T(7)"};

int doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
	{"-h", 1, &doHelp,
		"\t-h          Display the list of command options or one command option details\n",
		"\tUsage:      smtc -h    Display command options list\n",
		"\tUsage:      smtc -h <param>   Display help for <param> command option\n",
		"\tExample:    smtc -h write    Display help for \"write\" command option\n"};

int doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION = {"-v", 1, &doVersion,
	"\t-v          Display the version number\n", "\tUsage:      smtc -v\n", "",
	"\tExample:    smtc -v  Display the version number\n"};

int doWarranty(int argc, char *argv[]);
const CliCmdType CMD_WAR = {"-warranty", 1, &doWarranty,
	"\t-warranty   Display the warranty\n", "\tUsage:      smtc -warranty\n", "",
	"\tExample:    smtc -warranty  Display the warranty text\n"};

int doList(int argc, char *argv[]);
const CliCmdType CMD_LIST =
	{"-list", 1, &doList,
		"\t-list:      List all thermocouples connected cards, return nr of boards and stack level for every board\n",
		"\tUsage:      smtc -list\n", "",
		"\tExample:    smtc -list display all the connected thermocouples cards \n"};

int doSmtcRead(int argc, char *argv[]);
const CliCmdType CMD_READ =
	{"read", 2, &doSmtcRead, "\tread:       Read smtc channel temperature\n",
		"\tUsage:      smtc <id> read <channel>\n", "",
		"\tExample:    smtc 0 read 2; Read the temperature on channel #2 on Board #0\n"};

int doSmtcReadMv(int argc, char *argv[]);
const CliCmdType CMD_READ_MV =
	{"readmv", 2, &doSmtcReadMv,
		"\treadmv:    Read smtc channel voltage in mV\n",
		"\tUsage:      smtc <id> readmv <channel>\n", "",
		"\tExample:    smtc 0 readmv 2; Read the voltage on channel #2 on Board #0\n"};

int doSmtcReadConnTemp(int argc, char *argv[]);
const CliCmdType CMD_READ_CONN_TEMP =
	{"readct", 2, &doSmtcReadConnTemp,
		"\treadct:    Read smtc connector temperature measured by one of the 10 thermistors on the card\n",
		"\tUsage:      smtc <id> readct <channel>\n", "",
		"\tExample:    smtc 0 readct 2; Read the connector temperature on thermistor #2 on Board #0\n"};

int doSmtcCalib(int argc, char *argv[]);
const CliCmdType CMD_CALIB =
	{"cal", 2, &doSmtcCalib,
		"\tcal:	    Calibrate the resistance measurement, perform 2 points calibration for completion \n",
		"\tUsage:      smtc <id> cal <channel> <value in ohms>\n", "",
		"\tExample:    smtc 0 cal 2 100.34; Send one point of calibration at 100.34 ohms for channel #2 on card #0 \n"};

int doSmtcCalibRst(int argc, char *argv[]);
const CliCmdType CMD_CALIB_RST =
	{"calrst", 2, &doSmtcCalibRst,
		"\tcalrst:	    Reset calibration data for one channel\n",
		"\tUsage:      smtc <id> calrst <channel>\n", "",
		"\tExample:    smtc 0 calrst 2; Reset calibration data at factory default for channel #2 on card #0 \n"};

int doBoard(int argc, char *argv[]);
const CliCmdType CMD_BOARD = {"board", 2, &doBoard,
	"\tboard:      Display board firmware version\n",
	"\tUsage:      smtc <id> board\n", "", "\tExample:    smtc 0 board\n"};

int doSnsTypeRead(int argc, char *argv[]);
const CliCmdType CMD_SNS_TYPE_READ =
	{"styperd", 2, &doSnsTypeRead,
		"\tstyperd:    Display sensor type [0..7]: (B, E, J, K, N, R, S, T) per channel \n",
		"\tUsage:      smtc <id> styperd <channel>\n", "",
		"\tExample:    smtc 0 styperd 1; Display the type of sensor for channel 1 on the board #0\n"};

int doSnsTypeWrite(int argc, char *argv[]);
const CliCmdType CMD_SNS_TYPE_WRITE =
	{"stypewr", 2, &doSnsTypeWrite,
		"\tstypewr:    Set sensor type([0..7]: (B, E, J, K, N, R, S, T) per channel \n",
		"\tUsage:      smtc <id> stypewr <channel> <type> \n", "",
		"\tExample:    smtc 0 stypewr 1 1; Set the type of sensor for channel 1 on the board #0 to thermocouple E \n"};

int doFiltSizeRd(int argc, char *argv[]);
const CliCmdType CMD_FILT_SIZE_READ =
	{"fszrd", 2, &doFiltSizeRd,
		"\tfszrd:    Display moving averaging filter number of samples \n",
		"\tUsage:      smtc <id> fszrd \n", "",
		"\tExample:    smtc 0 fszrd ; Display moving averaging filter number of samples\n"};

int doFiltSizeWrite(int argc, char *argv[]);
const CliCmdType CMD_FILT_SIZE_WRITE =
	{"fszwr", 2, &doFiltSizeWrite,
		"\tfszwr:    Set the moving average filter size or number of samples \n",
		"\tUsage:      smtc <id> fszwr <value> \n", "",
		"\tExample:    smtc 0 fszwr 10; Set the moving average filter size or number of samples to 10\n"};

char *warranty =
	"	       Copyright (c) 2016-2023 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";

const CliCmdType *gCmdArray[] = {&CMD_HELP, &CMD_WAR, &CMD_LIST, &CMD_VERSION,
	&CMD_READ, &CMD_READ_MV, &CMD_READ_CONN_TEMP, &CMD_BOARD, &CMD_WDT_RELOAD,
	&CMD_WDT_SET_PERIOD, &CMD_WDT_GET_PERIOD, &CMD_WDT_SET_INIT_PERIOD,
	&CMD_WDT_GET_INIT_PERIOD, &CMD_WDT_SET_OFF_PERIOD, &CMD_WDT_GET_OFF_PERIOD,
	&CMD_WDT_GET_RESETS_COUNT, &CMD_WDT_CLR_RESETS_COUNT, &CMD_READ_LED_MODE,
	&CMD_WRITE_LED_MODE, &CMD_READ_LED_TH, &CMD_WRITE_LED_TH,
	//&CMD_CALIB,
	//&CMD_CALIB_RST,
	&CMD_RS485_READ, &CMD_RS485_WRITE, &CMD_SNS_TYPE_READ, &CMD_SNS_TYPE_WRITE,
	&CMD_FILT_SIZE_READ, &CMD_FILT_SIZE_WRITE,
	NULL}; //null terminated array of cli structure pointers

int doBoardInit(int stack)
{
	int dev = 0;
	int add = 0;
	uint8_t buff;

	if ( (stack < 0) || (stack > 7))
	{
		printf("Invalid stack level [0..7]!");
		return ERROR;
	}
	add = stack + SLAVE_OWN_ADDRESS_BASE;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, &buff, 1))
	{
		printf("Thermocouple card  id %d not detected\n", stack);
		return ERROR;
	}

	return dev;
}

int boardCheck(u8 add)
{
	int dev;
	u8 buff;

	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	return (i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, &buff, 1));
}

int smtcHwTypeGet(int dev, int *hw)
{
	u8 buff;
	if (FAIL == i2cMem8Read(dev, TCP_CARD_TYPE, &buff, 1))
	{
		return ERROR;
	}
	*hw = buff;
	return OK;
}

int smtcChGet(int dev, u8 channel, float *temperature)
{
	u8 buff[sizeof(int16_t)];
	int16_t val = 0;

	if (NULL == temperature)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > TCP_CH_NR_MAX))
	{
		printf("Invalid smtc channel nr!\n");
		return ERROR;
	}

	if (FAIL
		== i2cMem8Read(dev, TCP_VAL1_ADD + sizeof(int16_t) * (channel - 1), buff,
			sizeof(int16_t)))
	{
		return ERROR;
	}

	memcpy(&val, buff, sizeof(int16_t));
	*temperature = (float)val / TEMP_SCALE_FACTOR;
	return OK;
}

int smtcChGetMv(int dev, u8 channel, float *voltage)
{
	u8 buff[sizeof(int16_t)];
	int16_t val = 0;

	if (NULL == voltage)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > TCP_CH_NR_MAX))
	{
		printf("Invalid sensor channel nr!\n");
		return ERROR;
	}

	if (FAIL
		== i2cMem8Read(dev, TCP_MV1_ADD + sizeof(int16_t) * (channel - 1), buff,
			sizeof(int16_t)))
	{
		return ERROR;
	}

	memcpy(&val, buff, sizeof(int16_t));
	*voltage = (float)val / MV_SCALE_FACTOR;
	return OK;
}

int smtcChGetConnTemp(int dev, u8 channel, float *voltage)
{
	u8 buff[sizeof(int16_t)];
	int16_t val = 0;

	if (NULL == voltage)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > TCP_THERMISTORS_NR_MAX))
	{
		printf("Invalid thermistor channel nr![1..10]\n");
		return ERROR;
	}

	if (FAIL
		== i2cMem8Read(dev, I2C_THERMISTOR1_ADD + sizeof(int16_t) * (channel - 1),
			buff, sizeof(int16_t)))
	{
		return ERROR;
	}

	memcpy(&val, buff, sizeof(int16_t));
	*voltage = (float)val / 10;
	return OK;
}

/*
 * doSmtcRead:
 *	Read temperature on one channel
 ******************************************************************************************
 */
int doSmtcRead(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);

		if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_CH_NR_MAX))
		{
			printf("Thermocouple channel number value out of range!\n");
			exit(1);
		}

		if (OK != smtcChGet(dev, (u8)ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%.1f\n", val);
	}
	else
	{
		printf("Usage: %s read temperature value\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doSmtcReadMv(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_CH_NR_MAX))
		{
			printf("Thermocouple channel number value out of range!\n");
			exit(1);
		}

		if (OK != smtcChGetMv(dev, (u8)ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%.2f\n", val);
	}
	else
	{
		printf("Usage: %s read voltage  value\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doSmtcReadConnTemp(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_THERMISTORS_NR_MAX))
		{
			printf("Thermistor channel number value out of range![1..10]\n");
			exit(1);
		}

		if (OK != smtcChGetConnTemp(dev, (u8)ch, &val))
		{
			printf("Fail to read!\n");
			exit(1);
		}
		printf("%.1f\n", val);
	}
	else
	{
		printf("Usage: %s read connector temperature\n", argv[0]);
		exit(1);
	}
	return OK;
}

int doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				if (strcasecmp(argv[2], gCmdArray[i]->name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i]->help, gCmdArray[i]->usage1,
						gCmdArray[i]->usage2, gCmdArray[i]->example);
					break;
				}
			}
			i++;
		}
		if (NULL == gCmdArray[i])
		{
			printf("Option \"%s\" not found\n", argv[2]);
			i = 0;
			while (NULL != gCmdArray[i])
			{
				if (gCmdArray[i]->name != NULL)
				{
					printf("%s", gCmdArray[i]->help);
					break;
				}
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				printf("%s", gCmdArray[i]->help);
			}
			i++;
		}
	}
	return OK;
}

//********************** Calibration *************************
int smtcSetCalib(int dev, int channel, float value)
{
	u8 buff[sizeof(float) + 1];

	if ( (value < 0) || (value > 4000))
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > TCP_CH_NR_MAX))
	{
		printf("Invalid smtc channel nr!\n");
		return ERROR;
	}
	memcpy(buff, &value, sizeof(float));
	buff[sizeof(float)] = channel;
	if (FAIL == i2cMem8Write(dev, I2C_CALIB_RES, buff, sizeof(float) + 1))
	{
		return ERROR;
	}
	return OK;
}

int smtcResetCalib(int dev, int channel)
{
	u8 buff[sizeof(float) + 1];
	float value = -1;

	if ( (channel < CHANNEL_NR_MIN) || (channel > TCP_CH_NR_MAX))
	{
		printf("Invalid smtc channel nr!\n");
		return ERROR;
	}
	memcpy(buff, &value, sizeof(float));
	buff[sizeof(float)] = channel;
	if (FAIL == i2cMem8Write(dev, I2C_CALIB_RES, buff, sizeof(float) + 1))
	{
		return ERROR;
	}
	return OK;
}

int doSmtcCalib(int argc, char *argv[])
{
	int ch = 0;
	float val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 5)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_CH_NR_MAX))
		{
			printf("Thermocouple channel number value out of range!\n");
			exit(1);
		}
		val = atof(argv[4]);

		if (OK != smtcSetCalib(dev, ch, val))
		{
			printf("Fail to calibrate!\n");
			exit(1);
		}
		printf("OK\n");
	}
	else
	{
		printf("%s", CMD_CALIB.usage1);
		exit(1);
	}
	return OK;
}

int doSmtcCalibRst(int argc, char *argv[])
{
	int ch = 0;

	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_CH_NR_MAX))
		{
			printf("Thermocouple channel number value out of range!\n");
			exit(1);
		}

		if (OK != smtcResetCalib(dev, ch))
		{
			printf("Fail to calibrate!\n");
			exit(1);
		}
		printf("OK\n");
	}
	else
	{
		printf("%s", CMD_CALIB_RST.usage1);
		exit(1);
	}
	return OK;
}

int doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("smtc v%d.%d.%d Copyright (c) 2016 - 2023 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: smtc -warranty\n");
	return OK;
}

int doList(int argc, char *argv[])
{
	int ids[8];
	int i;
	int cnt = 0;

	UNUSED(argc);
	UNUSED(argv);

	for (i = 0; i < 8; i++)
	{
		if (boardCheck(SLAVE_OWN_ADDRESS_BASE + i) == OK)
		{
			ids[cnt] = i;
			cnt++;
		}
	}
	printf("%d board(s) detected\n", cnt);
	if (cnt > 0)
	{
		printf("Id:");
	}
	while (cnt > 0)
	{
		cnt--;
		printf(" %d", ids[cnt]);
	}
	printf("\n");
	return OK;
}

//#define DEBUG_ADS
/* 
 * Self test for production
 */
int doBoard(int argc, char *argv[])
{
	int dev = 0;
#ifdef DEBUG_ADS
	int reinit = 0;
	u8 cardType = 0;
	u16 sps[2] =
	{
		0,
		0};
#endif	
	s8 saux8 = 0;
	u8 buff[5] = {0, 0, 0, 0, 0};

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		exit(1);
	}

	if (argc == 3)
	{
#ifdef DEBUG_ADS
		if (FAIL == i2cMem8Read(dev, TCP_SPS1_ADD, buff, 5))
		{
			exit(1);
		}
		memcpy(sps, buff, 4);
		cardType = buff[4];
		if (FAIL == i2cMem8Read(dev, TCP_REINIT_COUNT, buff, 4))
		{
			exit(1);
		}
		memcpy(&reinit, buff, 4);
#endif		
		if (FAIL == i2cMem8Read(dev, DIAG_TEMPERATURE_MEM_ADD, buff, 3))
		{
			exit(1);
		}
		memcpy(&saux8, buff, 1);

		if (FAIL == i2cMem8Read(dev, REVISION_MAJOR_MEM_ADD, buff, 2))
		{
			exit(1);
		}
		printf("Thermocouple card firmware version %d.%02d\n", (int)buff[0],
			(int)buff[1]);
#ifdef DEBUG_ADS
		printf("ADC: ARC = %d, SPS1 = %d, SPS2 = %d, Card Type = %d\n", reinit,
		(int)sps[0], (int)sps[1], (int)cardType);
#endif		
		printf("CPU Temp %dC\n", (int)saux8);

	}
#ifdef DEBUG_ADS	
	else if (argc == 4)
	{
		printf("Perform reset..");
		if (FAIL == i2cMem8Write(dev, 0xaa, buff, 1))
		{
			printf("fail!\n");
		}
		else
		{
			printf("done\n");
		}
	}
#endif	
	else
	{
		printf("Invalid arguments number! Usage: %s\n", CMD_BOARD.usage1);
	}
	return OK;
}

int doWarranty(int argc UNU, char *argv[] UNU)
{
	printf("%s\n", warranty);
	return OK;
}

int sensorTypeRead(int dev, int ch, int *val)
{
	u8 buff;

	if (NULL == val)
	{
		return ERROR;
	}

	if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_CH_NR_MAX))
	{
		printf("Invalid thermocouple channel number!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, TCP_TYPE1 + ch - 1, &buff, 1))
	{
		return ERROR;
	}

	*val = buff;
	return OK;
}

int sensorWrite(int dev, int ch, int val)
{
	u8 buff;

	if (val < TC_TYPE_B || val > TC_TYPE_T)
	{
		printf(
			"Invalid thermocouple type! Use 0..7 : (B, E, J, K, N, R, S, T)\n");
		return ERROR;
	}

	if ( (ch < CHANNEL_NR_MIN) || (ch > TCP_CH_NR_MAX))
	{
		printf("Invalid thermocouple channel number!\n");
		return ERROR;
	}

	buff = val;
	if (FAIL == i2cMem8Write(dev, TCP_TYPE1 + ch - 1, &buff, 1))
	{
		return ERROR;
	}

	return OK;
}

int doSnsTypeRead(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 4)
	{
		ch = atoi(argv[3]);
		if (OK != sensorTypeRead(dev, ch, &val))
		{
			printf("Fail to read!\n");
			return ERROR;
		}
		if (val < TC_TYPE_B || val > TC_TYPE_T)
		{
			printf("Unknown thermocouple type!\n");

		}
		printf("%s\n", tcTypes[val]);
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}

int doSnsTypeWrite(int argc, char *argv[])
{
	int ch = 0;
	int val = 0;
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}
	if (argc == 5)
	{
		ch = atoi(argv[3]);
		val = atoi(argv[4]);

		if (OK != sensorWrite(dev, ch, val))
		{
			printf("Fail to write!\n");
			return ERROR;
		}
		printf("OK\n");
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}

int doFiltSizeRd(int argc, char *argv[])
{
	int dev = 0;
	u8 buff;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 3)
	{
		if (FAIL == i2cMem8Read(dev, I2C_MAV_FILT_SIZE, &buff, 1))
		{
			printf("Fail to read!\n");
			return ERROR;
		}

		printf("%d\n", (int)buff);
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}

int doFiltSizeWrite(int argc, char *argv[])
{
	int dev = 0;
	int val = 0;
	u8 buff;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 4)
	{
		val = atoi(argv[3]);
		if (val < 1 || val > 40)
		{
			printf("Invalid filter size value [1..40]\n");
			return ERROR;
		}
		buff = 0xff & val;
		if (FAIL == i2cMem8Write(dev, I2C_MAV_FILT_SIZE, &buff, 1))
		{
			printf("Fail to write!\n");
			return ERROR;
		}

		printf("Done\n");
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}

void usage(void)
{
	int i = 0;
	while (gCmdArray[i] != NULL)
	{
		if (gCmdArray[i]->name != NULL)
		{
			if (strlen(gCmdArray[i]->usage1) > 2)
			{
				printf("%s", gCmdArray[i]->usage1);
			}
			if (strlen(gCmdArray[i]->usage2) > 2)
			{
				printf("%s", gCmdArray[i]->usage2);
			}
		}
		i++;
	}
	printf("Where: <id> = Board level id = 0..7\n");
	printf("Type smtc -h <command> for more help\n");
}

int main(int argc, char *argv[])
{
	int i = 0;
	int ret = OK;

	if (argc == 1)
	{
		usage();
		return -1;
	}
#ifdef THREAD_SAFE
	sem_t *semaphore = sem_open("/SMI2C_SEM", O_CREAT, 0666, 1);
	int semVal = 2;
	sem_wait(semaphore);
#endif
	while (NULL != gCmdArray[i])
	{
		if ( (gCmdArray[i]->name != NULL) && (gCmdArray[i]->namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i]->namePos], gCmdArray[i]->name) == 0)
			{
				ret = gCmdArray[i]->pFunc(argc, argv);
				if (ret == ARG_CNT_ERR)
				{
					printf("Invalid parameters number!\n");
					printf("%s", gCmdArray[i]->usage1);
					if (strlen(gCmdArray[i]->usage2) > 2)
					{
						printf("%s", gCmdArray[i]->usage2);
					}
				}
				else if (ret == ARG_ERR)
				{
					printf("%s", gCmdArray[i]->usage1);
					if (strlen(gCmdArray[i]->usage2) > 2)
					{
						printf("%s", gCmdArray[i]->usage2);
					}
				}
#ifdef THREAD_SAFE
				sem_getvalue(semaphore, &semVal);
				if (semVal < 1)
				{
					sem_post(semaphore);
				}
#endif
				return ret;
			}
		}
		i++;
	}
	printf("Invalid command option\n");
	usage();
#ifdef THREAD_SAFE
	sem_getvalue(semaphore, &semVal);
	if (semVal < 1)
	{
		sem_post(semaphore);
	}
#endif
	return -1;
}
