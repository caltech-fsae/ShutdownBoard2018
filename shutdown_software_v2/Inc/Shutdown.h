#ifndef SHUTDOWN_H_
#define SHUTDOWN_H_

#include "stm32f4xx_hal.h"
#include "adc.h"
#include "tim.h"
#include "can.h"
#include "identifiers.h"
#include "mycan.h"

#define AMS_STATUS_PIN              GPIO_PIN_1
#define AMS_STATUS_GROUP            GPIOA

#define BSPD_STATUS_PIN             GPIO_PIN_2
#define BSPD_STATUS_GROUP           GPIOA

#define BAT_SENSE_PIN               GPIO_PIN_4
#define BAT_SENSE_GROUP             GPIOA

#define INTERLOCK_IN_STATUS_PIN     GPIO_PIN_0
#define INTERLOCK_IN_STATUS_GROUP   GPIOC

#define FLT_STATUS_PIN              GPIO_PIN_1
#define FLT_STATUS_GROUP            GPIOC

#define FLT_NR_STATUS_PIN           GPIO_PIN_2
#define FLT_NR_STATUS_GROUP         GPIOC

#define IMD_STATUS_PIN              GPIO_PIN_3
#define IMD_STATUS_GROUP            GPIOC

#define INTERLOCK_RESET_PIN         GPIO_PIN_7
#define INTERLOCK_RESET_GROUP       GPIOE

#define IMD_OBSERVE_PIN             GPIO_PIN_8
#define IMD_OBSERVE_GROUP           GPIOE

#define AMS_OBSERVE_PIN             GPIO_PIN_9
#define AMS_OBSERVE_GROUP           GPIOE

#define BSPD_OBSERVE_PIN            GPIO_PIN_10
#define BSPD_OBSERVE_GROUP          GPIOE

#define FLT_NR_OBSERVE_PIN          GPIO_PIN_11
#define FLT_NR_OBSERVE_GROUP        GPIOE

#define FLT_OBSERVE_PIN             GPIO_PIN_12
#define FLT_OBSERVE_GROUP           GPIOE

#define INTERLOCK_IN_OBSERVE_PIN    GPIO_PIN_13
#define INTERLOCK_IN_OBSERVE_GROUP  GPIOE

#define FLT_PIN                     GPIO_PIN_12
#define FLT_GROUP                   GPIOB

#define FLT_NR_PIN                  GPIO_PIN_13
#define FLT_NR_GROUP                GPIOB

#define LV_BATTERY_THRESHOLD		3

#define STARTUP_GRACE_PERIOD		1000
#define IMD_GRACE_PERIOD			4000

#define STATE_GRACE		0
#define STATE_IMD_GRACE 1
#define STATE_RUN 		2

//# of cycles without heartbeat to trigger fault if no core board heartbeat
//TODO: test to verify this number
#define CORE_BOARD_HEARTBEAT_TIMEOUT 2000


// Fault status struct
typedef struct faults_t {
	uint16_t lv_battery_fault;	// set battery fault
	uint16_t interlock_in_fault;
	uint16_t flt_fault;
	uint16_t flt_nr_fault;
	uint16_t imd_fault;
	uint16_t ams_fault;
	uint16_t bspd_fault;
} faults_t;

// Function Prototypes----------------------------------------
void resetFault();
void resetAllFaults();
void displayFaultStatus(faults_t);
void assertFLT();
void assertFLT_NR();
int IMDFaulted();
int BSPDFaulted();
int AMSFaulted();
int FLTFaulted();
int FLT_NRFaulted();
int Interlock_InFaulted();
int LVBatteryFaulted();
uint16_t ADC1_read();
void init();

void mainloop();
faults_t checkFaults();
void checkCANMessages();
void sendHeartbeat();

#endif // SHUTDOWN_H_
