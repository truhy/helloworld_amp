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

	Version: 20242701
	Program: Hello, World! AMP for core 0
	Target : ARM Cortex-A9 on the DE10-Nano development board (Intel Cyclone V SoC FPGA)
	Type   : Bare-metal C

	About
	=====

	An asymmetric multiprocessing (AMP) version of "Hello, World!".

	This sample consists of two separate Eclipse IDE embedded C projects or two
	applications, app1 and app2.

	Each run independently on one core of the Cortex-A9.  It is for the SoC
	(aka HPS or CPU) part of the FPGA so it should work on other Cyclone V
	SoC-FPGA dev boards, e.g.: DE1-SoC, DE-10 Standard, Arrow SoCKit, etc.

	On a cold or warm reset, core 0 is in a running state and core 1 is kept in
	the reset state.  App1 running on core 0 will release core 1 from reset.

	Makes use the Intel/Altera HWLib bare-metal library for some of the low
	level hardware access, and these are identified by their naming prefix
	"alt_" or "ALT_".

	HWLib's linker and startup code do not support the second core (core 1),
	so included is my own GNU linker script and startup source files.

	Limitations
	===========

	It seems the GDB & OpenOCD plugin for "Eclipse IDE for Embedded C/C++" do
	not support AMP debugging, i.e. we cannot debug both cores in a debug
	session.  We can only debug one core at a time.  If we debug on core 0,
	breakpoints on the other core (core 1) are ignored, they are not
	captured.  This is same the opposite way.
*/

#include "tru_cortex_a9.h"
#include "tru_c5_uart.h"
#include "tru_logger.h"
#include "socal/hps.h"
#include "socal/socal.h"
#include "socal/alt_sysmgr.h"
#include "socal/alt_rstmgr.h"
#include "alt_cache.h"
#include <string.h>

#ifdef SEMIHOSTING
	extern void initialise_monitor_handles(void);  // Reference function header from the external Semihosting library
#endif

#if(TRU_EXIT_TO_UBOOT)
	extern long unsigned int uboot_lr;
	extern long unsigned int uboot_sp;
	extern int uboot_argc;
	extern char **uboot_argv;
#endif

// =============================
// "Hello, World!" demonstration
// =============================

char app1_message[] = "App 1: Hello, World! (AMP, running on core x)\r\n";

void tx_hello(void){
	uint32_t mpidr;
	__read_mpidr(mpidr);  // Read MPIDR register to get current processor number
	char corenum_ascii = (mpidr & 0x3) + 48;

	// Transmit message
	app1_message[strlen(app1_message) - 4] = corenum_ascii;
	tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, app1_message, strlen(app1_message));
}

// By default, U-Boot enables the L1 D-cache, before enabling core 1 we need to
// clean the cache.  Clean, writes the dirty cache lines back into memory so
// that core 1 will see those changes that core 0 made to the memory
void release_core1(void){
	// Not needed because we did this already in the reset handler
	//alt_cache_l1_data_clean_all();  // Required if L1 d-cache was enabled (i.e. in U-Boot), this is because core 0 loaded app2 into memory and it may be cached and dirty
	//alt_cache_l2_clean_all();       // Required if L2 cache was was enabled (i.e. in U-Boot), this is because core 0 loaded app2 into memory and it may be cached and dirty

	alt_write_word(ALT_RSTMGR_MPUMODRST_ADDR, alt_read_word(ALT_RSTMGR_MPUMODRST_ADDR) & ALT_RSTMGR_MPUMODRST_CPU1_CLR_MSK);  // Put CPU1 out of reset, i.e. enable it (default start address is 0)
}

void delay(void){
	for(volatile uint32_t i = 0; i < 10000000; i++);
}

// ====================================
// U-Boot input arguments demonstration
// ====================================

enum message_id{
	MSG_INPUTS,
	MSG_ARGC,
	MSG_ARGV,
	MSG_NEWLINE,
	MSG_NONE,
	MSG_EXIT
};

// Messages
const char *messages[] = {
	"Arguments from U-Boot:\r\n",
	"argc: 0x",
	"argv: ",
	"\r\n",
	"none\r\n",
	"Exiting app1..\r\n"
};

// Transmit CLI arguments
void tx_cli_args(int argc, char *const argv[]){
	tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_INPUTS], strlen(messages[MSG_INPUTS]));

	// Transmit input arguments count from U-Boot
	tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_ARGC], strlen(messages[MSG_ARGC]));
	tru_c5_uart_write_inthex(TRU_C5_UART0_BASE_ADDR, argc, 32);
	tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_NEWLINE], strlen(messages[MSG_NEWLINE]));

	if(argc){
		// Transmit input argument value from U-Boot
		for(int i = 0; i < argc; i++){
			tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_ARGV], strlen(messages[MSG_ARGV]));
			tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, argv[i], strlen(argv[i]));
			tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_NEWLINE], strlen(messages[MSG_NEWLINE]));
		}
	}else{
		tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_NONE], strlen(messages[MSG_NONE]));
	}
}

int main(int argc, char **argv){
	#ifdef SEMIHOSTING
		initialise_monitor_handles();  // Initialise Semihosting
	#endif

#if(TRU_EXIT_TO_UBOOT)
	tx_cli_args(uboot_argc, uboot_argv);
	tx_hello();
	tru_c5_uart_wait_empty(TRU_C5_UART0_BASE_ADDR);  // Wait for messages to empty out of UART

	// Release core 1 from reset so that it starts executing app2
	release_core1();
	delay();  // Wait for core 1 to finish outputting its messages.  TODO: instead of brute-force wait, implement Inter-process communication (IPC) or interrupts/events

	tru_c5_uart_write_str(TRU_C5_UART0_BASE_ADDR, messages[MSG_EXIT], strlen(messages[MSG_EXIT]));
	tru_c5_uart_wait_empty(TRU_C5_UART0_BASE_ADDR);  // Wait for messages to empty out of UART
#else
	tx_hello();
	tru_c5_uart_wait_empty(TRU_C5_UART0_BASE_ADDR);  // Wait for messages to empty out of UART

	// Release core 1 from reset so that it starts executing app2
	release_core1();
#endif

	return 0xa9;
}

// Exit to U-Boot
void __attribute__((naked)) etu(int rc){
	__asm__(
		// Restore system stack pointer
		"LDR r3, =uboot_sp     \n"
		"LDR sp, [r3]          \n"

		// Restore return address
		"LDR r3, =uboot_lr     \n"
		"LDR lr, [r3]          \n"

		// Return to U-Boot
		"BX lr                 \n"
	);
}

// Override newlib _exit()
void __attribute__((noreturn)) _exit(int status){
#if(TRU_EXIT_TO_UBOOT)
	etu(status);
#endif

	//DEBUG_PRINTF("DEBUG: Starting infinity loop"_NL);
	while(1);
}
