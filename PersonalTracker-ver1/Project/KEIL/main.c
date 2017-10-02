/******************************************************************************
 * @file		 main.c
 * @version	V1.00
 * $Revision: 6 $
 * $Date: 14/09/11 5:31p $
 * @brief		Personal Tracking system microcontroller with GSM/GPS combo module.
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "Nano100Series.h"
#include "stdlib.h"
#include "math.h"
/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic										 // define objects in main module
#include "osObjects.h"											// RTOS object definitions
#include "uart.h"
#include "cmsis_os.h"							 // CMSIS RTOS header file


osMutexDef (uart_mutex);		// Declare mutex
osMutexId	(uart_mutex_id); // Mutex ID
		
osMutexDef (tcp_mutex);		// Declare mutex
osMutexId	(tcp_mutex_id); // Mutex ID

osMutexDef (fs_mutex);		// Declare mutex
osMutexId	(fs_mutex_id); // Mutex ID		

extern osThreadId tid_Thread1;																					// thread id
extern int Init_Thread (void);									 // thread object
extern void Init_Timers (void);
extern void SendChar(int ch);
extern char fileinstance[20];

void clear(void);
extern	int32_t life;
//int32_t parseok=0;
void SendAT(char * command, char * response1, char * response2, char * response3, int32_t timeout);
__inline void Save_FS(void);
__inline void manualdelay(int delayms);
__inline void Send_FS(void);
__inline int count_char(char ch,char* string);
__inline void parse_g(char* str, int first, int sec, char f, char s , char *string);
//__inline void batteryind (void);
__inline int8_t checkallnumsinstring(char* checkstring);
extern void fileclose(void);
extern void SendChar_To_UART1(int ch);
extern void send_string_to_uart1(char* string);

__inline void fileopen(void);


extern void remove_all_chars(char* str, char c, char d);
void SendAT_FS(char * command, char * response1, char * response2, char * response3, int32_t timeout);
void TCP_Send(char * tcpcommand,char * tcpdata, char * tcpresponse1, char * tcpresponse2, char * tcpresponse3, int32_t tcptimeout);
void SendAT_GPS(char * command, char * response1, char * response2, char * response3, int32_t timeout);
void Save_FS(void);
void SendAT_GPS_WO_MUTEX(char * command, char * response1, char * response2, char * response3, int32_t timeout);
void I2C_Write(uint16_t u16Address, uint8_t u8Data);
uint8_t I2C_Read(uint16_t u16Address);
void I2C_MasterTx(uint32_t u32Status);
void I2C_MasterRx(uint32_t u32Status);
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables																																												*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr = 0x1D;
uint32_t u32Status;

uint8_t g_au8TxData[1];
uint8_t g_u8RxData;
uint8_t g_u8DataLen = 0;
volatile uint8_t g_u8EndFlag = 0;
typedef void (*I2C_FUNC)(uint32_t u32Status);
static I2C_FUNC s_I2CHandlerFn = NULL;
char g_u8SendData[TXBUFSIZE] = {0};
char g_u8RecData[RXBUFSIZE]	= {0};
int32_t	inc=0;
uint8_t u8InChar=0xFF;
int32_t g_u8RecDataptr=0;
int32_t timer0ticks=0;
int32_t tmr0sec=0;
float	u32ADC0Result;
float u32ADC0Result1;
int8_t charging, cpinready, cregready;
int seeker = 0;
int breaker=0;
/* Global variables */
__IO int32_t	 _Wakeup_Flag = 0;		/* 1 indicates system wake up from power down mode */
__IO uint32_t	_Pin_Setting[11];		/* store Px_H_MFP and Px_L_MFP */
__IO uint32_t	_PullUp_Setting[6];	/* store GPIOx_PUEN */

char * r1;
char * r2;
char * r3;
volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead	= 0;
volatile uint32_t g_u32comRtail	= 0;
volatile int32_t g_bWait				 = TRUE;
volatile uint8_t u8ADF;
uint8_t mainla = 0;
uint8_t th1la = 0;
uint8_t th2la = 0;
  uint16_t OUT_P ,OUT_N;
int motion = 0;
volatile int32_t g_i32pointer = 0;
char temp[100];
int network=0;
char imei[25];
int imeiptr0=0;
int imeiptr=0;
char dmsg[50];
/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype																																							*/
/*---------------------------------------------------------------------------------------------------------*/

int32_t main(void);
void ADC_IRQHandler(void);
void UART1_TEST_HANDLE(void);
void UART_FunctionTest(void);

void SavePinSetting(void);
void RestorePinSetting(void);
void Enter_PowerDown(void);
void Leave_PowerDown(void);
void ADC0_Init(void);
void Send_FS(void);

void Thread1 (void const *argument);

void WDT_IRQHandler(void){
    // Reload WWDT counter and clear WWDT interrupt flag
    WWDT_RELOAD_COUNTER();
    WWDT_CLEAR_INT_FLAG();
    send_string_to_uart1("WWDT counter reload\n");
//    while(1);
}


__inline int count_char(char ch,char* string){
int32_t	count=0;
int32_t ass=0;
	for(ass=0;ass<strlen(string);ass++){
		if(string[ass] == ch)
		{count++;}
	}
return count;
}

__inline void manualdelay(int delayms){
//	int delayus=0;
	int d1,d2;
	for(d1 = 0; d1 < delayms ; d1++){
		for(d2=0; d2 < 65535; d2++){
		}
	}
}

