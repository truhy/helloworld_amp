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

	Version: 20240211
	Program: Hello, World! AMP for core 1
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

#include "tru_config.h"
#include "tru_cortex_a9.h"
#include "tru_uart_ll.h"
#include "tru_logger.h"
#include <string.h>

#ifdef SEMIHOSTING
	extern void initialise_monitor_handles(void);  // Reference function header from the external Semihosting library
#endif

char message[] = "App 2: Hello, World! (AMP, running on core x)\r\n";

void tx_hello(void){
	uint32_t mpidr;
	__read_mpidr(mpidr);  // Read MPIDR register to get current processor number
	char corenum_ascii = (mpidr & 0x3) + 48;

	// Transmit message
	message[strlen(message) - 4] = corenum_ascii;
	tru_uart_ll_write_str(TRU_UART0_BASE_ADDR, message, strlen(message));
}

int main(int argc, char **argv){
	#ifdef SEMIHOSTING
		initialise_monitor_handles();  // Initialise Semihosting
	#endif

	tx_hello();

	return 0;
}

// Override newlib _exit()
void __attribute__((noreturn)) _exit(int status){
	//DEBUG_PRINTF("DEBUG: App 2: Starting infinity loop"_NL);
	while(1);
}
