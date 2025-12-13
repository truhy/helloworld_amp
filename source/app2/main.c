/*
	MIT License

	Copyright (c) 2023 Truong Hy

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

	Version: 20251205
	Program: Hello, World! AMP for core 1
	Target : ARM Cortex-A9 on the DE10-Nano Kit development board (Altera
	         Cyclone V SoC FPGA)
	Type   : Stand-alone C application

	About
	=====

	An asymmetric multiprocessing (AMP) version of "Hello, World!".

	This example consists of two separate Eclipse IDE embedded C projects or two
	applications, app1 and app2.

	Each app executes independently on a separate Cortex-A9 core.

	On a cold or warm reset, core 0 is in a running state but core 1 is kept in
	the reset state.  App1 code will release core 1 from reset.

	Limitations
	===========

	It seems GDB & OpenOCD plugin for "Eclipse IDE for Embedded C/C++" does
	not support AMP debugging, i.e. we cannot debug both cores at the same time
	in a debug session.  We can only debug one core at a time.  If we debug on
	core 0, breakpoints on the other core (core 1) are ignored, they are not
	captured.  This is same for the opposite way.
*/

// Trulib includes
#include "tru_config.h"
#include "arm/tru_cortex_a9.h"

// Arm CMSIS includes
#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS

// Standard includes
#include <stdio.h>

#ifdef SEMIHOSTING
	extern void initialise_monitor_handles(void);  // Reference function header from the external Semihosting library
#endif

void tx_hello(void){
	uint32_t mpidr;
	__read_mpidr(mpidr);  // Read MPIDR register to get current processor number
	char corenum = (mpidr & 0x3);

	// Transmit message
	printf("App 2: Hello, World! (AMP, running on core %i)\n", corenum);
}

int main(int argc, char **argv){
	#ifdef SEMIHOSTING
		initialise_monitor_handles();  // Initialise Semihosting
	#endif

	tx_hello();
	tru_hps_uart_ll_wait_empty((void *)TRU_HPS_UART0_BASE);  // Wait for messages to empty out of UART

	return 0;
}
