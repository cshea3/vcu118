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
#include "xbram.h"
#include "xil_testmem.h"

#define TEST_BUFFER_SIZE        1050 //max num bytes per transaction
//platform constants pulled from the xparameters.h file in the design wrapper
#define UARTLITE_DEVICE_ID      XPAR_UARTLITE_0_DEVICE_ID
#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID
#define UARTLITE_INT_IRQ_ID     XPAR_INTC_0_UARTLITE_0_VEC_ID
#define GPIO_0_DEVICE_ID        XPAR_GPIO_0_DEVICE_ID
#define BRAM_DEVICE_ID		    XPAR_BRAM_0_DEVICE_ID


#define GPIO_CHANNEL            0x1 //there is only one channel
#define SET_AS_OUTPUT 0x0



XGpio Gpio; /* The Instance of the GPIO Driver */
XUartLite UartLite;            /* The instance of the UartLite Device */
XUartLite_Config *UartLite_Cfg; /* The instance of the UartLite Config */
XIntc InterruptController;     /* The instance of the Interrupt Controller */
XBram Bram;	/* The Instance of the BRAM Driver */
XBram_Config *ConfigPtr;

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */

u8 SendBuffer[TEST_BUFFER_SIZE];
u8 ReceiveBuffer[TEST_BUFFER_SIZE];

u8* ReceiveBufferPtr = &ReceiveBuffer[0];
u8* CommandPtr       = &ReceiveBuffer[0];
u32 commandPacket;

typedef struct{
	u32 command;
	u32 bytes; // divide by 4 gives number of payload entries
	u32 address;
	u32 increment;
	u32 payload[256];
} dataPacket;


static volatile dataPacket payloadPackett;

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;
static volatile int receiveCount;
static volatile bool run;
static volatile bool run_function;
static volatile int process;
volatile int led_cmd;
volatile unsigned int value;
int cylon_leds();
int walk_leds();
int manual_leds();
int clear_buffer();

//UART Functions
int UartLiteIntr(u16 DeviceId);
int SetupInterruptSystem(XUartLite *UartLitePtr);
void SendHandler(void *CallBackRef, unsigned int EventData);
void RecvHandler(void *CallBackRef, unsigned int EventData);

//BRAM Functions
int BramInit(u16 DeviceId);
void InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr);
u32 bramWrite();
u32 bramRead();
//ascii to hex
u32 num_convert();

// Note power of two buffer size
#define kNumPointsInMyBuffer 1024

typedef struct _ringBuffer {
    u32 currentIndex;
    u32 sizeOfBuffer;
    u32 data[kNumPointsInMyBuffer];
} ringBuffer;

/*
// A little function to write into the buffer
// N.B. First argument of writeIntoBuffer() just happens to have the
// same as the one calloc'ed above. It will only point to the same
// space in memory if the calloc'ed pointer is passed to
// writeIntoBuffer() as an arg when the function is called. Consider
// using another name for clarity
void writeIntoBuffer(ringBuffer *myRingBuffer, u32 *myData, int numsamples) {
    // -1 for our binary modulo in a moment
    int buffLen = myRingBuffer->sizeOfBuffer - 1;
    int lastWrittenSample = myRingBuffer->currentIndex;

    int idx;
    for (int i=0; i < numsamples; ++i) {
        // modulo will automagically wrap around our index
        idx = (i + lastWrittenSample) & buffLen;
        myRingBuffer->data[idx] = myData[i];
    }

    // Update the current index of our ring buffer.
    myRingBuffer->currentIndex += numsamples;
    myRingBuffer->currentIndex &= myRingBuffer->sizeOfBuffer - 1;
}
*/
int process_packet(u8 packeType, u32 bytes);
int process_command();
//packet definition -
//type of packet, size(total bytes), payload (base address, inc, data)
//01 is command, 4 bytes size, payload,
//02 is data 1024 bytes
#endif
