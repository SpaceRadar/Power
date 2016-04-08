#include <ioc8051f000.h>	// SFR declarations
#include "HardDev.h"


void  Delay16(unsigned short n)
{
  while(n--);
}


void InitADC(void)
{
  AMX0SL=0x00;
  AMX0CF=0x00;      // 0-7 Single Mode

  ADC0CF=0x80;      //Sysclock/16 Gain=1 ///!!! Difference between 005 and 020
                                         //should be 0x80 for 005  
                                         //should be 0x78 for 020
  
  ADC0CN=0x81;      //ADC0 Enabled, left justified
  REF0CN=0x03;                  
//  bset(EIP2,s1);
  EIE2_bit.EADC0=1;    //ADC0 Interrupt Enabled
}


void InitOscillator(void)
{
  OSCXCN = 0x77;
  Delay16(0xFFF);
  while (!OSCXCN & 0x80);
  OSCICN=0x08; //Uses External Oscillator as System Clock
}

void InitUART(void)
{
//  XBR0_bit.SMB0OEN=1;
  
  CKCON_bit.T1M=1; //Timer1 use System Clock / 1
  TL1    = 0xDC;
  TH1    = 0xDC;
  TMOD_bit.T1M1=1 ;  //TMOD: timer 1, mode 2, 8-bit reload
  TMOD_bit.T1M0=0 ;  //TMOD: timer 1, mode 2, 8-bit reload  
  
  TCON_bit.TR1    = 1;     //Timer 1 run 
  XBR0_bit.UARTEN=1;  //UART0 enabled in XBAR 
  SCON =0x50;  //SCON: mode 1, 8-bit UART, enable rcvr
  PCON_bit.SMOD=1; //double baudrate
                        
  IE_bit.ES = 1;        //UART0 interrupt enabled  
//  IP_bit.PS =1;        // Interrupt priority
}

void InitTimer0(void)
{
  TL0=0x00;
  TH0=0x00;
  TMOD_bit.T0M0=1;  // 16-bit Timer0 Mode
  TMOD_bit.T0M1=0;  // 16-bit Timer0 Mode
  IE_bit.ET0=1;     // Enable Timer 0 Interrupt //debug
  TCON_bit.TR0=1;
}

void InitTimer3(void)
{
  TMR3RLL=0x00;
  TMR3RLH=0x00;
  TMR3L=0x00;
  TMR3H=0x00;
  EIE2_bit.ET3=1;
}
  
void InitHardware(void)
{
  InitOscillator();
  InitADC();
  InitUART();
//  InitTimer0();
  InitTimer3();
}  