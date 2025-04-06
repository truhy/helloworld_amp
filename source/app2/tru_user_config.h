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

	Version: 20250405

	Trulib user configuration
*/

#ifndef TRU_USER_CONFIG_H
#define TRU_USER_CONFIG_H

#include "tru_options.h"

// ====================
// User config settings
// ====================

#define TRU_CFG_TARGET                  TRU_TARGET_C5SOC
#define TRU_CFG_CMSIS                   0U
#define TRU_CFG_CMSIS_WEAK_IRQH         0U  // This is to support FreeRTOS with CMSIS, set to 1 when using FreeRTOS, else set to 0
#define TRU_CFG_STARTUP                 1U
#define TRU_CFG_EXIT_TO_UBOOT           0U
#define TRU_CFG_NEON                    1U
#define TRU_CFG_UNALIGNED_ACCESS        1U
#define TRU_CFG_PRINT_UART0             1U
#define TRU_CFG_LOG                     1U
#define TRU_CFG_LOG_RN                  1U
#define TRU_CFG_LOG_LOC                 0U
#define TRU_CFG_DMA_BUFFER_NONCACHEABLE 0U
#define TRU_CFG_CLEAN_CACHE             0U

#endif