//I2C_Write(uint16_t u16Address, uint8_t u8Data)
__inline int16_t motion_sense(){
  static int motion_counts,immotion_counts;
  uint8_t OUT_Y_H_A,  OUT_X_H_A,  OUT_Z_H_A;
  I2C_Write(0x20, 0x67);  
 // I2C_Write(0x20, 0);  
//  OUT_X_L_A = I2C_Read(0x28);
  OUT_X_H_A = I2C_Read(0x29);
  OUT_Y_H_A = I2C_Read(0x2B);
  OUT_Z_H_A = I2C_Read(0x2D);
  OUT_N = sqrt((OUT_X_H_A*OUT_X_H_A) + (OUT_Y_H_A*OUT_Y_H_A) + (OUT_Z_H_A*OUT_Z_H_A));
  if( ((OUT_N - OUT_P) >= 2) || ((OUT_N - OUT_P) <= -2) ){
    motion_counts++;
  }
  else{
    immotion_counts++;
  }
  OUT_P = OUT_N;
  if(motion_counts >= 10){
    motion_counts=0;
    immotion_counts=0;
    motion=1;
  }
  if(immotion_counts >= 30){
    motion=0;
    immotion_counts=0;
    motion_counts=0;

  }
//  memset(dmsg,0,50);
//  sprintf(dmsg,"X:0x%x,Y:0x%x,Z:0x%x,Mot_cnts=%d,Immot_cnts=%d,Mot=%d\r\n", OUT_X_H_A, OUT_Y_H_A, OUT_Z_H_A, motion_counts, immotion_counts, motion);     
//  send_string_to_uart1(dmsg);
return 0;
}

__inline void SYS_Init(void){
		/*---------------------------------------------------------------------------------------------------------*/
		/* Init System Clock																																											 */
		/*---------------------------------------------------------------------------------------------------------*/
		/* Unlock protected registers */
		SYS_UnlockReg();
//		CLK->PWRCTL |= (0x1 << CLK_PWRCTL_HIRC_EN_Pos); // HIRC Enabled
//		CLK_WaitClockReady( CLK_CLKSTATUS_HIRC_STB_Msk);
		/* Switch HCLK clock source to XTAL */  

		/* Enable External XTAL (4~24 MHz) */
		CLK->PWRCTL |= (0x1 << CLK_PWRCTL_HXT_EN_Pos); // HXT Enabled
  
		/* Waiting for 12MHz clock ready */
		CLK_WaitClockReady( CLK_CLKSTATUS_HXT_STB_Msk);
		/* Switch HCLK clock source to XTAL */  
		CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLK_S_Msk;
		CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_HXT;
//		/* Enable IP clock */
		CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable
		CLK->APBCLK |= CLK_APBCLK_UART1_EN; // UART1 Clock Enable
		CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(WDT_MODULE);

    CLK_EnableModuleClock(I2C1_MODULE);
    
		/* Select IP clock source */
//		CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART_S_Msk;
//		CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART_S_Pos);// Clock source from external 12 MHz or 32 KHz crystal clock
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_UART_CLK_DIVIDER(1));
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_UART_CLK_DIVIDER(1));
//    CLK_SetModuleClock(I2C1_MODULE, 0, 0);


    CLK_SetModuleClock(WDT_MODULE, 0, 0);

		/* Update System Core Clock */
		/* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
		SystemCoreClockUpdate();

		/*---------------------------------------------------------------------------------------------------------*/
		/* Init I/O Multi-function																																								 */
		/*---------------------------------------------------------------------------------------------------------*/
		/* Set PB multi-function pins for UART0 RXD and TXD	*/
		SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB0_MFP_Msk | SYS_PB_L_MFP_PB1_MFP_Msk);
		SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB0_MFP_UART0_RX | SYS_PB_L_MFP_PB1_MFP_UART0_TX);
//		/* Set PB multi-function pins for UART1 RXD, TXD, RTS, CTS	*/
		SYS->PB_L_MFP &= ~(SYS_PB_L_MFP_PB4_MFP_Msk | SYS_PB_L_MFP_PB5_MFP_Msk );//|
//											// SYS_PB_L_MFP_PB6_MFP_Msk | SYS_PB_L_MFP_PB7_MFP_Msk);
		SYS->PB_L_MFP |= (SYS_PB_L_MFP_PB4_MFP_UART1_RX | SYS_PB_L_MFP_PB5_MFP_UART1_TX); //|
//											//SYS_PB_L_MFP_PB6_MFP_UART1_RTS	| SYS_PB_L_MFP_PB7_MFP_UART1_CTS);
											
    SYS->PA_H_MFP |= (SYS_PA_H_MFP_PA11_MFP_I2C1_SCL | SYS_PA_H_MFP_PA10_MFP_I2C1_SDA);
    CLK->APBCLK |= CLK_APBCLK_I2C1_EN;

    /* Reset I2C1 */
    SYS->IPRST_CTL2 |=  SYS_IPRST_CTL2_I2C1_RST_Msk;
    SYS->IPRST_CTL2 &= ~SYS_IPRST_CTL2_I2C1_RST_Msk;											
		/* Set PA multi-function pins for ADC */
		SYS->PA_L_MFP &= ~(SYS_PA_L_MFP_PA0_MFP_Msk);
		SYS->PA_L_MFP |= SYS_PA_L_MFP_PA0_MFP_ADC_CH0;

		/* Disable PA.0 digital input path */
		PA->OFFD |= (((1<<0)) << GP_OFFD_OFFD_Pos);											


		/* Lock protected registers */
		SYS_LockReg();

}

