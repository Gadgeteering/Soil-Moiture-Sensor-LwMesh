/**
 * \file WSNDemo.c
 *
 * \brief WSNDemo application implementation
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the WSN Demo Application Application
 * The WSNDemo application implements a typical wireless sensor network
 * scenario,
 * in which one central node collects the data from a network of sensors and
 * passes this data over a serial connection for further processing.
 * In the case of the WSNDemo this processing is performed by the WSNMonitor PC
 * application. The BitCloud&reg; Quick Start Guide  provides a detailed
 *description
 * of the WSNDemo application scenario, and instructions on how to use
 * WSNMonitor.
 *  However since BitCloud is a ZigBee&reg; PRO stack, there are a few
 *differences
 * in the protocol:
 * • Device types (Coordinator, Router and End Device) are simulated on the
 * application level; there is no such separation in Lightweight Mesh on the
 * stack level
 * • The value of the extended address field is set equal to the value of the
 * short address field
 * • For all frames, the LQI and RSSI fields are filled in by the coordinator
 * with the values of LQI and RSSI from the received frame. This means that
 *nodes
 * that are not connected to the coordinator directly will have the same values
 * as the last node on the route to the coordinator
 * • Sensor data values are generated randomly on all platforms
 * • Sending data to the nodes on the network is not implemented and not
 * supported in this demo application
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "user_board.h"
#include "config.h"
#include "sys.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "sysTimer.h"
#if APP_ROUTER || APP_ENDDEVICE
#include "sleep_mgr.h"
#if HTU21D_Enable
#include "i2c.h"
// HTU21D Humidity Sensor
#include "HTU21D.h"
#endif
#include "adc_sensors.h"

#endif
#include "commands.h"
#if APP_COORDINATOR
#if !SIO2USB
#include "sio2host.h"
#else
#include "stdio_usb.h"
uint16_t unused;
#define sio2host_putchar(data) stdio_usb_putchar(unused,data)
#endif
#endif
#if SAMD || SAMR21 || SAML21
#include "system.h"
#else
#include "sysclk.h"
#if (LED_COUNT > 0)
#include "led.h"
#endif
#endif
#include "asf.h"
#include "board.h"
#include "wsndemo.h"

uint8_t HTU21D_Valid;
/*****************************************************************************
*****************************************************************************/

#define APP_CAPTION_SIZE  (sizeof(APP_CAPTION) - 1)

/*- Types ------------------------------------------------------------------*/
COMPILER_PACK_SET(1)
typedef struct  AppMessage_t {
	uint8_t commandId;
	uint8_t nodeType;
	uint64_t extAddr;
	uint16_t shortAddr;
	uint32_t softVersion;
	uint32_t channelMask;
	uint16_t panId;
	uint8_t workingChannel;
	uint16_t parentShortAddr;
	uint8_t lqi;
	int8_t rssi;

	struct {
		uint8_t type;
		uint8_t size;
		int32_t battery;
		int32_t temperature;
		int32_t moisture;
		int32_t humidity;
	} sensors;

	struct {
		uint8_t type;
		uint8_t size;
		char text[APP_CAPTION_SIZE];
	} caption;
} AppMessage_t;


COMPILER_PACK_RESET()


#if APP_ROUTER || APP_ENDDEVICE
static NWK_DataReq_t appNwkDataReq;
static SYS_Timer_t appNetworkStatusTimer;
static SYS_Timer_t appCommandWaitTimer;
static bool appNetworkStatus;
#endif

#if APP_COORDINATOR
static uint8_t rx_data[APP_RX_BUF_SIZE];
#endif

static AppMessage_t appMsg;
static SYS_Timer_t appDataSendingTimer;
#define APP_COMMAND_PENDING 0x01
void UartBytesReceived(uint16_t bytes, uint8_t *byte );

/*- Implementations --------------------------------------------------------*/

#if APP_COORDINATOR

/*****************************************************************************
*****************************************************************************/
void UartBytesReceived(uint16_t bytes, uint8_t *byte )
{
	for (uint16_t i = 0; i < bytes; i++) {
		APP_CommandsByteReceived(byte[i]);
	}
}

static void appUartSendMessage(uint8_t *data, uint8_t size)
{
	uint8_t cs = 0;

	sio2host_putchar(0x10);
	sio2host_putchar(0x02);

	for (uint8_t i = 0; i < size; i++) {
		if (data[i] == 0x10) {
			sio2host_putchar(0x10);
			cs += 0x10;
		}

		sio2host_putchar(data[i]);
		cs += data[i];
	}

	sio2host_putchar(0x10);
	sio2host_putchar(0x03);
	cs += 0x10 + 0x02 + 0x10 + 0x03;

	sio2host_putchar(cs);
}

#endif

