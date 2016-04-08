#ifndef __HARDDEV_H__
#define __HARDDEV_H__

#define RUN_ADC0() ADC0CN_bit.ADBUSY=1

void InitOscillator(void);
void InitTimer0(void);
void  Delay16(unsigned short n);
void InitHardware(void);
void InitUART0(void);
#endif