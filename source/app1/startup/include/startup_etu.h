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

	Version: 20240120

	Bare-metal C startup initialisations for the Intel Cyclone V SoC (HPS), ARM Cortex-A9.

	L1_CACHE_ENABLE: 0 = disable L1 cache, 1 = disable, invalidate and enable L1 cache, 2 = do nothing
	L2_CACHE_ENABLE: 0 = disable L2 cache, 1 = disable, invalidate and enable L2 cache, 2 = do nothing

	References:
		- Cyclone V SoC: Cyclone V Hard Processor System Technical Reference Manual
*/

#ifndef STARTUP_ETU_H
#define STARTUP_ETU_H

// We do not want cache in DEBUG mode
#ifdef DEBUG
	#ifndef CLEAN_CACHE
		#define CLEAN_CACHE (0)
	#endif
	#ifndef MMU_ENABLE
		#define MMU_ENABLE (0)
	#endif
	#ifndef SMP_COHERENCY_ENABLE
		#define SMP_COHERENCY_ENABLE (0)
	#endif
	#ifndef L1_CACHE_ENABLE
		#define L1_CACHE_ENABLE (0)
	#endif
	#ifndef L2_CACHE_ENABLE
		#define L2_CACHE_ENABLE (0)
	#endif
	#ifndef SCU_ENABLE
		#define SCU_ENABLE (0)
	#endif
#else
	#ifndef CLEAN_CACHE
		#define CLEAN_CACHE (1)
	#endif
	#ifndef MMU_ENABLE
		#define MMU_ENABLE (0)
	#endif
	#ifndef SMP_COHERENCY_ENABLE
		#define SMP_COHERENCY_ENABLE (0)
	#endif
	#ifndef L1_CACHE_ENABLE
		#define L1_CACHE_ENABLE (2)
	#endif
	#ifndef L2_CACHE_ENABLE
		#define L2_CACHE_ENABLE (2)
	#endif
	#ifndef SCU_ENABLE
		#define SCU_ENABLE (0)
	#endif
#endif

// This should match with your compiler/linker flag
#ifndef NEON_ENABLE
	#define NEON_ENABLE (1)
#endif

#endif
