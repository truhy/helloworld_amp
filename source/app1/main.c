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
	Program: Hello, World! AMP for core 0
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

// =============================
// "Hello, World!" demonstration
// =============================

void tx_hello(void){
	uint32_t mpidr;
	__read_mpidr(mpidr);  // Read MPIDR register to get current processor number
	char corenum = (mpidr & 0x3);

	// Transmit message
	printf("App 1: Hello, World! (AMP, running on core %i)\n", corenum);
}

void delay_loop(void){
	volatile uint32_t i = 1000000;
	while(i--);
}

// ====================================
// U-Boot input arguments demonstration
// ====================================

// Transmit CLI arguments
void tx_cli_args(int argc, char *const argv[]){
	printf("Arguments from U-Boot:\n");

	// Transmit input arguments count from U-Boot
	printf("argc: %i\n", argc);

	if(argc){
		// Transmit input argument value from U-Boot
		for(int i = 0; i < argc; i++){
			printf("argv[%i]: %s\n", i, argv[i]);
		}
	}else{
		printf("none\n");
	}
}

// Release Core 1 from being held in reset state, basically enables it.
// Note, if d-cache is enabled, you would need to clean the cache because we
// normally use U-Boot SPL in core 0 to load App2, and so core 1 will not
// see the newly loaded code.  App1 startup code already cleaned the L1 d-cache.
void release_core1(void){
	//L1C_CleanDCacheAll();  // Required if L1 d-cache was enabled (i.e. in U-Boot), this is because core 0 loaded app2 into memory and it may be cached and dirty
	//L2C_CleanInvAllByWay();  // Required if L2 cache was was enabled (i.e. in U-Boot), this is because core 0 loaded app2 into memory and it may be cached and dirty

	uint32_t val = tru_iom_rd32((uint32_t *)0xffd05010) & 0xfffffffd;
	tru_iom_wr32((uint32_t *)0xffd05010, val);
}

int main(int argc, char **argv){
	#ifdef SEMIHOSTING
		initialise_monitor_handles();  // Initialise Semihosting
	#endif

	release_core1();
	delay_loop();  // Wait for core 1 to finish outputting its messages.  TODO: instead of brute-force wait, implement Inter-process communication (IPC) or interrupts/events

#if(TRU_EXIT_TO_UBOOT)
	tx_cli_args(uboot_argc, uboot_argv);
#endif

	tx_hello();
	tru_hps_uart_ll_wait_empty((void *)TRU_HPS_UART0_BASE);  // Wait for messages to empty out of UART

#if(TRU_EXIT_TO_UBOOT)
	printf("Exiting application..\n");
	tru_hps_uart_ll_wait_empty((void *)TRU_HPS_UART0_BASE);  // Wait for messages to empty out of UART
#endif

	return 0xa9;
}