__inline void UART0_Init(){
		/*---------------------------------------------------------------------------------------------------------*/
		/* Init UART																																															 */
		/*---------------------------------------------------------------------------------------------------------*/
		UART_Open(UART0, 115200);
		UART_ENABLE_INT(UART0, (UART_IER_RDA_IE_Msk ));
		NVIC_EnableIRQ(UART0_IRQn);
}
__inline void UART1_Init(){
		/*---------------------------------------------------------------------------------------------------------*/
		/* Init UART																																															 */
		/*---------------------------------------------------------------------------------------------------------*/
		UART_Open(UART1, 115200);
//		UART_ENABLE_INT(UART1, (UART_IER_RDA_IE_Msk ));
//		NVIC_EnableIRQ(UART1_IRQn);
}

__inline void ADC0_Init(){
		// Enable channel 0,1,2
		ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_CONTINUOUS, (ADC_CH_0_MASK));
		ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_VREF);
		ADC_POWER_ON(ADC);
		ADC_START_CONV(ADC);
}

__inline void delay(int32_t ms){
	int32_t us;
	for(;ms>0;ms--){
		for(us=0;us<100000;us++){}
	}
}

int main(void){
int freq;int32_t u32Data;
    SYS_Init();
		GPIO_SetMode(PC, BIT1, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PC, BIT0, GPIO_PMD_OUTPUT);
    PC0=1;
		PC1=1;
		GPIO_SetMode(PA, BIT3, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PA, BIT4, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PA, BIT5, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PA, BIT6, GPIO_PMD_OUTPUT);
		GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
		UART0_Init();
		UART1_Init();
    memset(dmsg,0,50);

    I2C_Open(I2C1, 100000);  
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn); 
    freq = I2C_GetBusClockFreq(I2C1);
    sprintf(dmsg,"\r\nI2C1 clock :%d \r\n", freq);
//    send_string_to_uart1(dmsg);
//    while(1){
//    motion_sense();
////    memset(dmsg,0,50);
////    sprintf(dmsg,"\r\nI2C1 X:%d\r\n", motion_sense());     
////    send_string_to_uart1(dmsg);
//    manualdelay(2);
//  
//    }
    Init_Timers();
		osKernelInitialize ();										// initialize CMSIS-RTOS
		ADC0_Init();
    // WDT register is locked, so it is necessary to unlock protect register before configure WDT
    SYS_UnlockReg();
    // WDT timeout every 2^14 WDT clock, enable system reset, disable wake up system
    WWDT_Open(WWDT_PRESCALER_768, 0x20, TRUE);
    // Enable WDT timeout interrupt
    WDT_EnableInt();
    NVIC_EnableIRQ(WDT_IRQn);
		uart_mutex_id = osMutexCreate(osMutex(uart_mutex));
		tcp_mutex_id = osMutexCreate(osMutex(tcp_mutex));
		fs_mutex_id = osMutexCreate(osMutex(fs_mutex));
		PA3=0;  
		PA4=0;
		PA5=0;
		PA6=0;
    Init_Thread();
		osKernelStart ();												 // start thread execution 
    PB2=0;
		SendAT("\r\nAT+QSCLK=1\r\n", "Ready", "OK" , "ERROR",5);
    SendAT("\r\nAT+SYSTEMSTARTS\r\n\r\n", "Ready", "OK" , "ERROR",10);	
    fileopen();
    SendAT("\r\nAT+QGNSSC=1\r\n", "Ready", "OK" , "ERROR",10);	    
    while(1){
      mainla = 1;
      th1la = 0;
      th2la = 0; 
 //     motion_sense();
 //     osDelay(100);                                           // suspend thread
      
		}
}
/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event																														*/
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
	//	UART1_TEST_HANDLE();
}
/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function																																									*/
/*---------------------------------------------------------------------------------------------------------*/

void UART0_IRQHandler(void)
{		
					while(!UART_GET_RX_EMPTY(UART0)) 
					{
						g_u8RecData[g_u8RecDataptr] = UART_READ(UART0);
						g_u8RecDataptr++;
					}
   //       while(1)

}
__inline void parse_g(char* str, int first, int sec, char f, char s , char *string)
{int sz1,sz2,i11,temp11,j11,l;

		sz1=	strlen(str);
		sz2=	strlen(string);
		memset(string,0,sz2);
		for(i11=0,temp11=0;i11<sz1;i11++)
		{
		if(str[i11]==s)
			{temp11++;j11=i11;}
		else{}
			
		if(temp11>=sec){i11=9900;}
		else{}	
		
		}	
		
		for(i11=0,temp11=0;i11<sz1;i11++)
		{
		if(str[i11]==f)
			{temp11++;l=i11;}										
		else{}
		if(temp11>=first){i11=9900;}else{}
		}	
		for(i11=0;i11<(j11-l-1);i11++)
		{
		string[i11] = str[l+i11+1];
		}
	
}
void SavePinSetting()
{
		/* Save Pin selection setting */
		_Pin_Setting[0] = SYS->PA_L_MFP;
		_Pin_Setting[1] = SYS->PA_H_MFP;
		_Pin_Setting[2] = SYS->PB_L_MFP;
		_Pin_Setting[3] = SYS->PB_H_MFP;
		_Pin_Setting[4] = SYS->PC_L_MFP;
		_Pin_Setting[5] = SYS->PC_H_MFP;
		_Pin_Setting[6] = SYS->PD_L_MFP;
		_Pin_Setting[7] = SYS->PD_H_MFP;
		_Pin_Setting[8] = SYS->PE_L_MFP;
		_Pin_Setting[9] = SYS->PE_H_MFP;
		_Pin_Setting[10] = SYS->PF_L_MFP;

		/* Save Pull-up setting */
		_PullUp_Setting[0] =	PA->PUEN;
		_PullUp_Setting[1] =	PB->PUEN;
		_PullUp_Setting[2] =	PC->PUEN;
		_PullUp_Setting[3] =	PD->PUEN;
		_PullUp_Setting[4] =	PE->PUEN;
		_PullUp_Setting[5] =	PF->PUEN;
}
/**
	* @brief	Restore original setting of multi-function pin selection.
	* @param	None.
	* @return None.
	*/
