/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 6 $
 * $Date: 14/09/11 5:31p $
 * @brief    Transmit and receive data from PC terminal through RS232 interface.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano100Series.h"

#include "uart.h"

#define RXBUFSIZE 1024
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8SendData[12] = {0};
uint8_t g_u8RecData[RXBUFSIZE]  = {0};
int32_t	inc=0;
uint8_t u8InChar=0xFF;

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;
volatile int32_t g_bWait         = TRUE;
volatile int32_t g_i32pointer = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void UART1_TEST_HANDLE(void);
void UART_FunctionTest(void);


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL |= (0x1 << CLK_PWRCTL_HXT_EN_Pos); // HXT Enabled

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_CLKSTATUS_HXT_STB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLK_S_Msk;
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_HXT;

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable
    CLK->APBCLK |= CLK_APBCLK_UART1_EN; // UART1 Clock Enable

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART_S_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART_S_Pos);// Clock source from external 12 MHz or 32 KHz crystal clock

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB0_MFP_Msk | SYS_PB_L_MFP_PB1_MFP_Msk);
    SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB0_MFP_UART0_RX | SYS_PB_L_MFP_PB1_MFP_UART0_TX);

    /* Set PB multi-function pins for UART1 RXD, TXD, RTS, CTS  */
    SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB4_MFP_Msk | SYS_PB_L_MFP_PB5_MFP_Msk );//|
                      // SYS_PB_L_MFP_PB6_MFP_Msk | SYS_PB_L_MFP_PB7_MFP_Msk);
    SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB4_MFP_UART1_RX | SYS_PB_L_MFP_PB5_MFP_UART1_TX); //|
                      //SYS_PB_L_MFP_PB6_MFP_UART1_RTS  | SYS_PB_L_MFP_PB7_MFP_UART1_CTS);

    /* Lock protected registers */
    SYS_LockReg();

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    UART_Open(UART0, 115200);
	  UART_ENABLE_INT(UART0, (UART_IER_RDA_IE_Msk ));
    NVIC_EnableIRQ(UART0_IRQn);
	//	UART_EnableInt(UART0, UART_IER_RDA_IE_Msk );
}

void UART1_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    UART_Open(UART1, 115200);

 

}


void delay(int32_t ms)
{
	int32_t us;
	for(;ms>0;ms--)
	{
		for(us=0;us<100000;us++){}
		
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Test Sample                                                                                        */
/* Test Item                                                                                               */
/* It sends the received data to HyperTerminal.                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for printf */
    UART0_Init();

    /* Init UART1 for test */
    UART1_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/


   // UART_FunctionTest();

    while(1)
		{
			printf("\r\nAT\r\n");
			
			delay(10);
		}
}














/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    UART1_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

void UART0_IRQHandler(void)
{    
//	uint8_t u8InChar=0xFF;

					while(!UART_GET_RX_EMPTY(UART0)) {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART0);           /* Rx trigger level is 1 byte*/
					}
}


void UART1_TEST_HANDLE()
{
    uint8_t u8InChar=0xFF;
    uint32_t u32IntSts= UART1->ISR;

    if(u32IntSts & UART_ISR_RDA_IS_Msk) {
        printf("\nInput:");

        /* Get all the input characters */
        while(!UART_GET_RX_EMPTY(UART1)) {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART1);           /* Rx trigger level is 1 byte*/

            printf("%c ", u8InChar);

            if(u8InChar == '0') {
                g_bWait = FALSE;
            }

            /* Check if buffer full */
            if(g_u32comRbytes < RXBUFSIZE) {
                /* Enqueue the character */
                g_u8RecData[g_u32comRtail] = u8InChar;
                g_u32comRtail = (g_u32comRtail == (RXBUFSIZE-1)) ? 0 : (g_u32comRtail+1);
                g_u32comRbytes++;
            }
        }
        printf("\nTransmission Test:");
    }

    if(u32IntSts & UART_ISR_THRE_IS_Msk) {
        uint16_t tmp;
        tmp = g_u32comRtail;
        if(g_u32comRhead != tmp) {
            u8InChar = g_u8RecData[g_u32comRhead];
            UART_WRITE(UART1,u8InChar);
            g_u32comRhead = (g_u32comRhead == (RXBUFSIZE-1)) ? 0 : (g_u32comRhead+1);
            g_u32comRbytes--;
        }
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/*  UART Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void UART_FunctionTest()
{
    printf("+-----------------------------------------------------------+\n");
    printf("|  UART Function Test                                       |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  Description :                                            |\n");
    printf("|    The sample code will print input char on terminal      |\n");
    printf("|    Please enter any to start     (Press '0' to exit)      |\n");
    printf("+-----------------------------------------------------------+\n");

    /*
        Using a RS232 cable to connect UART0 and PC.
        UART0 is set to debug port. UART1 is enable RDA and RLS interrupt.
        When inputting char to terminal screen, RDA interrupt will happen and
        UART0 will print the received char on screen.
    */

    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART1, (UART_IER_RDA_IE_Msk | UART_IER_THRE_IE_Msk | UART_IER_RTO_IE_Msk));
    NVIC_EnableIRQ(UART1_IRQn);
    while(g_bWait);

    /* Disable Interrupt */
    UART_DISABLE_INT(UART1, (UART_IER_RDA_IE_Msk | UART_IER_THRE_IE_Msk | UART_IER_RTO_IE_Msk));
    NVIC_DisableIRQ(UART1_IRQn);
    g_bWait =TRUE;
    printf("\nUART Sample Demo End.\n");

}


