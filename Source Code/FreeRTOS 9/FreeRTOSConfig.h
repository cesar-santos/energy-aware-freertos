// Modified for multiport portability

/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef __MSP430F247__
#include "msp430.h"

/*
Two interrupt examples are provided -

 + Method 1 does everything in C code.
 + Method 2 uses an assembly file wrapper.

Code size:
Method 1 uses assembly macros to save and restore the task context, whereas
method 2 uses functions. This means method 1 will be faster, but method 2 will
use less code space.

Simplicity:
Method 1 is very simplistic, whereas method 2 is more elaborate. This
elaboration results in the code space saving, but also requires a slightly more
complex procedure to define interrupt service routines.

Interrupt efficiency:
Method 1 uses the compiler generated function prologue and epilogue code to save
and restore the necessary registers within an interrupt service routine (other
than the RTOS tick ISR). Should a context switch be required from within the ISR
the entire processor context is saved. This can result in some registers being saved
twice - once by the compiler generated code, and then again by the FreeRTOS code.
Method 2 saves and restores all the processor registers within each interrupt service
routine, whether or not a context switch actually occurs. This means no registers
ever get saved twice, but imposes an overhead on the occasions that no context switch
occurs.
*/

#define configINTERRUPT_EXAMPLE_METHOD 1
#endif

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

//#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
//#define configUSE_TICKLESS_IDLE                 0
#define configUSE_PREEMPTION		        1
#define configUSE_IDLE_HOOK		        1
#define configUSE_TICK_HOOK			0

/* For each MSP430, a different maximum frequency. Check datasheet */
#ifdef __MSP430F247__
	#define configCPU_CLOCK_HZ		( 16000000UL )
#endif
#ifdef __MSP430F5172__
	#define configCPU_CLOCK_HZ		( 25000000UL )
	#define configLFXT_CLOCK_HZ       	( 32768L )
#endif
#ifdef CPU_MKL25Z128VLK4
        #define configCPU_CLOCK_HZ              (SystemCoreClock)
#endif

#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		        ( 3 )

#ifdef __MSP430F247__
    #define configMINIMAL_STACK_SIZE    ( ( unsigned short ) 50 )
#endif
#ifdef __MSP430F5172__
    #if __DATA_MODEL__ == __DATA_MODEL_SMALL__
            #define configMINIMAL_STACK_SIZE	( ( unsigned short ) 110 )
    #else
            #define configMINIMAL_STACK_SIZE	( ( unsigned short ) 80 )
    #endif
#endif
#ifdef CPU_MKL25Z128VLK4
    #define configMINIMAL_STACK_SIZE    ((unsigned short)90)
    //#define configTOTAL_HEAP_SIZE       ((size_t)(10240))
	#define configTOTAL_HEAP_SIZE       ((size_t)(4800))
#else
    #define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 1600 ) )
#endif


#define configMAX_TASK_NAME_LEN		        ( 8 )
#define configUSE_TRACE_FACILITY	        0

#ifdef CPU_MKL25Z128VLK4
    #define configUSE_16_BIT_TICKS              0
#else
    #define configUSE_16_BIT_TICKS              1
#endif
#define configIDLE_SHOULD_YIELD                 1

#ifdef __MSP430F5172__
    #define configUSE_MUTEXES                       0
    #define configQUEUE_REGISTRY_SIZE               0
    //#define configGENERATE_RUN_TIME_STATS 	    1
    #define configCHECK_FOR_STACK_OVERFLOW          0
    #define configUSE_RECURSIVE_MUTEXES             0
    #define configUSE_MALLOC_FAILED_HOOK            0
    #define configUSE_APPLICATION_TASK_TAG          0
#endif
#ifdef CPU_MKL25Z128VLK4
    #define configUSE_TASK_NOTIFICATIONS            0
    #define configUSE_MUTEXES                       0
    #define configUSE_RECURSIVE_MUTEXES             0
    #define configUSE_COUNTING_SEMAPHORES           0
    #define configUSE_ALTERNATIVE_API               0 /* Deprecated! */
    #define configQUEUE_REGISTRY_SIZE               0
    #define configUSE_QUEUE_SETS                    0
    #define configUSE_TIME_SLICING                  0
    #define configUSE_NEWLIB_REENTRANT              0
    #define configENABLE_BACKWARD_COMPATIBILITY     0
    #define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5
    #define configCHECK_FOR_STACK_OVERFLOW          0
    #define configUSE_MALLOC_FAILED_HOOK            0
    #define configUSE_APPLICATION_TASK_TAG          0
    #define configGENERATE_RUN_TIME_STATS 	    	0
    #define configUSE_STATS_FORMATTING_FUNCTIONS    1

    /* Memory allocation related definitions. */
    #define configSUPPORT_STATIC_ALLOCATION         0
    #define configSUPPORT_DYNAMIC_ALLOCATION        1
    #define configAPPLICATION_ALLOCATED_HEAP        0

    /* Software timer related definitions. */
    #define configUSE_TIMERS                        0
    #define configTIMER_TASK_PRIORITY               0
    #define configTIMER_QUEUE_LENGTH                0
    #define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE * 2)

    /* Interrupt nesting behaviour configuration. Cortex-M specific. */
    #ifdef __NVIC_PRIO_BITS
        /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
        #define configPRIO_BITS __NVIC_PRIO_BITS
    #else
        #define configPRIO_BITS 4 /* 15 priority levels */
    #endif

    /* The lowest interrupt priority that can be used in a call to a "set priority"
    function. */
    #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY ((1U << (configPRIO_BITS)) - 1)

    /* The highest interrupt priority that can be used by any interrupt service
    routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
    INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
    PRIORITY THAN THIS! (higher priorities are lower numeric values. */
    #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

    /* Interrupt priorities used by the kernel port layer itself.  These are generic
    to all Cortex-M ports, and do not rely on any particular library functions. */
    #define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
    /* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
    See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

    /* Define to trap errors during development. */
    #define configASSERT(x) if((x) == 0) {taskDISABLE_INTERRUPTS(); for (;;);}