void RestorePinSetting()
{
		/* Restore Pin selection setting */
		SYS->PA_L_MFP = _Pin_Setting[0];
		SYS->PA_H_MFP = _Pin_Setting[1];
		SYS->PB_L_MFP = _Pin_Setting[2];
		SYS->PB_H_MFP = _Pin_Setting[3];
		SYS->PC_L_MFP = _Pin_Setting[4];
		SYS->PC_H_MFP = _Pin_Setting[5];
		SYS->PD_L_MFP = _Pin_Setting[6];
		SYS->PD_H_MFP = _Pin_Setting[7];
		SYS->PE_L_MFP = _Pin_Setting[8];
		SYS->PE_H_MFP = _Pin_Setting[9];
		SYS->PF_L_MFP = _Pin_Setting[10];

		/* Restore Pull-up setting */
		PA->PUEN = _PullUp_Setting[0];
		PB->PUEN = _PullUp_Setting[1];
		PC->PUEN = _PullUp_Setting[2];
		PD->PUEN = _PullUp_Setting[3];
		PE->PUEN = _PullUp_Setting[4];
		PF->PUEN = _PullUp_Setting[5];
}
/**
	* @brief	Save multi-function pin setting and then go to power down.
	* @param	None.
	* @return None.
	*/
void Enter_PowerDown()
{
		/* Back up original setting */
		SavePinSetting();

		/* Set function pin to GPIO mode */
		SYS->PA_L_MFP = 0;
		SYS->PA_H_MFP = 0;
		SYS->PB_L_MFP = 0;
		SYS->PB_H_MFP = 0;
		SYS->PC_L_MFP = 0;
		SYS->PC_H_MFP = 0;
		SYS->PD_L_MFP = 0;
		SYS->PD_H_MFP = 0;
		SYS->PE_L_MFP = 0;
		SYS->PE_H_MFP = 0;
		SYS->PF_L_MFP = 0x00007700;

		/* Enable GPIO pull up */
		PA->PUEN = 0xFFFF;
		PB->PUEN = 0xFFFF;
		PC->PUEN = 0xFFFF;
		PD->PUEN = 0xFFFF;
		PE->PUEN = 0xFFFF;
		PF->PUEN = 0x0033;			/* exclude GPF2 and GPF3 which are HXT OUT/IN */

		/* Disable LCD clock */
		CLK->APBCLK &= ~CLK_APBCLK_LCD_EN; /* Disable LCD clock */
		CLK->PWRCTL &= ~CLK_PWRCTL_LXT_EN_Msk; /* disable LXT - 32Khz */

		CLK_PowerDown();

}
/**
	* @brief	This function will not be called.
	* @param	None.
	* @return None.
	*/
