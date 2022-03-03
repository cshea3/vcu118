/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "helloworld.h"

int main()
{
    init_platform();
    int Status;
   	run = true;
   	run_function = false;
   	led_cmd = 0;
   	/*
   	 * Run the UartLite Interrupt example, specify the Device ID that is
   	 * generated in xparameters.h.
   	 */

   	UartLite_Cfg = XUartLite_LookupConfig(UARTLITE_DEVICE_ID);
   	Status = UartLiteIntr(UARTLITE_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
    print("Hello World\n\r");
    print("Successfully ran Hello World application");
    usleep(500);
    print("Successfully ran Hello World application");
	//Failure to setup and initialize the GPIO will cause the MB to hang
	//setup the LED controls
	Status = XGpio_Initialize(&Gpio, GPIO_0_DEVICE_ID); //GPIO-0  per block diagram
	//Set the direction of the LED Channels (1 and 2)
	XGpio_SetDataDirection(&Gpio, GPIO_CHANNEL, SET_AS_OUTPUT); //pointer to Gpio, the gpio channel channel

	char message[] = "Please choose the LED pattern\r\n c for Cylon pattern \r\n i for increment \r\n q for quit \r\n x to end program \r\n";
	int len = strlen(message);
	xil_printf("Yeah %d\r\n",len);
	for (int i =0;i<len;i++){
		SendBuffer[i] = message[i];
	}

	XUartLite_Send(&UartLite, SendBuffer, len);
	//set the size of the receive buffer so that uart handler returns without waiting
	receiveCount = 2;

	while(run){
		if (ReceiveBuffer[0] == 'x'){// && ReceiveBuffer[1] =='$')
			Status = clear_buffer();
			run  = false;
		}else if (ReceiveBuffer[0] == 'c'){
			Status = clear_buffer();
			run_function = true;
			cylon_leds();
		}else if (ReceiveBuffer[0] == 's'){
			Status = clear_buffer();
			run_function = true;
			walk_leds();
		}else if (ReceiveBuffer[0] == 'm'){
			Status = clear_buffer();
			run_function = true;
			manual_leds();
		}else if (ReceiveBuffer[0] == 'a'){
		Status = clear_buffer();
			xil_printf("That was an 'a'\r\n");
		}
	}
	xil_printf("Successfully ran Microblaze based LED interrupt example\r\n");
	return XST_SUCCESS;

    cleanup_platform();
    return XST_SUCCESS;
}
//not really necessary, but clear the receive buffer after using it.
int clear_buffer(){
	for (int i = 0;i<TEST_BUFFER_SIZE;i++)
		ReceiveBuffer[i] = 0;
	return XST_SUCCESS;
}
/****************************************************************************/
/**
*
* This function does a minimal test on the UartLite device and driver as a
* design example. The purpose of this function is to illustrate
* how to use the XUartLite component.
*
* This function sends data and expects to receive the same data through the
* UartLite. The user must provide a physical loopback such that data which is
* transmitted will be received.
*
* This function uses interrupt driver mode of the UartLite device. The calls
* to the UartLite driver in the handlers should only use the non-blocking
* calls.
*
* @param	DeviceId is the Device ID of the UartLite Device and is the
*		XPAR_<uartlite_instance>_DEVICE_ID value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
****************************************************************************/
int UartLiteIntr(u16 DeviceId)
{
	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&UartLite, DeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartLite);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the UartLite to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
	Status = SetupInterruptSystem(&UartLite);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the UartLite driver instance as the callback reference so
	 * that the handlers are able to access the instance data.
	 */
	XUartLite_SetSendHandler(&UartLite, SendHandler, &UartLite);
	XUartLite_SetRecvHandler(&UartLite, RecvHandler, &UartLite);

	/*
	 * Enable the interrupt of the UartLite so that interrupts will occur.
	 */
	XUartLite_EnableInterrupt(&UartLite);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function is the handler which performs processing to send data to the
* UartLite. It is called from an interrupt context such that the amount of
* processing performed should be minimized. It is called when the transmit
* FIFO of the UartLite is empty and more data can be sent through the UartLite.
*
* This handler provides an example of how to handle data for the UartLite,
* but is application specific.
*
* @param	CallBackRef contains a callback reference from the driver.
*		In this case it is the instance pointer for the UartLite driver.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void SendHandler(void *CallBackRef, unsigned int EventData)
{
	xil_printf("Call to the transmit handler\r\n");
	TotalSentCount = EventData;
}

/****************************************************************************/
/**
*
* This function is the handler which performs processing to receive data from
* the UartLite. It is called from an interrupt context such that the amount of
* processing performed should be minimized.  It is called data is present in
* the receive FIFO of the UartLite such that the data can be retrieved from
* the UartLite. The size of the data present in the FIFO is not known when
* this function is called.
*
* This handler provides an example of how to handle data for the UartLite,
* but is application specific.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is the instance pointer for the UartLite driver.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void RecvHandler(void *CallBackRef, unsigned int EventData)
{
	//amount of data (u8) received from the buffer
	TotalReceivedCount = EventData;
	//read the data out of the fifo, blocks until receiveCount is meet
	XUartLite_Recv(&UartLite, ReceiveBuffer, receiveCount);
}

/****************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur
* for the UartLite device. This function is application specific since the
* actual system may or may not have an interrupt controller. The UartLite
* could be directly connected to a processor without an interrupt controller.
* The user should modify this function to fit the application.
*
* @param    UartLitePtr contains a pointer to the instance of the UartLite
*           component which is going to be connected to the interrupt
*           controller.
*
* @return   XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note     None.
*
****************************************************************************/
int SetupInterruptSystem(XUartLite *UartLitePtr)
{

	int Status;


	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, UARTLITE_INT_IRQ_ID,
			   (XInterruptHandler)XUartLite_InterruptHandler,
			   (void *)UartLitePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the UartLite can cause interrupts through the interrupt controller.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt for the UartLite device.
	 */
	XIntc_Enable(&InterruptController, UARTLITE_INT_IRQ_ID);

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XIntc_InterruptHandler,
			 &InterruptController);

	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

int cylon_leds() {
	u16 led_value = 1;

	u8 dir = 0;
	u8 k;

    print("Display 'Cylon' pattern on RGB LEDs in R,G,B sequence.  Press BTN3 to exit.\r\n");

    while(run_function){
    	led_value = 1;
        for (k=0; k<8; k++) {
        	XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, led_value);
       		xil_printf("Wrote to GPIO \r\n");
       		usleep(500);
       		xil_printf("Returned some sleep\r\n");
       		//set by interrupt and escape back to main menu
            if (ReceiveBuffer[0] == 'p'){
               	run_function = false;
            	break;
            }
            if (dir == 0)
               	led_value = led_value <<1 ;
            else
              	led_value = led_value >>1 ;

        }
    }
    return XST_SUCCESS;
}

