#ifndef KNIGHT_RIDER_H
#define KNIGHT_RIDER_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_LEDS 6
#define KNIGHT_DELAY 500    
#define DEBOUNCE_DELAY 100  

extern const uint8_t ledPins[NUM_LEDS];

// System state
typedef enum {STOPPED, RUNNING, RESET} KR_Mode;
extern volatile KR_Mode currentMode;

// Globals for LEDs
extern volatile int krIndex;
extern volatile bool krForward;
extern volatile uint16_t tcc_msCount;
extern volatile uint32_t lastButtonTime;
extern volatile bool buttonPressed;
extern volatile uint8_t buttonPressCount;

// Initialize system 
void KnightRider_init(void);

// Timer ISR
void TCC0_Handler(void);

// Button ISR
void buttonISR(void);

#endif
