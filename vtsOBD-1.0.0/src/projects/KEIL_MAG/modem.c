#include "uart.h"
#include "string.h"
#include "gpios.h"
#include <math.h>
//#include "stdlib.c"
void clear2(void), clear(void), read(char * A,char * B,char * C), read2(char * respA,char * respB,char * respC, int timeout), send_modem(char ch[]),  send_modem2(char ch[]), log_it(), parse_g(char* str, int first, int sec,	char f,		char s , char *string); 
//extern-static MISRA-C rule 8.8
extern int myAtoi(char *str);
extern   char * cond201;char * cond200;
extern void delay(int delaytime),  LED_state(void) , accel_read(void), detect_motion(),send_chunk();
//--------------------
void send_GSM(char *string, char *responseA, char *responseB, char *responseC, int32 timeout);
//extern-static MISRA-C rule 8.8
extern char string_gps[100],gga[100],rmc[100],uart_string_gsm[5000],uart_string_obd[3000], at_[100], fsize[15], ip[16],port[10],interval[5],drive[5],lptmr_interrupt;
//---------------------------------
char uart_string_gsm[5000],data[1500],num2[30];
extern  char imei[20],apn[25];
extern volatile uint8_t stateCount;	
extern float stof(const char* s);
extern int motion,motion_samples,int_erval;	
extern char* test;extern int32 tmeout;
extern int sleep_flag;
extern long times,qwe,k5,enters,cursorvaluepr,cursorvaluene;
extern int pidcounter;

int k,l,k2,k23,l2;
extern void HardFault_Handler(void);
extern void remove_all_chars(char* str, char c, char d);
int tout=0,tout2=0;
long time=999999;
extern char mystrstr(char* str1, char character);

extern char obdrespbinary[33];
extern char suppportedpid[100][7];
extern char obdresp[20];
extern char tempobdresp[20];

int temp;int flagch=0;
	char cond121;
	char* cond122;
	char* cond123;
	char* cond124;
	int len;
char cond;
char* con1d;
static	char* stcond1;
char * stcond2;char * stcond3;char * stcond4;
extern long qwe;
long time111,timeout222;
int qweq;

int i11=0,j11=0,temp11=0;
int sz1,sz2;


__inline void clearobd()
{

  if (UART_S1_REG(UART0_BASE_PTR) & UART_S1_OR_MASK)
    {
      temp = UART_D_REG(UART0_BASE_PTR);
		}
		
		UART_S1_REG(UART0_BASE_PTR) |= (UART_S1_OR_MASK | UART_C2_ILIE_MASK );
	
}
__inline void saveapn(){
	
send_GSM("\nAT+FSCREATE=C:\\APN.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);

	memset(at_,0,100);
	sprintf(at_,"\r\nAT+FSWRITE=C:\\APN.TXT,0,%d,10\r\n",strlen(apn)+3);	
	send_GSM(at_, ">", "ERROR\r\n" , "ERROR:",10);	
	//clear();

	
	send_modem("|");
		for(k=0;k<strlen(apn);k++)
		{
			uart_putchar(UART1_BASE_PTR, apn[k]);// uart0_putchar(UART0_BASE_PTR, interval[k]);//clear();
		}	
			send_modem("|");			
 
									k2=0;									//string array counter for uart 1
									time=19999;						//timer counts to wait for characters in uart 1 									
									while(!(strstr(uart_string_gsm,"OK\r\n")  ||  (time < 1)  ||(strstr(uart_string_gsm, "ERROR"))))
									{
									time--;
									if(time % 2000 == 0)
										{
											//uart0_putchar(UART0_BASE_PTR,'1');
											uart_putchar(UART1_BASE_PTR,'\n');
										}
	
									}


	
}

__inline void readapn()
{
	memset(at_,0,100);
		sprintf(at_, "\nAT+FSREAD=C:\\APN.TXT,0,100,1\r\n");		
		send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);	
	
		parse_g(uart_string_gsm, 1,2, '|', '|',apn);
	
	
}