void Leave_PowerDown()
{

		/* Restore pin setting */
		RestorePinSetting();

		/* Set PF.0 and PF.1 to ICE Data and Clock */
		SYS->PF_L_MFP |= 0x00000077;

		/* Enable LCD clock */
		CLK->PWRCTL |= CLK_PWRCTL_LXT_EN_Msk; /* enable LXT - 32Khz */
		CLK->APBCLK |= CLK_APBCLK_LCD_EN; /* enable LCD clock */

}
void clear()
{
					if(!UART_GET_RX_EMPTY(UART0)) 
					{
						g_u8RecData[g_u8RecDataptr] = UART_READ(UART0);
						g_u8RecDataptr++;
					}
	
}
// end
void SendAT1(char * command, char * response1, char * response2, char * response3, int32_t timeout)
{
	PB2=0;
 	osDelay(500);

//	osMutexWait(uart_mutex_id, osWaitForever);
	tmr0sec=0;
//	timeout =5;

	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	printf("%c",0x1A);
	clear();
	printf(command);
	do{
		r1 = strstr(g_u8RecData, response1);
		r2 = strstr(g_u8RecData, response2);
		r3 = strstr(g_u8RecData, response3);
			
	}while(!(r1 || r2 || r3 ||((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
//	PB2=1;
//	osMutexRelease(uart_mutex_id);
  send_string_to_uart1(g_u8RecData);  
  
osDelay(5);
}
 void cpinquerry()
{
  int timeout;
  osMutexWait(uart_mutex_id, osWaitForever);
	tmr0sec=0;
	timeout =5;
  PB2=0;
	osDelay(100);
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	printf("%c",0x1A);
	clear();
	printf("\r\nAT+CPIN?\r\n");
	do{
		r1 = strstr(g_u8RecData, "CPIN: READY");
		r2 = strstr(g_u8RecData, "OK");
		r3 = strstr(g_u8RecData, "ERROR");
			
	}while(!(r1 || r2 || r3 ||((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
	PB2=1;
  
  if(r1)
  {
    cpinready=1;
  }
  else
  {
    cpinready=0;
  }

osMutexRelease(uart_mutex_id);
osDelay(5);  
}

 void cregquerry()
{
  int timeout;
  char cregresp[10];
  osMutexWait(uart_mutex_id, osWaitForever);
	tmr0sec=0;
	timeout =5;
    PB2=0;
	osDelay(100);
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	printf("%c",0x1A);
	clear();
	printf("\r\nAT+CREG?\r\n");
	do{
		r1 = strstr(g_u8RecData, "CPIN: READY");
		r2 = strstr(g_u8RecData, "OK");
		r3 = strstr(g_u8RecData, "ERROR");
			
	}while(!(r1 || r2 || r3 ||((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
	PB2=1;
  memset(cregresp,0,10);
  parse_g(g_u8RecData, 1, 2, ',', '\n' , cregresp);
  if(strstr(cregresp,"1") || strstr(cregresp, "5"))
  {
    cregready=1;
  }
  else
  {
    cregready=0;
  }
  
  
	osMutexRelease(uart_mutex_id);
osDelay(5);
  
  
}




void SendAT(char * command, char * response1, char * response2, char * response3, int32_t timeout)
{
	osMutexWait(uart_mutex_id, osWaitForever);
	tmr0sec=0;
//	timeout =5;
  PB2=0;
	osDelay(10);
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	printf("%c",0x1A);
	clear();
	printf(command);
	do{
		r1 = strstr(g_u8RecData, response1);
		r2 = strstr(g_u8RecData, response2);
		r3 = strstr(g_u8RecData, response3);
	}while(!(r1 || r2 || r3 ||((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||

   if(!strstr(command, "QILOCIP"))
   {
     if(!(r1 || r2 || r3))
      {
//        printf("\r\n\r\nAT+CFUN=1,1\r\n\r\n");
//        manualdelay(100);
      }
   }
//  memset(dmsg,0,50);
//  sprintf(dmsg,"X:0x%x,Y:0x%x,Z:0x%x,Mot_cnts=%d,Immot_cnts=%d,Mot=%d\r\n", OUT_X_H_A, OUT_Y_H_A, OUT_Z_H_A, motion_counts, immotion_counts, motion);     
  PB2=1;
  osMutexRelease(uart_mutex_id);
//osDelay(2);
}

void SendAT_FS(char * command, char * response1, char * response2, char * response3, int32_t timeout)
{
	osMutexWait(uart_mutex_id, osWaitForever);
	tmr0sec=0;
//	timeout =5;
  PB2=0;
	osDelay(10);
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
//	printf("%c",0x1A);
	clear();
	printf(command);
	do{
		r1 = strstr(g_u8RecData, response1);
		r2 = strstr(g_u8RecData, response2);
		r3 = strstr(g_u8RecData, response3);
	}while(!(r1 || r2 || r3 ||((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||

   if(!strstr(command, "QILOCIP"))
   {
     if(!(r1 || r2 || r3))
      {
//        printf("\r\n\r\nAT+CFUN=1,1\r\n\r\n");
//        manualdelay(100);
      }
   }
  PB2=1;
  osMutexRelease(uart_mutex_id);
osDelay(2);
}

void TCP_Send_ch(char * tcpcommand,char * tcpdataq, char * tcpresponse1, char * tcpresponse2, char * tcpresponse3, int32_t tcptimeout)
{
  int tcpdatalength=0;
	int tcpdataptr=0;
	int times = 0;
	int timesptr=0;
	int tdp=0;
	int tdpend=600;
  char chdatalength[5];
  char tcpdata[600];
  int  cc =0;
  breaker = 0;
	osMutexWait(uart_mutex_id, osWaitForever);
	osMutexWait(tcp_mutex_id, osWaitForever);
	PB2=0;
 //	osDelay(500);
	printf("%c",0x1A);
	tmr0sec=0;
	tcptimeout =5;
	r1=0;
	r2=0;
	r3=0;
  SendAT("\r\nAT+QFLDs=\"UFS\"\r\n", "Ready", "OK" , "ERROR",50);
  
  memset(chdatalength,0,5);
  SendAT("\r\nAT+QFLST=\"LOG.TXT\"\r\n", "Ready", "OK" , "ERROR",50);
  parse_g(g_u8RecData, 1, 2, ',', '\n' , chdatalength);
	remove_all_chars(chdatalength, '\r', 0x1A);		
  tcpdatalength =  atoi(chdatalength);

  fileopen();

  if(tcpdatalength%600 == 0)
  {
    times = (tcpdatalength/600);
  }
  else
  {
    times = (tcpdatalength/600)+1;
  }  

  for(timesptr=0; timesptr<times ;timesptr++)
  {
	g_u8RecDataptr=0;

		if(tcpdatalength>15)
		{
      clear();
      SendAT_GPS_WO_MUTEX("\r\nAT+QGNSSRD=\"NMEA/RMC\"\r\n", "+MGPSSTATUS", "OK" , "ERROR",10);	
      memset(temp,0,100);
      sprintf(temp,"\r\nAT+QFSEEK=%s,%d\r\n",fileinstance,seeker);
      SendAT(temp, "+CME ERROR", "OK" , "ERROR",10);
      if(seeker > tcpdatalength)
      {
        seeker = 0;
        SendAT("\r\nAT+QFDEL=\"LOG.TXT\"\r\n", "Ready", "OK" , "ERROR",10);
        breaker = 1;
        break;
      }
      if(strstr(g_u8RecData, "+CME ERROR")){breaker = 1;break;}
      memset(temp,0,100);
      sprintf(temp,"\r\nAT+QFREAD=%s,600\r\n",fileinstance);
      SendAT(temp, "+CME ERROR", "OK" , "ERROR",10);
      if(strstr(g_u8RecData, "+CME ERROR")){breaker = 1;break;}
      memset(tcpdata,0,600);
      remove_all_chars(g_u8RecData, '\r', 0x1A);	
      cc=count_char('\n',g_u8RecData)-1;      
      parse_g(g_u8RecData, 2,cc, '\n', '\n' , tcpdata);  
      memset(temp,0,100);
			g_u8RecDataptr=0;
			tmr0sec=0;
			memset(g_u8RecData,0,RXBUFSIZE);
			printf(tcpcommand);
			do{
				r1 = strstr(g_u8RecData, tcpresponse1);
				r2 = strstr(g_u8RecData, tcpresponse2);
				r3 = strstr(g_u8RecData, tcpresponse3);
					
			}while(!(r1 || r2 || r3 ||((tmr0sec >= tcptimeout))));	 //!(r1 || r2 || r3 ||

				tmr0sec=0;
				if(r1)
					{
						tmr0sec=0;
						tcptimeout =5;
						r1=0;
						r2=0;
						r3=0;
						g_u8RecDataptr=0;
						memset(g_u8RecData,0,RXBUFSIZE);
						clear();
//						printf(tcpdata);

 						tcpdataptr=0;
            tdpend=strlen(tcpdata);
           
						for(tdp=0;tdp<tdpend;tdp++)
						{
						SendChar(tcpdata[tdp]);
						tcpdataptr++;
            }
						printf("%c",0x1A);
						do{
							r1 = strstr(g_u8RecData, tcpresponse1);
							r2 = strstr(g_u8RecData, tcpresponse2);
							r3 = strstr(g_u8RecData, tcpresponse3);
						}while(!(r1 || r2 || r3 ||((tmr0sec >= tcptimeout))));
          }

			if(!(r3))
			{
				network=1;
        breaker = 1;        
				break;

			}
			else
			{
				network=0;
			}

    }
   
	if(network == 0 && timesptr>=times)
	{
//		memset(tcpdata,0,TXBUFSIZE);
	}
  seeker+=600;
		
	}
  
  fileclose();
  
  
  if(network == 0 && (timesptr>= times))
  {
  seeker = 0;
  SendAT("\r\nAT+QFDEL=\"LOG.TXT\"\r\n", "Ready", "OK" , "ERROR",10);    

  }
	PB2=1;
	r1=0;
	r2=0;
	r3=0;
	osMutexRelease(tcp_mutex_id);
	osMutexRelease(uart_mutex_id);

//osDelay(5);

}

void TCP_Send(char * tcpcommand,char * tcpdata, char * tcpresponse1, char * tcpresponse2, char * tcpresponse3, int32_t tcptimeout)
{
	int tcpdatalength=0;
	int tcpdataptr=0;
	int times = 0;
	int timesptr=0;
	int tdp=0;
	int tdpend=600;
	osMutexWait(uart_mutex_id, osWaitForever);
	osMutexWait(tcp_mutex_id, osWaitForever);
	PB2=0;
 //	osDelay(500);

	tmr0sec=0;
	tcptimeout =5;
	r1=0;
	r2=0;
	r3=0;

 
//	memset(g_u8RecData,0,RXBUFSIZE);
	g_u8RecDataptr=0;
	printf("%c",0x1A);
	tcpdatalength = strlen(tcpdata);
	if(tcpdatalength%600 == 0)
	{
		times = (tcpdatalength/600);
	}
	else
		{
			times = (tcpdatalength/600)+1;
		}
		if(tcpdatalength>15)
		{
			clear();
			for(timesptr = 0; timesptr < times ; timesptr++)
			{
			g_u8RecDataptr=0;
			tmr0sec=0;
			memset(g_u8RecData,0,RXBUFSIZE);
			printf(tcpcommand);
			do{
				r1 = strstr(g_u8RecData, tcpresponse1);
				r2 = strstr(g_u8RecData, tcpresponse2);
				r3 = strstr(g_u8RecData, tcpresponse3);
					
			}while(!(r1 || r2 || r3 ||((tmr0sec >= tcptimeout))));	 //!(r1 || r2 || r3 ||

				tmr0sec=0;
				if(r1)
					{
						tmr0sec=0;
						tcptimeout =5;
						r1=0;
						r2=0;
						r3=0;
						g_u8RecDataptr=0;
						memset(g_u8RecData,0,RXBUFSIZE);
						clear();
//						printf(tcpdata);
						if(timesptr == (times-1))
						{
							tdpend = tcpdatalength%600;
						}
						for(tdp=0;tdp<tdpend;tdp++)
						{
						SendChar(tcpdata[tcpdataptr]);
						tcpdataptr++;}
						printf("%c",0x1A);
						do{
							r1 = strstr(g_u8RecData, tcpresponse1);
							r2 = strstr(g_u8RecData, tcpresponse2);
							r3 = strstr(g_u8RecData, tcpresponse3);
						}while(!(r1 || r2 || r3 ||((tmr0sec >= tcptimeout))));
          }

			if(!(r3))
			{
				network=1;
				break;
			}
			else
			{
				network=0;
			}
		}

   
	if(network == 0 && timesptr>=times)
	{
		memset(g_u8SendData,0,TXBUFSIZE);
    Send_FS();
 	}
		
	}
	PB2=1;

	osMutexRelease(tcp_mutex_id);
	osMutexRelease(uart_mutex_id);

//osDelay(5);

}



void SendAT_GPS(char * command, char * response1, char * response2, char * response3, int32_t timeout)
{

	osMutexWait(uart_mutex_id, osWaitForever);
	PB2=0;
	osDelay(500);
	tmr0sec=0;
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	printf("%c",0x1A);
	clear();
	if(checkallnumsinstring(imei) ||  (strlen(imei) < 5))
	{
		printf("\r\nAT+GSN\r\n");
		do{
			r1 = strstr(g_u8RecData, response1);
			r2 = strstr(g_u8RecData, response2);
			r3 = strstr(g_u8RecData, response3);
		}while(!(r1 || r2 || r3 || ((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
		memset(imei,0,25);
		imeiptr0=0;
		for(imeiptr=0;imeiptr<strlen(g_u8RecData);imeiptr++)
		{
			if((g_u8RecData[imeiptr]>47)	&& (g_u8RecData[imeiptr]<58))
			{
				imei[imeiptr0] = g_u8RecData[imeiptr];
				imeiptr0++;
      }
		}
	remove_all_chars(imei, '\r', 0x1a);		
	}
	tmr0sec=0;
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	clear();
	printf("%c",0x1A);
	printf(command);
	do{
		r1 = strstr(g_u8RecData, response1);
		r2 = strstr(g_u8RecData, response2);
		r3 = strstr(g_u8RecData, response3);
	}while(!(r1 || r2 || r3 || ((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
	if(strstr(g_u8RecData,"GNRMC"))
	{
    u32ADC0Result = 3;
    ADC_START_CONV(ADC);
    u32ADC0Result = ADC_GET_CONVERSION_DATA(ADC, 0);
    u32ADC0Result = (3.943/2.097)*((u32ADC0Result*3.312) /4096);
    
		memset(temp,0,100);
		parse_g(g_u8RecData, 2, 10, 'C', ',' , temp);
		strcat(g_u8SendData,imei);
		strcat(g_u8SendData,",");
		strcat(g_u8SendData,temp);
		memset(temp,0,100);
		sprintf(temp,",F=%.1f\n",u32ADC0Result);
		strcat(g_u8SendData,temp);
		
  }
  if((strlen(g_u8SendData) > 2900))
  {
    memset(g_u8SendData,0,TXBUFSIZE);
    strcat(g_u8SendData,imei);
    strcat(g_u8SendData,"error:RAMfull\n");
  }
  
	PB2=1;

	osMutexRelease(uart_mutex_id);
	osDelay(1);
}



void SendAT_GPS_WO_MUTEX(char * command, char * response1, char * response2, char * response3, int32_t timeout)
{

//	osMutexWait(uart_mutex_id, osWaitForever);
	PB2=0;
	osDelay(500);
	tmr0sec=0;
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	printf("%c",0x1A);
	clear();
	if(checkallnumsinstring(imei) ||  (strlen(imei) < 5))
	{
		printf("\r\nAT+GSN\r\n");
		do{
			r1 = strstr(g_u8RecData, response1);
			r2 = strstr(g_u8RecData, response2);
			r3 = strstr(g_u8RecData, response3);
		}while(!(r1 || r2 || r3 || ((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
		memset(imei,0,25);
		imeiptr0=0;
		for(imeiptr=0;imeiptr<strlen(g_u8RecData);imeiptr++)
		{
			if((g_u8RecData[imeiptr]>47)	&& (g_u8RecData[imeiptr]<58))
			{
				imei[imeiptr0] = g_u8RecData[imeiptr];
				imeiptr0++;
      }
		}
	remove_all_chars(imei, '\r', 0x1a);		
	}
	tmr0sec=0;
	r1=0;
	r2=0;
	r3=0;
	g_u8RecDataptr=0;
	memset(g_u8RecData,0,RXBUFSIZE);
	clear();
	printf("%c",0x1A);
	printf(command);
	do{
		r1 = strstr(g_u8RecData, response1);
		r2 = strstr(g_u8RecData, response2);
		r3 = strstr(g_u8RecData, response3);
	}while(!(r1 || r2 || r3 || ((tmr0sec >= timeout))));	 //!(r1 || r2 || r3 ||
	if(strstr(g_u8RecData,"GNRMC"))
	{
    u32ADC0Result = 3;
    ADC_START_CONV(ADC);
    u32ADC0Result = ADC_GET_CONVERSION_DATA(ADC, 0);
    u32ADC0Result = (3.943/2.097)*((u32ADC0Result*3.312) /4096);
    
		memset(temp,0,100);
		parse_g(g_u8RecData, 2, 10, 'C', ',' , temp);
		strcat(g_u8SendData,imei);
		strcat(g_u8SendData,",");
		strcat(g_u8SendData,temp);
		memset(temp,0,100);
		sprintf(temp,",F=%.1f\n",u32ADC0Result);
		strcat(g_u8SendData,temp);
		
  }
  if((strlen(g_u8SendData) > 2900))
  {
    memset(g_u8SendData,0,TXBUFSIZE);
    strcat(g_u8SendData,imei);
    strcat(g_u8SendData,"error:RAMfull\n");
  }
	PB2=1;

//	osMutexRelease(uart_mutex_id);
//	osDelay(1);
}







__inline int8_t checkallnumsinstring(char* checkstring)
{
  int returnval, checkstringptr, lencheckstring=0;
  lencheckstring  = strlen(checkstring);
  for(checkstringptr  = 0 ; checkstringptr < lencheckstring ; checkstringptr++)
  {
    if((checkstring[checkstringptr]  > 47 ) && (checkstring[checkstringptr]  < 58 ))
    {returnval  = 0;}
    else
    {returnval  = 1;break;}
  }
  return returnval;
}


void Send_FS(void)
{

		TCP_Send_ch("\r\nAT+QISEND\r\n",g_u8SendData,">","ERROR","SEND OK",10);	

 
}






void I2C1_IRQHandler(void)
{

    // clear interrupt flag
    I2C1->INTSTS = I2C_INTSTS_INTSTS_Msk;

    u32Status = I2C1->STATUS;

}




/**
 * @brief       This function checks the status of I2C, sets the related control bit and data if needed
 *      when this I2C module is master receiver.
 *
 * @param[in]   u32Status   The current value of I2C status register.
 *
 * @return      None
 */

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08) {                    /* START has been transmitted and prepare SLA+W */
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr << 1)); /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    } else if (u32Status == 0x18) {             /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    } else if (u32Status == 0x20) {             /* SLA+W has been transmitted and NACK has been received */
        I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_STO | I2C_SI);
    } else if (u32Status == 0x28) {             /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen != 2) {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_SI);
        } else {
            I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_SI);
        }
    } else if (u32Status == 0x10) {             /* Repeat START has been transmitted and prepare SLA+R */
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr << 1) | 0x01);  /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    } else if (u32Status == 0x40) {             /* SLA+R has been transmitted and ACK has been received */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    } else if (u32Status == 0x58) {             /* DATA has been received and NACK has been returned */
        g_u8RxData = I2C_GET_DATA(I2C1);
        I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);
        g_u8EndFlag = 1;
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08) {                    /* START has been transmitted */
        I2C_SET_DATA(I2C1, g_u8DeviceAddr << 1);  /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    } else if (u32Status == 0x18) {             /* SLA+W has been transmitted and ACK has been received */
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_SI);
    } else if (u32Status == 0x20) {             /* SLA+W has been transmitted and NACK has been received */
        I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_STO | I2C_SI);
    } else if (u32Status == 0x28) {             /* DATA has been transmitted and ACK has been received */
        if (g_u8DataLen != 3) {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_SI);
        } else {
            I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);
            g_u8EndFlag = 1;
        }
    } else {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}




/**
 * @brief       This function do the I2C data writing to EEPROM device.
 *
 * @param[in]   u16Address  An address of EEPROM that will be written.
 *
 * @param[in]   u18Data     The data will be written to EEPROM.
 *
 * @return      None
 */
void I2C_Write(uint16_t u16Address, uint8_t u8Data)
{
  I2C_SET_CONTROL_REG(I2C1, I2C_STA );
  while(u32Status != 0x08);    //ST ack
  I2C_SET_DATA(I2C1, ((g_u8DeviceAddr << 1)));   
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);
  while(u32Status != 0x18);   //SAD+W ack
  I2C_SET_DATA(I2C1, ((u16Address) ));   
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);  
  while(u32Status != 0x28);   //SAK 
  u32Status = 0;
  I2C_SET_DATA(I2C1, ((u8Data)));  
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);  
  while(u32Status != 0x28);  
  
  
            //  I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_SI);   // SUB
            //  while(u32Status != 0x10);   //SUB ack 
            //  I2C_SET_DATA(I2C1, ((g_u8DeviceAddr << 1) | (0x01) ));   
            //  I2C_SET_CONTROL_REG(I2C1, I2C_SI);
//  while(u32Status != 0x40);
//  I2C_SET_CONTROL_REG(I2C1, I2C_SI);  
//  while(u32Status != 0x58);
//  g_u8RxData = I2C_GET_DATA(I2C1);
  I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);  
  
//    g_au8TxData[0] = u16Address >> 8;
//    g_au8TxData[1] = u16Address & 0xFF;
//    g_au8TxData[2] = u8Data;

//    g_u8DataLen = 0;
//    g_u8EndFlag = 0;

//    /* I2C function to write data to slave */
//    s_I2CHandlerFn = (I2C_FUNC)I2C_MasterTx;

//    /* I2C as master sends START signal */
//    I2C_SET_CONTROL_REG(I2C1, I2C_STA);

//    /* Wait I2C Tx Finish */
//    while (g_u8EndFlag == 0);
}

/**
 * @brief       This function do the I2C data reading from EEPROM device.
 *
 * @param[in]   u16Address  An address of EEPROM that will be read.
 *
 * @return      The data be read out.
 */
uint8_t I2C_Read(uint16_t u16Address){
  
  I2C_SET_CONTROL_REG(I2C1, I2C_STA );
  while(u32Status != 0x08);
  I2C_SET_DATA(I2C1, ((g_u8DeviceAddr << 1)));   
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);
  while(u32Status != 0x18);
  I2C_SET_DATA(I2C1, ((u16Address)));   
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);  
  while(u32Status != 0x28);
  I2C_SET_CONTROL_REG(I2C1, I2C_STA | I2C_SI);
  while(u32Status != 0x10);   
  I2C_SET_DATA(I2C1, ((g_u8DeviceAddr << 1) | (0x01) ));   
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);
  while(u32Status != 0x40);
  I2C_SET_CONTROL_REG(I2C1, I2C_SI);  
  while(u32Status != 0x58);
  g_u8RxData = I2C_GET_DATA(I2C1);
  I2C_SET_CONTROL_REG(I2C1, I2C_STO | I2C_SI);  
//    g_au8TxData[0] = u16Address >> 8;
//    g_au8TxData[1] = u16Address & 0xFF;
////    g_au8TxData[0] = u16Address;
//  
//    g_u8DataLen = 0;
//    g_u8EndFlag = 0;

//    /* I2C function to write data to slave */
//    s_I2CHandlerFn = (I2C_FUNC)I2C_MasterRx;

//    /* I2C as master sends START signal */
//    I2C_SET_CONTROL_REG(I2C1, I2C_STA);

//    /* Wait I2C Tx Finish */
//    while (g_u8EndFlag == 0);

    return g_u8RxData;
}



    






