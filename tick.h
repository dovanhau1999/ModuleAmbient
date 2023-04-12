/* 
 * File:   tick.h
 * Author: HAU DO
 *
 * Created on April 10, 2023, 6:07 PM
 */

#ifndef TICK_H
#define	TICK_H

#include <stdint.h>

void Tick_Init_SES(void);
void rtcc_handle(void);
void delay_ms(uint16_t count);
uint32_t Get_millis(void);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* TICK_H */