__inline void save_interval()
{
	
send_GSM("\nAT+FSCREATE=C:\\INTERVAL.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);

	memset(at_,0,100);
	sprintf(at_,"\r\nAT+FSWRITE=C:\\INTERVAL.TXT,0,%d,10\r\n",strlen(interval)+11);	
	send_GSM(at_, ">", "ERROR\r\n" , "ERROR:",10);	
	//clear();

	
	send_modem("|INTERVAL|");
		for(k=0;k<strlen(interval);k++)
		{
			uart_putchar(UART1_BASE_PTR, interval[k]);// uart0_putchar(UART0_BASE_PTR, interval[k]);//clear();
		}	
			send_modem("|");			
 
									k2=0;									//string array counter for uart 1
									time=19999;						//timer counts to wait for characters in uart 1 									
									while(!(strstr(uart_string_gsm,"OK\r\n")  ||  (time < 1)  ||(strstr(uart_string_gsm, "ERROR"))))
									{
									time--;
									if(time % 2000 == 0)
										{
											//uart0_putchar(UART0_BASE_PTR,'1');
											uart_putchar(UART1_BASE_PTR,'\n');
										}
	
									}


	
}	
	

	
__inline void save_ip_port()
{
	
send_GSM("\nAT+FSCREATE=C:\\IP_PORT.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);
	
	memset(at_,0,100);
	sprintf(at_,"\r\nAT+FSWRITE=C:\\IP_PORT.TXT,0,%d,10\r\n",strlen(ip)+12+strlen(port));	
send_GSM(at_, ">", "ERROR\r\n" , "ERROR:",10);	
	clear();

	
	send_modem("|IP|");
		for(k=0;k<strlen(ip);k++)
		{
			uart_putchar(UART1_BASE_PTR, ip[k]);// uart0_putchar(UART0_BASE_PTR, ip[k]);//clear();
		}	
	send_modem("|PORT|");			
		for(k=0;k<strlen(port);k++)
		{
			uart_putchar(UART1_BASE_PTR, port[k]);// uart0_putchar(UART0_BASE_PTR, port[k]);//clear();
		}	
send_modem("|");			
									//memset(data,0,1000);
									k2=0;									//string array counter for uart 1
									time=19999;						//timer counts to wait for characters in uart 1 									
									while(!(strstr(uart_string_gsm,"OK\r\n")  ||  (time < 1)  ||(strstr(uart_string_gsm, "ERROR"))))
									{
									time--;
									if(time % 2000 == 0)
										{
											uart0_putchar(UART0_BASE_PTR,'1');
											uart_putchar(UART1_BASE_PTR,'\n');
										}
	
									}

	
}	
	
__inline void ignstaton(){
send_GSM("\nAT+FSCREATE=C:\\IGN.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);
send_GSM("\r\nAT+FSWRITE=C:\\IGN.TXT,0,6,10\r\n", ">", "ERROR\r\n" , "ERROR:",5);
	delay(10);
send_GSM("\nION\n\n\n\n", "OK\r\n", "ERROR\r\n" , "ERROR:",10);


}	
__inline void ignstatoff(){
send_GSM("\nAT+FSCREATE=C:\\IGN.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);
send_GSM("\r\nAT+FSWRITE=C:\\IGN.TXT,0,6,10\r\n", ">", "ERROR\r\n" , "ERROR:",5);
	delay(10);
send_GSM("\nIOFF\n\n\n\n", "OK\r\n", "ERROR\r\n" , "ERROR:",10);


}		


__inline void ignread(){
memset(at_,0,100);
		sprintf(at_, "\nAT+FSREAD=C:\\IGN.TXT,0,100,1\r\n");		
		send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);	
if(strstr(uart_string_gsm,"ION")){IGNITION_ON;FUEL_ON;}
else if(strstr(uart_string_gsm,"IOFF")){IGNITION_OFF;FUEL_OFF;}

}	
	
	
__inline void read_ip_port()
{
		memset(at_,0,100);
		sprintf(at_, "\nAT+FSREAD=C:\\IP_PORT.TXT,0,100,1\r\n");		
		send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);	
		parse_g(uart_string_gsm, 2,3, '|', '|',ip);
		parse_g(uart_string_gsm, 4,5, '|', '|',port);
	
}		
	
__inline void read_interval()
{
		memset(at_,0,100);
		sprintf(at_, "\nAT+FSREAD=C:\\INTERVAL.TXT,0,100,1\r\n");		
		send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);	
		parse_g(uart_string_gsm, 2,3, '|', '|',interval);
		int_erval=myAtoi(interval);		


}	



	__inline void savecursor(){
send_GSM("\nAT+FSCREATE=C:\\CURSOR.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);
send_GSM("\r\nAT+FSWRITE=C:\\CURSOR.TXT,0,50,10\r\n", ">", "ERROR\r\n" , "ERROR:",5);
	delay(10);
memset(at_,0,100);
sprintf(at_, "\n,%ld,%ld,%ld,\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",times,k5,cursorvaluepr);		
send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);
}	
	__inline void savecursord(){
send_GSM("\nAT+FSCREATE=C:\\CURSOR.TXT\r\n", "OK", "ERROR\r\n" , "ERROR:",tmeout);
send_GSM("\r\nAT+FSWRITE=C:\\CURSOR.TXT,0,50,10\r\n", ">", "ERROR\r\n" , "ERROR:",5);
	delay(10);
memset(at_,0,100);
sprintf(at_, "\n,%ld,%ld,%ld,\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",0,0,0);		
send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);
}	
		
	
__inline void readcursor()
{
		memset(at_,0,100);
		sprintf(at_, "\nAT+FSREAD=C:\\CURSOR.TXT,0,100,1\r\n");		
		send_GSM(at_, "OK", "ERROR\r\n" , "ERROR:",10);	
		memset(at_,0,100);
		parse_g(uart_string_gsm, 4,5, ',', ',',at_);
		times=myAtoi(at_);		
		memset(at_,0,100);
		parse_g(uart_string_gsm, 5,6, ',', ',',at_);
		k5=myAtoi(at_);		
		memset(at_,0,100);
		parse_g(uart_string_gsm, 6,7, ',', ',',at_);
		cursorvaluepr=myAtoi(at_);		


}	
	

