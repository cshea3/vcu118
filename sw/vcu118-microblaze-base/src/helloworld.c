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

ringBuffer *myRingBuffer;
int main()
{

	// Initialize the ring buffer
	value = 0;
	xil_printf("Entered the main loop\r\n");
	init_platform();
    int Status = XST_SUCCESS;
   	run = true;
   	run_function = false;
   	led_cmd = 0;
   	xil_printf("called the init platform\r\n");
   	//setup the BRAM
   	ConfigPtr = XBram_LookupConfig(BRAM_DEVICE_ID);
   	xil_printf("Returned from bram config\r\n");
   	/*
   	 * Run the UartLite Interrupt example, specify the Device ID that is
   	 * generated in xparameters.h.
   	 */
   	xil_printf("Initalize the BRAM \r\n");
   	BramInit(BRAM_DEVICE_ID);
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
	//test the DDR4 memory
	xil_printf("Testing the DDR4 memory access with DEADBEFF");

	u32 *BaseAddress = XPAR_MIG_0_BASEADDR;
	xil_printf("Base address was set to %d\r\n",XPAR_MIG_0_BASEADDR);
	Status = Xil_TestMem32(BaseAddress, 200, 0, XIL_TESTMEM_ALLMEMTESTS);
	xil_printf("Returned from Xil_TestMem32 \r\n");

	if(Status == XST_SUCCESS)
		xil_printf("Memory test was sucessfull\r\n");

	char message[] = "Please \r\n x to end program \r\n";
	int len = strlen(message);
	xil_printf("Yeah %d\r\n",len);
	for (int i =0;i<len;i++){
		SendBuffer[i] = message[i];
	}
	process = false;
	XUartLite_Send(&UartLite, SendBuffer, len);
	//set the size of the receive buffer so that uart handler returns without waiting
	clear_buffer();
	//u8 skip = false;
	u8 packetType = 0;

	while(run){
		if(process){
			process = false;
			u32 amount = ReceiveBufferPtr-CommandPtr;
			xil_printf("CommandPtr address is %d\r\n",CommandPtr);
			xil_printf("ReceiveBufferPtr address is %d\r\n",ReceiveBufferPtr);
			xil_printf("Total Received is %d \r\n",amount);
			if (amount > 0){
			for(int i=0;i<amount;i++){
				xil_printf("CommandPtr is %d \r\n",*CommandPtr);
				CommandPtr++;
			}
			CommandPtr -= amount;
			xil_printf("Rewind to the start %d \r\n",*CommandPtr);

			if (*CommandPtr == 0x78){
				run = false;
				xil_printf("We are getting out!\r\n");

			}else if(*CommandPtr == 0x63 || *CommandPtr == 0x64){ //command packet
				xil_printf("Command or Data Packet Received \r\n",*CommandPtr);
				packetType = *CommandPtr;
				xil_printf("The type is %h %d \r\n",packetType, packetType-48);
				Status = process_packet(packetType, amount);
				//advance past the return character
				CommandPtr++;
				//we have processed all the data, we dont need to advance anymore

			}else
				CommandPtr+=amount;
			}
		}
	}
	xil_printf("Successfully ran Microblaze based LED interrupt example\r\n");
    cleanup_platform();
    return Status;
}
/*
int process_command(){

					run_function = true;
					cylon_leds();
					xil_printf("Returned from cyclon_leds function\r\t");
					//receiveCount = 1;
				}
	return XST_SUCCESS;
}

int process_data(){
	return XST_SUCCESS;
}
*/

int process_packet(u8 packeType, u32 recBytes){
	//check the packet type
	xil_printf("Call to process_packet \r\n");
	//wait for a process
	u8 type = *CommandPtr;
	CommandPtr++;

	xil_printf("Call the num_convert and get the payload size\r\n");
	//get the bytes in the payload
	u32 bytes=num_convert();
	//check that this is equal to the number received
	//recBytes includes the command and byte size
	xil_printf("Total Received is %d vs the bytes declared in the packet %d\r\n", TotalReceivedCount, bytes);
	//if (recBytes != bytes)
	//	return XST_FAILURE;

	//payload bytes are 4 less then the bytes
	payloadPackett.bytes=bytes;
	//index = 0;
	//process the payload
	xil_printf("Processing the payload of %d bytes \r\n",payloadPackett.bytes);
	//u8 first = 1;

	//types of packets
	//0x77 write to memory first u32 word, start address second word, inc address by third word, 0-256 words data payload
	//0x72 read from memory is first u32 word, start address second word, inc by address third word, number of words to read fourth word
	//0x6d manual led is first u32 word, value to write to leds
	//0x73 walk the leds is the first u32 word
	//0x75 cylon led is the first u32 word
	//get the command
	xil_printf("What is the CommandPtr address? %d \r\n",CommandPtr);
	xil_printf("what is the max ReceiveBufferPtr? %d \r\n", ReceiveBufferPtr);
	u32 value = ReceiveBufferPtr - CommandPtr;
	xil_printf("Diff is %d \r\n",value);
	//for(int z=0;z<value;z++){
	//	xil_printf("value is %d \r\n", *CommandPtr);
	//	CommandPtr++;
	//}

	bytes = num_convert();
	payloadPackett.command = bytes;
	xil_printf("Payload command is %d\r\n",bytes);
	xil_printf("Payload command is %d\r\n",payloadPackett.command);
	process_command();
	return XST_SUCCESS;

}