/*****************************************************************************
*****************************************************************************/
static bool appDataInd(NWK_DataInd_t *ind)
{
	AppMessage_t *msg = (AppMessage_t *)ind->data;
#if (LED_COUNT > 0)
	LED_Toggle(LED_DATA);
#endif
	msg->lqi = ind->lqi;
	msg->rssi = ind->rssi;
#if APP_COORDINATOR
	appUartSendMessage(ind->data, ind->size);

	if (APP_CommandsPending(ind->srcAddr)) {
		NWK_SetAckControl(APP_COMMAND_PENDING);
	}
#endif
	return true;
}

/*****************************************************************************
*****************************************************************************/
static void appDataSendingTimerHandler(SYS_Timer_t *timer)
{
	if (APP_STATE_WAIT_SEND_TIMER == appState) {
		appState = APP_STATE_SEND;
	} else {
		SYS_TimerStart(&appDataSendingTimer);
	}

	(void)timer;
}

#if APP_ROUTER || APP_ENDDEVICE

/*****************************************************************************
*****************************************************************************/
static void appNetworkStatusTimerHandler(SYS_Timer_t *timer)
{
#if (LED_COUNT > 0)
	LED_Toggle(LED_NETWORK);
#endif
	(void)timer;
}

/*************************************************************************//**
*****************************************************************************/
static void appCommandWaitTimerHandler(SYS_Timer_t *timer)
{
	appState = APP_STATE_SENDING_DONE;
	(void)timer;
}

#endif

/*****************************************************************************
*****************************************************************************/
#if APP_ROUTER || APP_ENDDEVICE
static void appDataConf(NWK_DataReq_t *req)
{
#if (LED_COUNT > 0)
	LED_Off(LED_DATA);
#endif

	if (NWK_SUCCESS_STATUS == req->status) {
		if (!appNetworkStatus) {
#if (LED_COUNT > 0)
			LED_On(LED_NETWORK);
#endif
			SYS_TimerStop(&appNetworkStatusTimer);
			appNetworkStatus = true;
		}
	} else {
		if (appNetworkStatus) {
#if (LED_COUNT > 0)
			LED_Off(LED_NETWORK);
#endif
			SYS_TimerStart(&appNetworkStatusTimer);
			appNetworkStatus = false;
		}
	}

	if (APP_COMMAND_PENDING == req->control) {
		SYS_TimerStart(&appCommandWaitTimer);
#if (LED_COUNT > 0)
		LED_Toggle(LED_NETWORK);
#endif
		appState = APP_STATE_WAIT_COMMAND_TIMER;
	} else {
		appState = APP_STATE_SENDING_DONE;
	}
}

#endif

/*****************************************************************************
*****************************************************************************/
static void appSendData(void)
{
#ifdef NWK_ENABLE_ROUTING
	appMsg.parentShortAddr = NWK_RouteNextHop(0, 0);
#else
	appMsg.parentShortAddr = 0;
#endif
#if APP_ROUTER || APP_ENDDEVICE

	if (HTU21D_Valid){
    float temp = HTU21D_readTemperature();
	float humid = HTU21D_readHumidity();
	appMsg.sensors.temperature = temp;
	appMsg.sensors.humidity	   = humid;
	}
	
else
	{appMsg.sensors.temperature =  0xffff;
	appMsg.sensors.humidity	   =  0xffff;
	}
	
	
	uint16_t moisture =moist_adc_read();
	appMsg.sensors.moisture =moisture;
	uint16_t bat =bat_adc_read();
	appMsg.sensors.battery =bat;
	//uint16_t light =light_adc_read();
	//appMsg.sensors.light =light;
	//uint16_t ph =ph_adc_read();
	//appMsg.sensors.ph =ph;
	LED_Toggle(LED_DATA);
	
#endif


#if APP_COORDINATOR
	appUartSendMessage((uint8_t *)&appMsg, sizeof(appMsg));
	SYS_TimerStart(&appDataSendingTimer);
	appState = APP_STATE_WAIT_SEND_TIMER;
#else
	appNwkDataReq.dstAddr = 0;
	appNwkDataReq.dstEndpoint = APP_ENDPOINT;
	appNwkDataReq.srcEndpoint = APP_ENDPOINT;
	appNwkDataReq.options = NWK_OPT_ACK_REQUEST | NWK_OPT_ENABLE_SECURITY;
	appNwkDataReq.data = (uint8_t *)&appMsg;
	appNwkDataReq.size = sizeof(appMsg);
	appNwkDataReq.confirm = appDataConf;
#if (LED_COUNT > 0)
	LED_On(LED_DATA);
#endif
	NWK_DataReq(&appNwkDataReq);

	appState = APP_STATE_WAIT_CONF;
#endif
}