__inline void send_modem(char ch[])
	{	
						unsigned char i20=0;
						for(i20=0;i20<strlen(ch);i20++)
						{
							uart_putchar(UART1_BASE_PTR, ch[i20]);//clear();
						}
						
	}

__inline void send_obd(char ch[])
	{	unsigned char i20=0;
		
		
		memset(uart_string_gsm,0,5000);k2=0;
		
						
						for(i20=0;i20<strlen(ch);i20++)
						{
							uart_putchar(UART1_BASE_PTR, ch[i20]);//clear();
						}
		
	}
	
	
	
void send_modem2(char ch[])
	{	
						unsigned char i21;
						i21=0;
						while ((*(ch+i21)) != '\0') // NULL
						{	
						// delay(1);
						 uart_putchar(UART2_BASE_PTR, (*(ch+i21)));
						 i21++;
						}
		
	}	
	
__inline void clear()
{
  if (UART_S1_REG(UART1_BASE_PTR) & UART_S1_OR_MASK)
    {
      temp = UART_D_REG(UART1_BASE_PTR);
		}
		
		UART_S1_REG(UART1_BASE_PTR) |= (UART_S1_OR_MASK | UART_C2_ILIE_MASK );
}	





__inline void clear2()
{
//    if (UART_S1_REG(UART2_BASE_PTR) & UART_S1_OR_MASK)
//    {
//      temp = UART_D_REG(UART2_BASE_PTR);
//    }

	UART_S1_REG(UART2_BASE_PTR) |= (UART_S1_OR_MASK | UART_C2_ILIE_MASK );
}	



__inline int hextodecimal(char *hex){
	
	 double decimal, place;
    double val, lentt;

	int hex2deci;
	 
    decimal = 0;
    place = 1;
	   /* Finds the length of total number of hex digit */
    lentt = strlen(hex); 
    lentt--;
     
    /*
     * Converts the hexadecimal number to decimal number
     * using algorithm decimal = decimal + (hex[i] * 16 ^ digit)
     */
    for(hex2deci=0; hex[hex2deci]!='\0'; hex2deci++)
    {
        /*
         * Finds the decimal each equivalent hexadecimal digit
         */
        switch(hex[hex2deci])
        {
            case '0':
                val = 0;
                break;
            case '1':
                val = 1;
                break;
            case '2':
                val = 2;
                break;
            case '3':
                val = 3;
                break;
            case '4':
                val = 4;
                break;
            case '5':
                val = 5;
                break;
            case '6':
                val = 6;
                break;
            case '7':
                val = 7;
                break;
            case '8':
                val = 8;
                break;
            case '9':
                val = 9;
                break;
            case 'a':
            case 'A':
                val = 10;
                break;
            case 'b':
            case 'B':
                val = 11;
                break;
            case 'c':
            case 'C':
                val = 12;
                break;
            case 'd':
            case 'D':
                val = 13;
                break;
            case 'e':
            case 'E':
                val = 14;
                break;
            case 'f':
            case 'F':
                val = 15;
                break;
        }
 
        decimal += val * pow(16, lentt);
        lentt--;
    }
	
return decimal;	
	
}

	



