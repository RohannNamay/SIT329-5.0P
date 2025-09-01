#ifndef TCC_TIMER_H
#define TCC_TIMER_H

#include "sam.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Software counter
extern volatile uint32_t tcc_msCount;
extern volatile bool tcc_finished;

// Type for user callback
typedef void (*TCC_Callback)(void);

// Function prototypes
void TCC_Timer_init(void);
void TCC_Timer_start(void);
void TCC_Timer_stop(void);
void TCC_Timer_setLED(uint8_t port, uint8_t pin);
void TCC_Timer_setCallback(TCC_Callback cb);

#ifdef __cplusplus
}
#endif

#endif
