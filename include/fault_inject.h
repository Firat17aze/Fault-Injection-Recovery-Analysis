
#ifndef FAULT_INJECT_H
#define FAULT_INJECT_H

#include <stdint.h>


volatile uint32_t* fault_get_victim_ptr(void);

void fault_set_victim_ptr(volatile uint32_t *ptr);


void fault_inject_execute(void);

#endif /* FAULT_INJECT_H */
// ...existing code...