__inline void hextobinary(char* hex, char* bin){
	int cou;
			cou=0;
		while(hex[cou]){
         switch(hex[cou]){
             case '0': strcat(bin,"0000"); break;
             case '1': strcat(bin,"0001"); break;
             case '2': strcat(bin,"0010"); break;
             case '3': strcat(bin,"0011"); break;
             case '4': strcat(bin,"0100"); break;
             case '5': strcat(bin,"0101"); break;
             case '6': strcat(bin,"0110"); break;
             case '7': strcat(bin,"0111"); break;
             case '8': strcat(bin,"1000"); break;
             case '9': strcat(bin,"1001"); break;
             case 'A': strcat(bin,"1010"); break;
             case 'B': strcat(bin,"1011"); break;
             case 'C': strcat(bin,"1100"); break;
             case 'D': strcat(bin,"1101"); break;
             case 'E': strcat(bin,"1110"); break;
             case 'F': strcat(bin,"1111"); break;
             case 'a': strcat(bin,"1010"); break;
             case 'b': strcat(bin,"1011"); break;
             case 'c': strcat(bin,"1100"); break;
             case 'd': strcat(bin,"1101"); break;
             case 'e': strcat(bin,"1110"); break;
             case 'f': strcat(bin,"1111"); break;
             default:  printf("\nInvalid hexadecimal digit %c ",hex[cou]); 
         }
         cou++;
    }


}


//__inline void send_OBD(char* OBDstring)//, char *OBDresponseA, char* OBDresponseB, char* OBDresponseC, int32 OBDtimeout)
//{
//	unsigned char i20=0;
//	memset(uart_string_obd,0,3000);k23=0;
//		for(i20=0;i20<strlen(OBDstring);i20++)
//		{
//			uart0_putchar(UART0_BASE_PTR, OBDstring[i20]);//clear();
//		}
//	uart0_putchar(UART0_BASE_PTR, '\r');	
//	uart0_putchar(UART0_BASE_PTR, '\n');
//	while(!strstr(uart_string_obd,">"));
//	clear();
//	delay(10);
//	
//}


__inline void send_OBD(char* OBDstring)//, char *OBDresponseA, char* OBDresponseB, char* OBDresponseC, int32 OBDtimeout)
{
	unsigned char i20=0;
tout=0;
	//clearobd();
		memset(uart_string_obd,0,3000);k23=0;
		for(i20=0;i20<strlen(OBDstring);i20++)
		{
			uart0_putchar(UART0_BASE_PTR, OBDstring[i20]);//clear();
		}
			uart0_putchar(UART0_BASE_PTR, '\r');	

//	uart0_putchar(UART0_BASE_PTR, '\n');
	delay(100);
	while(!(strstr(uart_string_obd,">") || (tout > 5)));
//	delay(2000);
	//clearobd();

}



__inline void send_GSM(char *string, char *responseA, char *responseB, char *responseC, int32 timeout)
{	 

//							//uart_putchar(UART1_BASE_PTR,0xA);
//							//uart_putchar(UART1_BASE_PTR,0x1A);
//							clear();
//							tout=0;
//							//	qwe=strlen(uart_string_gsm);
//							memset(uart_string_gsm,0,5000);
//							k2=0;									//string array counter for uart 1
//							//timer counts to wait for characters in uart 1 
//							clear();
//							send_modem(string);
//							uart_putchar(UART1_BASE_PTR,'\n');	uart_putchar(UART1_BASE_PTR,'\n');	
//							delay(10);
//							//	uart_putchar(UART1_BASE_PTR,'\r');
//							//uart_putchar(UART1_BASE_PTR,'\r');uart_putchar(UART1_BASE_PTR,'\n');//uart_putchar(UART1_BASE_PTR,0x1A);
//							do
//							{

//							//					time111--;
//							//				  timeout222--;


//							cond122=strstr(uart_string_gsm, responseA);
//							cond123=strstr(uart_string_gsm, responseB);
//							cond124=strstr(uart_string_gsm, responseC);
//							cond121 = (cond122	|| cond123	|| cond124  || (tout > timeout))	;

//							}while (!cond121);
//							clear();
//							if(tout>timeout){
//							uart_putchar(UART1_BASE_PTR,'\r');
//							uart_putchar(UART1_BASE_PTR,'\n');uart_putchar(UART1_BASE_PTR,0x1A);
//							uart_putchar(UART1_BASE_PTR,0x1A);uart_putchar(UART1_BASE_PTR,'\r');
//							uart_putchar(UART1_BASE_PTR,'\n');}
//							//return;
//							//		return uart_string_gsm;




}



