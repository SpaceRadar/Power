#include <ioc8051f000.h>	// SFR declarations
#include <stdio.h>
#include "Power.h"
#include "HardDev.h"


const __code signed char TempTable[256]=
{
-61, -60, -59, -59, -58, -57, -56, -55, -54, -54, -53, -52, -51, -50, -49, -49, 
-48, -47, -46, -45, -44, -44, -43, -42, -41, -40, -39, -39, -38, -37, -36, -35, 
-34, -34, -33, -32, -31, -30, -29, -28, -28, -27, -26, -25, -24, -23, -23, -22, 
-21, -20, -19, -18, -18, -17, -16, -15, -14, -13, -13, -12, -11, -10, -9, -8, 
-8, -7, -6, -5, -4, -3, -3, -2, -1, 0, 1, 2, 2, 3, 4, 5, 
6, 7, 7, 8, 9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 18, 
19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 
33, 33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 43, 44, 45, 
46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 
59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 69, 70, 71, 72, 
73, 74, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 84, 85, 
86, 87, 88, 89, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99, 
100, 100, 101, 102, 103, 104, 105, 105, 106, 107, 108, 109, 110, 110, 111, 112, 
113, 114, 115, 115, 116, 117, 118, 119, 120, 120, 121, 122, 123, 124, 125, 125, 
126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 
127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127   
};  

unsigned char nTCounter;

tx_packet_t tx_packet[2];
volatile unsigned char ready_packet=0;
unsigned short Voltage_Arr[ADC_LAST];

void StartSendPacket(void)
{
  nTCounter=1;
  TMR3CN_bit.TR3=0;
  P1_bit.P17=0; //Ligth on H4
  TMR3L=0x00;  TMR3H=0x00;
  TMR3CN_bit.TR3=1;  
  SBUF=tx_packet[ready_packet].array[0];  
} 

void MakeUpPacket(void)
{
  unsigned char idx;
  unsigned char CS=0;
  unsigned char makeup_packet;
  
  makeup_packet=(!ready_packet);
  tx_packet[makeup_packet].data.StartFlag=OUT_PACKET_LABEL;
  tx_packet[makeup_packet].data.Flags1=0;
  
  tx_packet[makeup_packet].data.Voltage_Arr[ADC_TEMP]=TempTable[Voltage_Arr[ADC_TEMP]>>3];
  if ((signed char)(tx_packet[makeup_packet].data.Voltage_Arr[ADC_TEMP])<TEMP_MAX)
  {
    tx_packet[makeup_packet].data.Flags1=1<<ADC_TEMP;
  }  
  
  //Voltage_Arr[ADC_M12V]*=0.867;
  //Voltage_Arr[ADC_12V]*=1.026;  
  
  Voltage_Arr[ADC_M12V]*=0.83682008368200836820083682008368;
  Voltage_Arr[ADC_12VA]*=0.83682008368200836820083682008368;
  for(idx=1;idx<ADC_LAST;++idx)
  {     
    tx_packet[makeup_packet].data.Voltage_Arr[idx]=Voltage_Arr[idx]>>3;
    if((tx_packet[makeup_packet].data.Voltage_Arr[idx]>Lower_Limit[idx])&&
       (tx_packet[makeup_packet].data.Voltage_Arr[idx]<Upper_Limit[idx]))
    {
      tx_packet[makeup_packet].data.Flags1|=1<<idx;
    }       
  }
/*  
  tx_packet[makeup_packet].data.Flags2=P2_bit.P25;
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P05<<1;
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P04<<2;
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P06<<3; 
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P07<<4;
*/  

  tx_packet[makeup_packet].data.Flags2=P0_bit.P04;
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P05<<1;
  tx_packet[makeup_packet].data.Flags2|=P2_bit.P25<<2;
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P06<<3; 
  tx_packet[makeup_packet].data.Flags2|=P0_bit.P07<<4;

  for(idx=0;idx<sizeof(tx_packet_t)-1;++idx)
  {
    CS+=tx_packet[makeup_packet].array[idx];
  }
  tx_packet[makeup_packet].data.CS=CS;
  IE_bit.EA=0;
  ready_packet=(!ready_packet);
  IE_bit.EA=1;  
}  


#pragma vector=TI_int
__interrupt void UART_ISR(void)
{
  EIE2_bit.EADC0=0;
  SCON_bit.REN=0;  // UART reception disabled  
  if (SCON_bit.TI)
  {
    SCON_bit.TI=0;    
  
    if(nTCounter<sizeof(tx_packet_t))
    {
      SBUF=tx_packet[ready_packet].array[nTCounter];
      nTCounter++;
    }
    else
    {
      EIE2_bit.EADC0=1;
      SCON_bit.RI=0;
      SCON_bit.REN=1;  // UART reception enabled
    }  
  }
  if (SCON_bit.RI)
  {
    SCON_bit.RI=0;
    if(SBUF==33)
    {
      StartSendPacket();
    }
    else
    {
      SCON_bit.REN=1;  // UART reception disabled
      EIE2_bit.EADC0=1;       
    }  
  }   
} 


#pragma vector=ADCINT_int
__interrupt void ADC0_ISR(void)
{
  static unsigned char cycle_counter=0;
  static unsigned short long_cycle_counter=0;
  static unsigned char led=0;
  unsigned char ADC_channel; 
  ADC0CN_bit.ADCINT=0;
  ADC_channel=AMX0SL;

  if (!cycle_counter)
  {
    Voltage_Arr[ADC_channel]=0;    
  }
  Voltage_Arr[ADC_channel]+=ADC0H;     
 
  if(ADC_channel<(ADC_LAST-1))
  {
     AMX0SL++;
  }
  else
  {
     AMX0SL=0;
     cycle_counter++;
     if (cycle_counter>7)
     {
       cycle_counter=0;
       MakeUpPacket();
       long_cycle_counter++;
       if(long_cycle_counter>250)
       {
           led^=0x01;
           P3_bit.P34=led; 
           long_cycle_counter=0;
       }  
     }  
  }
  
  RUN_ADC0(); 
}

#pragma vector=TF0_int
__interrupt void Timer0_ISR(void)
{
  static unsigned char led=0;
  led^=0x01;
  P3_bit.P34=led; 
}

#pragma vector=TF3_int
__interrupt void Timer3_ISR(void)
{
  TMR3CN_bit.TR3=0;
  TMR3CN_bit.TF3=0;  
  P1_bit.P17=1; 
}

void main()
{   
  WDTCN = 0xDE;
  WDTCN = 0xAD;
  InitHardware();

  XBR2_bit.XBARE=1;  
//  P0=0x00;
//  P2=0x00;
  PRT3CF_bit.P3CF4=1;  //enable H3 output   
  PRT1CF_bit.P1CF7=1;  //enable H4 output   
  XBR2_bit.WEAKPUD=1;
   
  IE_bit.EA=1;
//    StartSendPacket();  
  RUN_ADC0();
#if 1  
  TMR3CN_bit.TR3=0;
  P1_bit.P17=0; //Ligth on H4
  TMR3L=0x00;  TMR3H=0x00;
  TMR3CN_bit.TR3=1;
#endif  
  while(1);
}

