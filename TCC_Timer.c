#include "TCC_Timer.h"
#include <stdbool.h>

volatile uint32_t tcc_msCount = 0;
volatile bool tcc_finished = false;

static uint8_t ledPort = 0;
static uint8_t ledPin  = 17;
static TCC_Callback userCallback = 0;

void TCC_Timer_init(void) {
    PM->APBCMASK.reg |= PM_APBCMASK_TCC0;

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TCC0_TCC1 |
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);

    TCC0->CTRLA.bit.SWRST = 1;
    while (TCC0->SYNCBUSY.bit.SWRST);

    TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV64;
    TCC0->PER.reg = 749;
    while (TCC0->SYNCBUSY.bit.PER);

    TCC0->INTENSET.reg = TCC_INTENSET_OVF;
    NVIC_EnableIRQ(TCC0_IRQn);
    __enable_irq();

    tcc_msCount = 0;
    tcc_finished = false;
}

void TCC_Timer_setLED(uint8_t port, uint8_t pin) {
    ledPort = port;
    ledPin  = pin;
    PORT->Group[ledPort].DIRSET.reg = (1 << ledPin);
    PORT->Group[ledPort].OUTCLR.reg = (1 << ledPin);
}

void TCC_Timer_setCallback(TCC_Callback cb) {
    userCallback = cb;
}

void TCC_Timer_start(void) {
    if (!tcc_finished) {
        TCC0->CTRLA.bit.ENABLE = 1;
        while (TCC0->SYNCBUSY.bit.ENABLE);
    }
}

void TCC_Timer_stop(void) {
    TCC0->CTRLA.bit.ENABLE = 0;
    while (TCC0->SYNCBUSY.bit.ENABLE);
}

// ISR
void TCC0_Handler(void) {
    if (TCC0->INTFLAG.bit.OVF) {
        TCC0->INTFLAG.reg = TCC_INTFLAG_OVF;

        if (!tcc_finished) {
            tcc_msCount++;

            if (tcc_msCount % 1000 == 0) { // LED toggle every second
                PORT->Group[ledPort].OUTTGL.reg = (1 << ledPin);
            }

            if (tcc_msCount >= 60000) { // max period reached
                tcc_finished = true;      // stop future counting
                TCC0->CTRLA.bit.ENABLE = 0; // stop timer
                if (userCallback) {
                    userCallback();
                }
            }
        }
    }
}