//direct write to enable each led from user
int walk_leds()
{
	u16 LED=0x1;

	while (run_function) {
		/* Set the LED to High */
		XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, LED);
		/* Wait a small amount of time so the LED is visible */
		usleep(500);
		/* Clear the LED bit */
		XGpio_DiscreteClear(&Gpio, GPIO_CHANNEL, LED);
		/* Wait a small amount of time so the LED is visible */
		usleep(500);

		LED = LED << 1;

		if (LED > 255)
			LED=1;

		if (ReceiveBuffer[0] == 'p'){
          	run_function = false;
		}
		xil_printf("LED value = %d\r\n", LED);
	}
	return XST_SUCCESS;
}

int manual_leds()
{
	receiveCount = 3;
	u16 LED=0;

	while (run_function) {
		if (ReceiveBuffer[0] != 0){
			xil_printf("%d%d%d\r\n",ReceiveBuffer[0],ReceiveBuffer[1],ReceiveBuffer[2]);
			LED = ReceiveBuffer[0];
			xil_printf("LED set  to %d \r\n",LED);

		}

		/* Set the LED to High */
		XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, LED);
		/* Wait a small amount of time so the LED is visible */
		usleep(500);
		/* Clear the LED bit */
		//XGpio_DiscreteClear(&Gpio, GPIO_CHANNEL, LED);
		/* Wait a small amount of time so the LED is visible */
		//usleep(500);

		//LED = LED << 1;

		//if (LED > 255)
		//	LED=1;

		if (ReceiveBuffer[0] == 'p'){
          	run_function = false;
		}
		//xil_printf("LED value = %d\r\n", LED);
	}
	return XST_SUCCESS;
}
