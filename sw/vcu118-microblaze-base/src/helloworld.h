#ifndef __HELLOWORLD_H__
#define __HELLOWORLD_H__
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xuartlite.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include <stdbool.h>
#include "xgpio.h"

#define TEST_BUFFER_SIZE        100 //max num bytes per transaction
//platform constants pulled from the xparameters.h file in the design wrapper
#define UARTLITE_DEVICE_ID      XPAR_UARTLITE_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define UARTLITE_INT_IRQ_ID     XPAR_INTC_0_UARTLITE_0_VEC_ID
#define GPIO_0_DEVICE_ID        XPAR_GPIO_0_DEVICE_ID
#define GPIO_CHANNEL            0x1 //there is only one channel
#define SET_AS_OUTPUT 0x0

XGpio Gpio; /* The Instance of the GPIO Driver */
XUartLite UartLite;            /* The instance of the UartLite Device */
XUartLite_Config *UartLite_Cfg; /* The instance of the UartLite Config */
XIntc InterruptController;     /* The instance of the Interrupt Controller */

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */

u8 SendBuffer[TEST_BUFFER_SIZE];
u8 ReceiveBuffer[TEST_BUFFER_SIZE];

u8* ReceiveBufferPtr = &ReceiveBuffer[0];
u8* CommandPtr       = &ReceiveBuffer[0];

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;
static volatile int receiveCount;
static volatile bool run;
static volatile bool run_function;
volatile int led_cmd;
int cylon_leds();
int walk_leds();
int manual_leds();
int clear_buffer();

int UartLiteIntr(u16 DeviceId);
int SetupInterruptSystem(XUartLite *UartLitePtr);
void SendHandler(void *CallBackRef, unsigned int EventData);
void RecvHandler(void *CallBackRef, unsigned int EventData);

int num_convert();

#endif