__inline char * send_MSG(char *string, char *responseA, char *responseB, char *responseC, int32 timeout)
{	
		qwe=strlen(uart_string_gsm);

	
		memset(uart_string_gsm,0,qwe);
		k2=0;									//string array counter for uart 1
	  time111=99999;						//timer counts to wait for characters in uart 1 
		clear();
		timeout222=2001;
		send_modem(string);
		do	{
			
					time111--;
				  timeout222--;
//          if(lptmr_interrupt == 1){
//					//accel_read();
//					detect_motion();
//					lptmr_interrupt = 0;}		
//					if(timeout222 % 400 == 0)
//					{
//						//uart0_putchar(UART0_BASE_PTR,'1');uart_putchar(UART1_BASE_PTR,0x1A);
//					}
			
	cond122=strstr(uart_string_gsm, responseA);
	cond123=strstr(uart_string_gsm, responseB);
  cond124=strstr(uart_string_gsm, responseC);
	cond121 = (cond122	|| cond123	|| cond124  || (time111<=1))	;			
			
			
		}while (!cond121);
		clear();
		return uart_string_gsm;

}



__inline void parse(char* str, int first, int sec, char *string)
{

		delay(10);		
		memset(string,0,15);
		for(i11=0,temp11=0;i11<100;i11++)
		{
		if(str[i11]==',')
			{temp11++;j11=i11;}
		else{}
			
		if(temp11>=sec){i11=900;}
		else{}	
		
		}	
		
		for(i11=0,temp11=0;i11<100;i11++)
		{
		if(str[i11]==',')
			{temp11++;l=i11;}										
		else{}
		if(temp11>=first){i11=900;}else{}
		}	
		for(i11=0;i11<(j11-l-1);i11++)
		{
		string[i11] = str[l+i11+1];
		}			
	

}

void parse_gsm(char *str, int first, int sec, char *string)
{

		delay(10);		
		memset(string,0,15);
		for(i11=0,temp11=0;i11<100;i11++)
		{
		if(str[i11]=='"')
			{temp11++;j11=i11;}
		else{}
			
		if(temp11>=sec){i11=900;}
		else{}	
		
		}	
		
		for(i11=0,temp11=0;i11<100;i11++)
		{
		if(str[i11]=='"')
			{temp11++;l=i11;}										
		else{}
		if(temp11>=first){i11=900;}else{}
		}	
		for(i11=0;i11<(j11-l-1);i11++)
		{
		string[i11] = str[l+i11+1];
		}			
	

}



__inline void parse_g(char* str, int first, int sec, char f, char s , char *string)
{

		sz1=	strlen(str);
		sz2=  strlen(string);
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

__inline int stringtoint(char* num){
	int dec99 = 0, i99, j99, len99;
	len = strlen(num);
	for(i99=0; i99<len99; i99++){
		dec99 = dec99 * 10 + ( num[i99] - '0' );
	}
	
	return dec99;

	
}




__inline void supportedpid(char command[5]){
	char* lenghtchecker;
	char commandtemp[3];
	int supportedfori;int cmd;
	int  i99, j99, len99,dec99;
	int dec999 = 0;
	//delay(100);
	send_OBD(command);
		memset(obdresp,0,20);
		memset(tempobdresp,0,20);
		memset(commandtemp,0,3);
		parse_g(uart_string_obd, 1, 2, '\r', '\r' ,tempobdresp);
		remove_all_chars(tempobdresp,' ','\r');	
		strcpy(obdresp, &tempobdresp[4]);
		strcpy(commandtemp, &command[2]);
		memset(obdrespbinary,0,32);
		hextobinary( obdresp,  obdrespbinary);
	//	remove_all_chars(command,'\r','\n');	
		
	
	
//		len99 = strlen(command);
//		for(i99=0; i99<len99; i99++){
//		dec99 = dec99 * 10 + ( command[i99] - '0' );
//		}
	
		strcpy(obdresp, &tempobdresp[4]);	
		dec999=hextodecimal(commandtemp);					
	
		for(supportedfori=0;supportedfori<32;supportedfori++)
		{
			if(obdrespbinary[supportedfori]=='1' && !strstr(tempobdresp,"NODATA") && !strstr(tempobdresp,"SEARCHING"))
				{

					if(dec999+supportedfori+1<0x10){
												sprintf(suppportedpid[pidcounter],"%c%c%c%x",command[0],command[1],command[2],dec999+supportedfori+1);
												pidcounter++;
					}
					else
					{
												sprintf(suppportedpid[pidcounter],"%c%c%x",command[0],command[1],dec999+supportedfori+1);
												pidcounter++;
						
					}

				}
		}
}