int process_command(){
	xil_printf("Entered into the process_command\r\n");
	xil_printf("The command to process is %d \r\n", payloadPackett.command);
	if ( payloadPackett.command == 0x1d){
		xil_printf("Made it to command processing\r\n");
		//get the start address
		payloadPackett.address = num_convert();
		xil_printf("The base address is %d \r\n",payloadPackett.address);
		payloadPackett.increment = num_convert();
		xil_printf("The address increment is %d \r\n",payloadPackett.increment);
		u8 index = 0;
		payloadPackett.bytes-=12; //remove the number of bytes for the address and the increment
		if(payloadPackett.bytes%4 == 0 ){
			for (int i=0;i<payloadPackett.bytes;i+=4){
				payloadPackett.payload[index++] = num_convert();
				xil_printf("The payload has the following numbers %d\r\n",payloadPackett.payload[index-1]);
			}

		}
		else
			return XST_FAILURE;
		bramWrite();
	}else if (payloadPackett.command == 0x1e){
		payloadPackett.address = num_convert();
		xil_printf("The base address is %d \r\n",payloadPackett.address);
		payloadPackett.increment = num_convert();
		xil_printf("The address increment is %d \r\n",payloadPackett.increment);
		//number of words to read out
		payloadPackett.payload[0] = num_convert();
		bramRead();
		xil_printf("Returned from function bramRead");
	}else if (payloadPackett.command == 0x27){ //W
		//how many times to walk the LEDs
		payloadPackett.payload[0] = num_convert();
		walk_leds();
		xil_printf("Returned from walk leds function\r\t");
	}else if (payloadPackett.command == 0x23){ //S
		//value to set
		payloadPackett.payload[0] = num_convert();
		manual_leds();
		xil_printf("Returned from manual_leds function\r\n");
	}else if (payloadPackett.command == 0x13){
		//how many times should the cylon pattern run
		payloadPackett.payload[0] = num_convert();
		cylon_leds();
		xil_printf("Returned from cyclon_leds function\r\n");
	}
	return XST_SUCCESS;
}




//not really necessary, but clear the receive buffer after using it.
int clear_buffer(){
	for (int i = 0;i<TEST_BUFFER_SIZE;i++)
		ReceiveBuffer[i] = 0;
	return XST_SUCCESS;
}

int BramInit(u16 DeviceId)
{
	int Status;


	/*
	 * Initialize the BRAM driver. If an error occurs then exit
	 */

	/*
	 * Lookup configuration data in the device configuration table.
	 * Use this configuration info down below when initializing this
	 * driver.
	 */
	ConfigPtr = XBram_LookupConfig(DeviceId);
	if (ConfigPtr == (XBram_Config *) NULL) {
		return XST_FAILURE;
	}

	Status = XBram_CfgInitialize(&Bram, ConfigPtr,
				     ConfigPtr->CtrlBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


        InitializeECC(ConfigPtr, ConfigPtr->CtrlBaseAddress);


	/*
	 * Execute the BRAM driver selftest.
	 */
	Status = XBram_SelfTest(&Bram, 0);
	xil_printf("DataWidth is %d\r\n",ConfigPtr->DataWidth);
	xil_printf("MemBaseAddress is %x\r\n",ConfigPtr->MemBaseAddress);
	xil_printf("MemBaseAddress is %x\r\n",ConfigPtr->MemHighAddress);
	xil_printf("Write Access is %d\r\n",ConfigPtr->WriteAccess);


	//XBram_WriteReg(BaseAddress, RegOffset, Data)
	//XBram_ReadReg(BaseAddress, RegOffset)
	xil_printf("Let's write some data to the BRAM %x\r\n",0xDEADBEEF);
	XBram_WriteReg(ConfigPtr->MemBaseAddress,0x0,0xDEADBEEF);
	u32 data = XBram_ReadReg(ConfigPtr->MemBaseAddress,0x0);
	xil_printf("Read back from the address a value of %x",data);


	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


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
	/*
	//amount of data (u8) received from the buffer
		TotalReceivedCount = EventData;
		//read the data out of the fifo, blocks until receiveCount is meet
		xil_printf("ReceiveCount = %d\r\n",receiveCount);
		XUartLite_Recv(&UartLite, ReceiveBuffer, receiveCount);
*/
	XUartLite_Recv(&UartLite, ReceiveBufferPtr, 1);

	xil_printf("%x\r\n",*ReceiveBufferPtr);
	if(*ReceiveBufferPtr == 0xd){
		process=true;
	}
	ReceiveBufferPtr++;
	TotalReceivedCount++;


	//If we've reached the end of the buffer, start over
    if (ReceiveBufferPtr >= (&ReceiveBuffer[0] + TEST_BUFFER_SIZE)){
      xil_printf("Resetting Receive Buffer. Please enter a new command!\n\r");
      ReceiveBufferPtr = &ReceiveBuffer[0];
      CommandPtr = &ReceiveBuffer[0];
      TotalReceivedCount = 0;
    }
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
	u16 LED = 0x1;

	bool dir = true;
	u8 k;

    print("Display 'Cylon' pattern on LEDS\r\n");

    for (int jj=0;jj<payloadPackett.payload[0];jj++){
    	for (k=0; k<8; k++) {
        	XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, LED);
       		sleep(1);
       		//set by interrupt and escape back to main menu
            if (dir)
            	LED = LED <<1 ;
            else
            	LED = LED >>1 ;
        }
        if(dir)
        	dir = false;
        xil_printf("Direction is %b \r\n",dir );
    }
    return XST_SUCCESS;
}

