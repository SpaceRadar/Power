#ifndef __POWER_H__
#define __POWER_H__

#define TEMP_MAX 70 
#define TEMP_MIN 0  /* Not used*/

#define V2_MIN   191
#define V2_MAX   234

#define V198_MIN   188
#define V198_MAX   231

#define V165_MIN 159
#define V165_MAX 194

#define OUT_PACKET_LABEL  22

enum
{
  ADC_TEMP,
  ADC_5VA,
  ADC_12VA,
  ADC_5V,
  ADC_33V,
  ADC_M12V,
  ADC_12V,
  ADC_LAST
};

const unsigned char Upper_Limit[ADC_LAST]=
{TEMP_MAX, V2_MAX,V198_MAX,V2_MAX,V165_MAX,V198_MAX,V198_MAX};


const unsigned char Lower_Limit[ADC_LAST]=
{TEMP_MIN, V2_MIN,V198_MIN,V2_MIN,V165_MIN,V198_MIN,V198_MIN};
typedef struct
{  
  unsigned char StartFlag;
  unsigned char Voltage_Arr[ADC_LAST];
  unsigned char Flags1;
  unsigned char Flags2;
  unsigned char CS;  
} packet_data_t;

typedef union
{
  packet_data_t data;
  unsigned char array[sizeof(packet_data_t)];
} tx_packet_t; 



#endif