#endif

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		        0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete			0
#define INCLUDE_vTaskCleanUpResources	        0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay			1

#ifdef CPU_MKL25Z128VLK4
    #define INCLUDE_xResumeFromISR                  1
    #define INCLUDE_xTaskGetSchedulerState          0
    #define INCLUDE_xTaskGetCurrentTaskHandle       0
    #define INCLUDE_uxTaskGetStackHighWaterMark     0
    #define INCLUDE_xTaskGetIdleTaskHandle          0
    #define INCLUDE_eTaskGetState                   0
    #define INCLUDE_xEventGroupSetBitFromISR        0
    #define INCLUDE_xTimerPendFunctionCall          0
    #define INCLUDE_xTaskAbortDelay                 0
    #define INCLUDE_xTaskGetHandle                  0
    #define INCLUDE_xTaskResumeFromISR              1

    /* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
    standard names. */
    #define vPortSVCHandler SVC_Handler
    #define xPortPendSVHandler PendSV_Handler
    #define xPortSysTickHandler SysTick_Handler
#endif

#ifdef __MSP430F247__
    #define INCLUDE_uxTaskGetStackHighWaterMark     0
#endif

#ifdef __MSP430F5172__
    /* This demo makes use of one or more example stats formatting functions.  These
    format the raw data provided by the uxTaskGetSystemState() function in to human
    readable ASCII form.  See the notes in the implementation of vTaskList() within
    FreeRTOS/Source/tasks.c for limitations. */
    #define configUSE_STATS_FORMATTING_FUNCTIONS	1

    /* The MSP430X port uses a callback function to configure its tick interrupt.
    This allows the application to choose the tick interrupt source.
    configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
    interrupt vector for the chosen tick interrupt source.  This implementation of
    vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
    case configTICK__VECTOR is set to TIMER0_A0_VECTOR. */
    #define configTICK_VECTOR		        TIMER0_A0_VECTOR

    /* Prevent the following definitions being included when FreeRTOSConfig.h
    is included from an asm file. */
    #ifdef __ICC430__
            extern void vConfigureTimerForRunTimeStats( void );
            extern volatile unsigned long ulStatsOverflowCount;
    #endif /* __ICCARM__ */

    /* Configure a 16 bit timer to generate the time base for the run time stats.
    The timer is configured to interrupt each time it overflows so a count of
    overflows can be kept - that way a 32 bit time value can be constructed from
    the timers current count value and the number of overflows. */
    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()

    /* Construct a 32 bit time value for use as the run time stats time base.  This
    comes from the current value of a 16 bit timer combined with the number of times
    the timer has overflowed. */
    #define portALT_GET_RUN_TIME_COUNTER_VALUE( ulCountValue )						\
    {																			\
            /* Stop the counter counting temporarily. */							\
            TA1CTL &= ~MC__CONTINOUS;												\
                                                                                                                                                            \
            /* Check to see if any counter overflow interrupts are pending. */		\
            if( ( TA1CTL & TAIFG ) != 0 )											\
            {																		\
                    /* An overflow has occurred but not yet been processed. */			\
                    ulStatsOverflowCount++;												\
                                                                                                                                                            \
                    /* Clear the interrupt. */											\
                    TA1CTL &= ~TAIFG;													\
            }																		\
                                                                                                                                                            \
            /* Generate a 32 bit counter value by combinging the current peripheral	\
            counter value with the number of overflows. */							\
            ulCountValue = ( ulStatsOverflowCount << 16UL );						\
            ulCountValue |= ( unsigned long ) TA1R;									\
            TA1CTL |= MC__CONTINOUS;												\
    }
#endif

#endif /* FREERTOS_CONFIG_H */
