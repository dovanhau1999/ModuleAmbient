/* 
 * File:   main.h
 * Author: HAU DO
 *
 * Created on April 12, 2023, 5:36 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "mcc_generated_files/eusart.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/examples/i2c_master_example.h"
#include "mcc_generated_files/tmr1.h"

enum LED_STATUS {
    OFF_Sensor = 0,
    ON_Sensor = 1,
    ERR_Sensor = 2
};

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

