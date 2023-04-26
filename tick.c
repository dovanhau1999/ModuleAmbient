#include "mcc_generated_files/mcc.h"
#include "tick.h"

static volatile uint32_t device_time;

void Tick_Init_SES(void) {
    device_time = 0;
    TMR1_SetInterruptHandler(rtcc_handle);
}

void rtcc_handle(void) {
    device_time++;
}

uint32_t Get_millis(void) {
    return device_time;
}


