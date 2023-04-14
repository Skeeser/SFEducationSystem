#include "delay.h"

/////////////////////delay//////////////////////////////////////
void driver_delay_us(unsigned int xus) // 1us
{
  for (; xus > 1; xus--)
    ;
}

void driver_delay_xms(unsigned long xms) // 1ms
{
  unsigned long i = 0, j = 0;
  for (j = 0; j < xms; j++)
  {
    for (i = 0; i < 256; i++)
      ;
  }
}

void DELAY_S(unsigned int delaytime)
{
  int j, k;
  for (unsigned int i = 0; i < delaytime; i++)
  {
    for (j = 0; j < 4000; j++)
    {
      for (k = 0; k < 222; k++)
        ;
    }
  }
}
