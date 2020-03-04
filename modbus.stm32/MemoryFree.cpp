extern unsigned int __bss_end;
extern unsigned int __heap_start;
//AVR    extern void *__brkval;


#include "MemoryFree.h"

static volatile int longmi[]={12};

int freeMemory() {
  int free_memory=0;
  int dummy[]={12};
  longmi[0]++;
//AVR      if((int)__brkval == 0)
//AVR         free_memory = ((int)&free_memory) - ((int)&__bss_end);
//AVR      else
//AVR    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory = 19768;
    free_memory = &dummy - &longmi;

  return free_memory;
}