/*************************************************************************//**
*****************************************************************************/
static void appInit(void)
{
	appMsg.commandId            = APP_COMMAND_ID_NETWORK_INFO;
	appMsg.nodeType             = APP_NODE_TYPE;
	appMsg.extAddr              = APP_ADDR;
	appMsg.shortAddr            = APP_ADDR;
	appMsg.softVersion          = 0x01010100;
	appMsg.channelMask          = (1L << APP_CHANNEL);
	appMsg.panId                = APP_PANID;
	appMsg.workingChannel       = APP_CHANNEL;
	appMsg.parentShortAddr      = 0;
	appMsg.lqi                  = 0;
	appMsg.rssi                 = 0;

	appMsg.sensors.type        = 1;
	appMsg.sensors.size        = sizeof(int32_t) * 4;
	appMsg.sensors.battery     = 0;
	appMsg.sensors.temperature = 0;
	//appMsg.sensors.light       = 0;
	appMsg.sensors.humidity		= 0;
	appMsg.sensors.moisture      = 0;

	appMsg.caption.type         = 32;
	appMsg.caption.size         = APP_CAPTION_SIZE;
	memcpy(appMsg.caption.text, APP_CAPTION, APP_CAPTION_SIZE);

	NWK_SetAddr(APP_ADDR);
	NWK_SetPanId(APP_PANID);
	PHY_SetChannel(APP_CHANNEL);
#if (defined(PHY_AT86RF212B) || defined(PHY_AT86RF212))
	PHY_SetBand(APP_BAND);
	PHY_SetModulation(APP_MODULATION);
#endif
	PHY_SetRxState(true);

#ifdef NWK_ENABLE_SECURITY
	NWK_SetSecurityKey((uint8_t *)APP_SECURITY_KEY);
#endif

	NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);

	appDataSendingTimer.interval = APP_SENDING_INTERVAL;
	appDataSendingTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appDataSendingTimer.handler = appDataSendingTimerHandler;
#if (LED_DATA)
	LED_Init(LED_DATA);
#define LED_COUNT 1
#endif
#if LED_NETWORK
	LED_Init(LED_NETWORK);
#endif
#if APP_ROUTER || APP_ENDDEVICE
	appNetworkStatus = false;
	appNetworkStatusTimer.interval = 500;
	appNetworkStatusTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appNetworkStatusTimer.handler = appNetworkStatusTimerHandler;
	SYS_TimerStart(&appNetworkStatusTimer);

	appCommandWaitTimer.interval = NWK_ACK_WAIT_TIME;
	appCommandWaitTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appCommandWaitTimer.handler = appCommandWaitTimerHandler;
#else
#if (LED_COUNT > 0)
	
	LED_On(LED_NETWORK);
#endif
#endif

#ifdef PHY_ENABLE_RANDOM_NUMBER_GENERATOR
	srand(PHY_RandomReq());
#endif

	APP_CommandsInit();

	appState = APP_STATE_SEND;
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	switch (appState) {
	case APP_STATE_INITIAL:
	{
		appInit();
	}
	break;

	case APP_STATE_SEND:
	{
		appSendData();
	}
	break;

	case APP_STATE_SENDING_DONE:
	{
#if APP_ENDDEVICE
		appState = APP_STATE_PREPARE_TO_SLEEP;
#else
		SYS_TimerStart(&appDataSendingTimer);
		appState = APP_STATE_WAIT_SEND_TIMER;
#endif
	}
	break;

#if APP_ENDDEVICE
	case APP_STATE_PREPARE_TO_SLEEP:
	{
		if (!NWK_Busy()) {
			NWK_SleepReq();
			appState = APP_STATE_SLEEP;
		}
	}
	break;

	case APP_STATE_SLEEP:
	{
		sm_sleep(APP_SENDING_INTERVAL / 1000);
		appState = APP_STATE_WAKEUP;
	}
	break;

	case APP_STATE_WAKEUP:
	{
		NWK_WakeupReq();

		/*
		 * #if (LED_COUNT > 0)
		 *    LED_On(LED_NETWORK);
		 #endif*/

		appState = APP_STATE_SEND;
	}
	break;
#endif
	default:
		break;
	}

#if (APP_COORDINATOR)
	uint16_t bytes;
#if !SIO2USB
	if ((bytes = sio2host_rx(rx_data, APP_RX_BUF_SIZE)) > 0) {
		UartBytesReceived(bytes, (uint8_t *)&rx_data);
	}
#else
	uint16_t unused;
	stdio_usb_getchar(unused,bytes);
	if (bytes != 0)
	{
		UartBytesReceived(bytes, (uint8_t *)&rx_data);
	}
#endif
#endif
}

/*****************************************************************************
*****************************************************************************/

/**
 * Init function of the WSNDemo application
 */
void wsndemo_init(void)
{
	
	SYS_Init();
#if APP_ROUTER || APP_ENDDEVICE
	sm_init();
	configure_adc_averaging();
#if HTU21D_Enable
	i2c_init(); //comment out during debug of adc
	HTU21D_Valid=HTU21D_Init();
#endif // HTU21D

#endif
#if APP_COORDINATOR
#if !SIO2USB
	sio2host_init();
#else
	stdio_usb_init();
	stdio_usb_enable();
#endif

#endif
}

/**
 * Task of the WSNDemo application
 * This task should be called in a while(1)
 */
void wsndemo_task(void)
{
	SYS_TaskHandler();
	APP_TaskHandler();
}
