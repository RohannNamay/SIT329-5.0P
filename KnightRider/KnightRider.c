#include "KnightRider.h"
#include "sam.h"

const uint8_t ledPins[NUM_LEDS] = {18, 20, 21, 16, 19, 17};

volatile KR_Mode currentMode = STOPPED;
volatile bool buttonPressed = false;
volatile uint8_t buttonPressCount = 0;

volatile int krIndex = 0;
volatile bool krForward = true;
volatile uint16_t tcc_msCount = 0;
volatile uint32_t lastButtonTime = 0;

void KnightRider_init(void) {
  
    for (int i = 0; i < NUM_LEDS; i++) {
        PORT->Group[0].DIRSET.reg = (1 << ledPins[i]);
        PORT->Group[0].OUTCLR.reg = (1 << ledPins[i]);
    }

    PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

    // Configure GCLK for TCC0
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TCC0_TCC1 |
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);

    // Reset TCC0
    TCC0->CTRLA.bit.SWRST = 1;
    while (TCC0->SYNCBUSY.bit.SWRST);

    // Prescaler and period for ~500 ms tick
    TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1024;
    TCC0->PER.reg = 46875; // 48 MHz / 1024 ≈ 46.875 kHz → 0.5s
    while (TCC0->SYNCBUSY.bit.PER);

    // Enable overflow interrupt
    TCC0->INTENSET.reg = TCC_INTENSET_OVF;
    NVIC_EnableIRQ(TCC0_IRQn);

    // Start timer
    TCC0->CTRLA.bit.ENABLE = 1;
}

// Timer ISR (called every 500 ms)
void TCC0_Handler(void) {
    if (TCC0->INTFLAG.bit.OVF) {
        TCC0->INTFLAG.reg = TCC_INTFLAG_OVF;

        tcc_msCount += KNIGHT_DELAY; // software ms counter for debounce

        if (currentMode == RUNNING) {
            // Turn off all LEDs
            for (int i = 0; i < NUM_LEDS; i++)
                PORT->Group[0].OUTCLR.reg = (1 << ledPins[i]);

            // Turn on current LED
            PORT->Group[0].OUTSET.reg = (1 << ledPins[krIndex]);

            // Update index
            if (krForward) {
                krIndex++;
                if (krIndex >= NUM_LEDS - 1) krForward = false;
            } else {
                krIndex--;
                if (krIndex <= 0) krForward = true;
            }
        } else if (currentMode == RESET) {
            // Turn off all LEDs and reset index
            for (int i = 0; i < NUM_LEDS; i++)
                PORT->Group[0].OUTCLR.reg = (1 << ledPins[i]);
            krIndex = 0;
            krForward = true;
            currentMode = STOPPED;
        }
    }
}

// Button ISR (D2 / PB10)
void buttonISR(void) {
    uint32_t now = tcc_msCount;
    if ((now - lastButtonTime) > DEBOUNCE_DELAY) {
        buttonPressCount = (buttonPressCount + 1) % 3; 

        switch (buttonPressCount) {
            case 0: currentMode = RESET; break;    // 3rd press
            case 1: currentMode = RUNNING; break;  // 1st press
            case 2: currentMode = STOPPED; break;  // 2nd press
        }

        buttonPressed = true;
        lastButtonTime = now;
    }
}
