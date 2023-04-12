#include "mcc_generated_files/mcc.h"
#include "tick.h"

static volatile uint32_t device_time;

void Tick_Init_SES(void) {
    device_time = 0;
    TMR1_SetInterruptHandler(&rtcc_handle);
}

void rtcc_handle(void) {
    device_time++;
}

uint32_t Get_millis(void) {
    return device_time;
}

void delay_ms(uint16_t count) {
    volatile uint32_t _DowCount;
    _DowCount = Get_millis();
    while (((uint32_t) Get_millis() - (uint32_t) _DowCount) <= (uint32_t) count);
}
