/**********************************************************************************
 * UNIFEI - Universidade Federal de Itajuba                                       *
 * POSCOMP - Pos-Graduacao em Ciencia e Tecnologia da Computacao                  *
 *                                                                                *
 * Master Thesis: Dynamic Power Saving Module for Real Time Operating Systems     *
 *                                                                                *
 * Authors:     Cesar Augusto Marcelino dos Santos (cesaraugustounifei@gmail.com) *
 *              Rodrigo Maximiano Antunes de Almeida (rmaalmeida@gmail.com)       *
 *              Carlos Henrique Valerio de Moraes (carloshvmoraes@gmail.com)      *
 *                                                                                *
 * This project is licensed by Creative Commons (https://creativecommons.org/),   *
 * demanding author's attribution (BY-type).									  *
 *                                                                                *
 * This is a modified FreeRTOS project. Feel free to use or modify it by your own *
 * risk, without any warranties. Although, you can still contact the authors for  *
 * for questions or new ideas :)                                                  *
 **********************************************************************************/


#ifdef CPU_MKL25Z128VLK4
    #error "DO NOT USE IAR!!! PLEASE USE KDS FOR KL25Z"
#endif

#ifdef __MSP430F247__
    #error "Please use the default linker file option!"
#endif

//#ifdef __MSP430F5172__
    //#error "Remember to override the linker descriptor file!"
//#endif

/* Standard includes. */
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifndef CPU_MKL25Z128VLK4
    /* Hardware includes. */
    #include "msp430.h"
    #include "hal_MSP-EXP430F5438.h"
#else
    #include "board.h"
    #include "pin_mux.h"
    #include "clock_config.h"
	#include "fsl_smc.h"
#endif
/* Just used to ensure parameters are passed into tasks correctly. */
#define mainTASK_PARAMETER_CHECK_VALUE	( ( void * ) 0xDEAD )
/*-----------------------------------------------------------*/



/**********************************************************************
 ***************** MODIFIED FOR POWER SAVING PURPOSES *****************
 **********************************************************************/
/********************************************************
 * Possible values are:                                 *
 * 0	( no load, no FreeRTOS )						*
 * 1    (100% load = 2 tasks with same wakeup time)     *
 * 2    ( 75% load = 3 tasks + idle task)               *
 * 3    ( 50% load = 1 task + idle cycling each tick)   *
 * 5    (  0% load = idle task only)                    *
 *                                                      *
 ********************************************************/
#define LOAD_TEST 5


/************************************************************************
 * Possible values are...                                               *
 * For all microcontrollers/microprocessors:							*
 * -1                   (no LPM at all)                                 *
 * 5                    (choice according to usContextSwitchBitmap)     *
 *																		*
 * For MSP430:															*
 * 0, 1, 2, 3, 4        (LPMx possibilities)                            *
 * 																		*
 * For ARM:																*
 * 0, 1					Wait Mode										*
 * 2, 3					Very Low Power Wait Mode						*
 * 4					Stop Mode										*
 *                                                                      *
 ************************************************************************/
#define LPM 5

// Turns on or off an LED connected to Port 2, pins 2 or 3
extern void vPowerUpLed( portBASE_TYPE arg, portBASE_TYPE bit );

// CSB that assigns when an idle comes out - context switching history
extern volatile uint8_t usContextSwitchBitmap;

// Task signature
void vTaskCode( void * pvParameters );
void vApplicationIdleHook( void );



/*
 * Configures clocks, LCD, port pints, etc. necessary to execute this app.
 */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

#ifndef CPU_MKL25Z128VLK4
	/* The linker script can be used to test the FreeRTOS ports use of 20bit
	addresses by locating all code in high memory. The following pragma ensures
	that main remains in low memory when that is done. The ISR_CODE segment is used
	for convenience as ISR functions are always placed in low memory. */
	#pragma location="ISR_CODE"