//direct write to enable each led from user
int walk_leds()
{
	u16 LED=0x1;
	xil_printf("Walk LEDs\r\n");
	for (int jj=0;jj<100;jj++){	/* Set the LED to High */
		XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, LED);
		/* Wait a small amount of time so the LED is visible */
		sleep(1);
		/* Clear the LED bit */
		//XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x0);
		//XGpio_DiscreteClear(&Gpio, GPIO_CHANNEL, LED);
		/* Wait a small amount of time so the LED is visible */
		//sleep(5000);
		LED = LED << 1;
		xil_printf("LED offset is %d\r\n", LED);

		if (LED > 128)
			LED=1;
	}
	return XST_SUCCESS;
}

int manual_leds()
{
	u16 LED=payloadPackett.payload[0];
	/* Set the LED to High */
	XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, LED);
	return XST_SUCCESS;
}

u32 bramWrite(){
	xil_printf("Now in bramWrite \r\n");
	xil_printf("BaseAddress = %d\r\n",payloadPackett.address);
	xil_printf("Address inc = %d\r\n",payloadPackett.increment);
	u32 base_address = payloadPackett.address;
	u8 idx = 0;
	for(int i =0;i<payloadPackett.bytes;i+=4){
		xil_printf("Writing to base_address %d \r\n", base_address);
		XBram_WriteReg(ConfigPtr->MemBaseAddress,base_address, payloadPackett.payload[idx++]);
		base_address += payloadPackett.increment;
	}
	return XST_SUCCESS;
}

u32 bramRead(){
	u32 base_address = payloadPackett.address;
	u32 data = 0;
	for (int i = 0;i<payloadPackett.payload[0];i++){
		data = XBram_ReadReg(ConfigPtr->MemBaseAddress,base_address+=payloadPackett.increment);
		xil_printf("%d\r\n",data);
	}
	return data;
}

u32 num_convert(){
	xil_printf("Call to num_convert \r\n");
	u32 decimalnumber = 0;

	//xil_printf("ReceiveBuffer[9]=%x\r\n",ReceiveBuffer[9]);
	//if(ReceiveBuffer[9]==13){
	u32 multi = 1000;
	for (int i=0;i<4;i++){
		xil_printf("Received Number is %d\r\n",*CommandPtr);
		decimalnumber += (*CommandPtr-48)*multi;
		xil_printf("Converted Number is %d\r\n",(*CommandPtr-48)*multi);
		multi = multi/10;
		CommandPtr++;
	}

	xil_printf("Dec Number is %d\r\n",decimalnumber);
	return decimalnumber;
}

/****************************************************************************/
/**
*
* This function ensures that ECC in the BRAM is initialized if no hardware
* initialization is available. The ECC bits are initialized by reading and
* writing data in the memory. This code is not optimized to only read data
* in initialized sections of the BRAM.
*
* @param	ConfigPtr is a reference to a structure containing information
*		about a specific BRAM device.
* @param 	EffectiveAddr is the device base address in the virtual memory
*		address space.
*
* @return
*		None
*
* @note		None.
*
*****************************************************************************/
void InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr)
{
	u32 Addr;
	volatile u32 Data;

	if (ConfigPtr->EccPresent &&
	    ConfigPtr->EccOnOffRegister &&
	    ConfigPtr->EccOnOffResetValue == 0 &&
	    ConfigPtr->WriteAccess != 0) {
		for (Addr = ConfigPtr->MemBaseAddress;
		     Addr < ConfigPtr->MemHighAddress; Addr+=4) {
			Data = XBram_In32(Addr);
			XBram_Out32(Addr, Data);
		}
		XBram_WriteReg(EffectiveAddr, XBRAM_ECC_ON_OFF_OFFSET, 1);
	}
}