#endif
void main( void )
{
	/* Configure the peripherals used by this demo application. This includes
	configuring the joystick input select button to generate interrupts. */
	prvSetupHardware();

	/* Test 1: 100% load */
	#if( LOAD_TEST == 1 )
		xTaskCreate( vTaskCode, "TASK1", configMINIMAL_STACK_SIZE, ( void * ) 5, 1, NULL );
		xTaskCreate( vTaskCode, "TASK2", configMINIMAL_STACK_SIZE, ( void * ) 5, 1, NULL );
	#endif

	/* Test 2: 75% load */
	#if( LOAD_TEST == 2 )
		xTaskCreate( vTaskCode, "TASK1", configMINIMAL_STACK_SIZE, ( void * ) 3, 1, NULL );
		xTaskCreate( vTaskCode, "TASK2", configMINIMAL_STACK_SIZE, ( void * ) 3, 1, NULL );
		xTaskCreate( vTaskCode, "TASK3", configMINIMAL_STACK_SIZE, ( void * ) 3, 1, NULL );
	#endif

	/* Test 3: 50% load */
	#if( LOAD_TEST == 3 )
		xTaskCreate( vTaskCode, "TASK1", configMINIMAL_STACK_SIZE, ( void * ) 2, 1, NULL );
	#endif

	/* Test 4: 25% load */
	// Not possible - check v8 comment

	/* Test 5: 0% load */
	#if( LOAD_TEST == 5 )
		// no tasks
	#endif

	#if( LOAD_TEST != 0)
		/* Start the scheduler. */
		vTaskStartScheduler();
	#else
		for(;;)
		{
			vApplicationIdleHook();
		}
	#endif

	/* If all is well then this line will never be reached. If it is reached
	then it is likely that there was insufficient (FreeRTOS) heap memory space
	to create the idle task. This may have been trapped by the malloc() failed
	hook function, if one is configured. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
        /* Configuring GPIO Port. Check the Port Mapping. */
#ifdef __MSP430F247__
        WDTCTL = WDTPW + WDTHOLD; /* Kick off the dog. */
        P3OUT  = 0x00;      // all Port 2 output pins are in 0
        P3DIR  = 0xFF;      // 0b1111 - 0b1111
        P3SEL  = 0;         // all 8 pins are GPIO - General Purpose I/O
#endif

#ifdef __MSP430F5172__
        //halBoardInit();
        //LFXT_Start( XT1DRIVE_0 );
	hal430SetSystemClock( configCPU_CLOCK_HZ, configLFXT_CLOCK_HZ );

        // P2.2 and P2.3 are output
        P2OUT  = 0x00;      // all Port 2 output pins are in 0
        P2DIR  = 0x3D;      // 0b0011 - 0b1101
        P2SEL  = 0;         // all 8 pins are GPIO - General Purpose I/O

        /* Disable SVS ... Allows Fast Wakeup ... BOR is still active. */
        PMMCTL0_H = PMMPW_H;                    // PMM Password
        SVSMHCTL &= ~( SVMHE + SVSHE );         // Disable High side SVS
        SVSMLCTL &= ~( SVMLE + SVSLE );         // Disable Low side SVS
#endif

#ifdef CPU_MKL25Z128VLK4
        /* Init board hardware. */
        BOARD_InitPins();
        BOARD_BootClockRUN();

        const gpio_pin_config_t cfg = {1, 1};
        GPIO_PinInit(GPIOB, 2, &cfg);
        GPIO_PinInit(GPIOB, 3, &cfg);
#endif
}
/*-----------------------------------------------------------*/

#ifndef CPU_MKL25Z128VLK4
/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
    const unsigned short usACLK_Frequency_Hz = 32768;

    /* Ensure the timer is stopped. */
    TA0CTL = 0;

    /* Run the timer from the ACLK. */
    TA0CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA0CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

    /* Enable the interrupts. */
    TA0CCTL0 = CCIE;

    /* Start up clean. */
    TA0CTL |= TACLR;

    /* Up mode. */
    TA0CTL |= MC_1;
}
/*-----------------------------------------------------------*/
#endif


void vApplicationIdleHook( void )
{
    /*   Counting bits set, in parallel:
     *   http://graphics.stanford.edu/~seander/bithacks.html
     *
     *   Adapted for 16-bit counting.
     */
    unsigned portSHORT v = usContextSwitchBitmap; // count bits set in this (16-bit value)
    unsigned portSHORT c; // store the total here

    c = v - ( ( v >> 1 ) & 0x5555 );
    c = ( ( c >> 2 ) & 0x3333 ) + ( c & 0x3333 );
    c = ( ( c >> 4 ) + c ) & 0x0F0F;
    c = ( ( c >> 8 ) + c ) & 0x00FF;

    // Turns On LPM LED debug
    vPowerUpLed( pdTRUE, 2 );

#ifndef CPU_MKL25Z128VLK4
    #if( LPM == -1 )
        __no_operation();
    #endif

    #if( LPM == 0 )
      __bis_SR_register( LPM0_bits + GIE );
    #endif

    #if( LPM == 1 )
      __bis_SR_register( LPM1_bits + GIE );
    #endif

    #if( LPM == 2 )
      __bis_SR_register( LPM2_bits + GIE );
    #endif

    #if( LPM == 3 )
      __bis_SR_register( LPM3_bits + GIE );
    #endif

    #if( LPM == 4 )
      __bis_SR_register( LPM4_bits + GIE );
    #endif

    #if( LPM == 5 )
        switch( c )
        {
            // For a full-load system, not a good idea to go to any LPM mode
            case 0:
              break;

            // For a high-load system, wake-up time can be a problem for RT
            case 1:
              /* Fall through case. */
            case 2:
              /* Fall through case. */
            case 3:
              __bis_SR_register( LPM1_bits + GIE );
              break;

            // Otherwise, it is worth saving energy
            case 4:
              /* Fall through case. */
            case 5:
              /* Fall through case. */
            case 6:
              /* Fall through case. */
            case 7:
              /* Fall through case. */
            case 8:
              /* Fall through case. */
            case 9:
              /* Fall through case. */
            case 10:
              /* Fall through case. */
            case 11:
              /* Fall through case. */
            case 12:
              /* Fall through case. */
            case 13:
              /* Fall through case. */
            case 14:
              /* Fall through case. */
            case 15:
              /* Fall through case. */
            case 16:
              __bis_SR_register( LPM3_bits + GIE );
              break;

            default:
              /* Unexpected value. Do nothing! */
              break;
        }
    #endif
#else
	#if( LPM == -1 )
        __asm("NOP");
	#endif

	#if( LPM == 0 )
	  SMC_SetPowerModeWait(SMC);
	#endif

	#if( LPM == 1 )
	  SMC_SetPowerModeWait(SMC);
	#endif

	#if( LPM == 2 )
	  SMC_SetPowerModeVlpr(SMC);
	  SMC_SetPowerModeVlpw(SMC);
	  SMC_SetPowerModeRun(SMC);
	#endif

	#if( LPM == 3 )
	  SMC_SetPowerModeVlpr(SMC);
	  SMC_SetPowerModeVlpw(SMC);
	  SMC_SetPowerModeRun(SMC);
	#endif

	#if( LPM == 4 )
	  SMC_SetPowerModeStop(SMC, 0);
	#endif

	#if( LPM == 5 )
		switch( c )
		{
			// For a full-load system, not a good idea to go to any LPM mode
			case 0:
			  break;

			// For a high-load system, wake-up time can be a problem for RT
			case 1:
			  /* Fall through case. */
			case 2:
			  /* Fall through case. */
			case 3:
				SMC_SetPowerModeWait(SMC);
			  break;

			// Otherwise, it is worth saving energy
			case 4:
			  /* Fall through case. */
			case 5:
			  /* Fall through case. */
			case 6:
			  /* Fall through case. */
			case 7:
			  /* Fall through case. */
			case 8:
			  /* Fall through case. */
			case 9:
			  /* Fall through case. */
			case 10:
			  /* Fall through case. */
			case 11:
			  /* Fall through case. */
			case 12:
			  /* Fall through case. */
			case 13:
			  /* Fall through case. */
			case 14:
			  /* Fall through case. */
			case 15:
			  /* Fall through case. */
			case 16:
			  SMC_SetPowerModeVlpr(SMC);
			  SMC_SetPowerModeVlpw(SMC);
			  SMC_SetPowerModeRun(SMC);
			  break;

			default:
			  /* Unexpected value. Do nothing! */
			  break;
		}
	#endif

#endif
}
/*-----------------------------------------------------------*/

void vTaskCode( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = ( TickType_t ) pvParameters;

    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    int i = 0;

    for( ;; )
    {
        /* The function __delay_cycles() messes up with the system determinism.
         * Instead, use dummy operations, like incrementing. */
        for( i = 0; i < 3000; i++);
        if( xFrequency > 0 )
        {
            vTaskDelayUntil( &xLastWakeTime, xFrequency );
        }
    }
}
