#include "Shutdown.h"

// Global Variables

int core_timeout_counter = CORE_BOARD_HEARTBEAT_STARTUP_TIMEOUT;	// starts at CORE_BOARD_HEARTBEAT_TIMEOUT
// reset every time core board heartbeat received
// counts down to zero, then fault_nr asserted

int state = STATE_WAITING;		// Starting state: waiting for core board to come online

void init() {
	state = STATE_WAITING;
	resetAllFaults();
	HAL_GPIO_WritePin(FLT_GROUP, FLT_PIN, GPIO_PIN_SET);
	core_timeout_counter = CORE_BOARD_HEARTBEAT_STARTUP_TIMEOUT;
}

void mainloop()
{
	if(core_timeout_counter < 0) {
		assertFLT_NR();
	}
	faults_t faults = checkFaults();			// Check for faults
	displayFaultStatus(faults);	// Display fault status on LEDS

	// Send fault status message out on CAN
	uint16_t msg =	  ((faults.lv_battery_fault << 6)
					| (faults.interlock_in_fault << 5)
					| (faults.flt_fault << 4)
					| (faults.flt_nr_fault << 3)
					| (faults.imd_fault << 2)
					| (faults.ams_fault << 1)
					| (faults.bspd_fault));
	can_msg_t can_msg;
	CAN_short_msg(&can_msg, create_ID(BID_SHUTDOWN, MID_FAULT_STATUS), msg);
	CAN_queue_transmit(&can_msg);

	if (faults.lv_battery_fault || faults.interlock_in_fault || faults.imd_fault
		|| faults.ams_fault || faults.bspd_fault) {
		assertFLT_NR();
	}

	if (faults.flt_nr_fault) {
		can_msg_t fault_msg;
		CAN_short_msg(&fault_msg, create_ID(BID_SHUTDOWN, MID_FAULT_NR), 0);
		CAN_queue_transmit(&fault_msg);
		assertFLT_NR();
	}

	if (faults.flt_fault) {
		can_msg_t fault_msg;
		CAN_short_msg(&fault_msg, create_ID(BID_SHUTDOWN, MID_FAULT), 0);
		CAN_queue_transmit(&fault_msg);
	}
	core_timeout_counter--;
}

faults_t checkFaults()
{
	faults_t faults;
	faults.lv_battery_fault = 0; // <- For Testing (uint16_t) LVBatteryFaulted();
	faults.interlock_in_fault = (uint16_t) Interlock_InFaulted();
	faults.flt_fault = (uint16_t) FLTFaulted();
	faults.flt_nr_fault = (uint16_t) FLT_NRFaulted();
	faults.imd_fault = (uint16_t) IMDFaulted();
	faults.ams_fault = (uint16_t) AMSFaulted();
	faults.bspd_fault = (uint16_t) BSPDFaulted();
	return faults;
}

void checkCANMessages()
{
	can_msg_t msg;
	while(CAN_dequeue_msg(&msg)) {
		uint16_t type = 0b0000011111110000 & msg.identifier;
		uint16_t board = 0b00001111 & msg.identifier;
		if(type == MID_FAULT_NR) {
			assertFLT_NR();
		}
		else if(type == MID_HEARTBEAT) {
			if(board == BID_CORE) {
				core_timeout_counter = CORE_BOARD_HEARTBEAT_TIMEOUT;
				if(state == STATE_WAITING) {
					resetFault();
					state = STATE_ONLINE;
				}
			}
		}
	}
}

void sendHeartbeat()
{
	can_msg_t msg;
	CAN_short_msg(&msg, create_ID(BID_SHUTDOWN, MID_HEARTBEAT), 0);
	CAN_queue_transmit(&msg);
}

void resetFault()
{
	// Reset resettable fault
	HAL_GPIO_WritePin(FLT_GROUP, FLT_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FLT_GROUP, FLT_NR_PIN, GPIO_PIN_RESET);
}

void resetAllFaults()
{
	// Reset resettable faults
	resetFault();
	// Reset non-resettable faults
	HAL_GPIO_WritePin(INTERLOCK_RESET_GROUP, INTERLOCK_RESET_PIN, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim3);
}

void displayFaultStatus(faults_t faults)
{
	/* Sets the LEDs according to the current states of fault lines */
	if (faults.imd_fault) // IMD
		HAL_GPIO_WritePin(IMD_STATUS_GROUP, IMD_STATUS_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(IMD_STATUS_GROUP, IMD_STATUS_PIN, GPIO_PIN_RESET);

	if (faults.bspd_fault) // BSPD
		HAL_GPIO_WritePin(BSPD_STATUS_GROUP, BSPD_STATUS_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(BSPD_STATUS_GROUP, BSPD_STATUS_PIN, GPIO_PIN_RESET);

	if (faults.ams_fault) // AMS
		HAL_GPIO_WritePin(AMS_STATUS_GROUP, AMS_STATUS_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(AMS_STATUS_GROUP, AMS_STATUS_PIN, GPIO_PIN_RESET);

	if (faults.flt_nr_fault) // FLT_NR
		HAL_GPIO_WritePin(FLT_NR_STATUS_GROUP, FLT_NR_STATUS_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(FLT_NR_STATUS_GROUP, FLT_NR_STATUS_PIN, GPIO_PIN_RESET);

	if (faults.flt_fault) // FLT
		HAL_GPIO_WritePin(FLT_STATUS_GROUP, FLT_STATUS_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(FLT_STATUS_GROUP, FLT_STATUS_PIN, GPIO_PIN_RESET);

	if (faults.interlock_in_fault) // Interlock In
		HAL_GPIO_WritePin(INTERLOCK_IN_STATUS_GROUP, INTERLOCK_IN_STATUS_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(INTERLOCK_IN_STATUS_GROUP, INTERLOCK_IN_STATUS_PIN, GPIO_PIN_RESET);
}

uint16_t ADC1_read()
{
	/* Samples the ADC1 and returns the value as a uint16_t. Returns FFFF if error */
	/* Must configure "End of Conversion Selection" to
	 * "EOC flag at the end of all conversions" in CubeMX (ADC1 menu) */

	uint16_t ok;
	uint16_t adc_value = 0xFFFF;
	HAL_ADC_Start(&hadc1);
	ok = HAL_ADC_PollForConversion(&hadc1, 1000000);
	if (ok == HAL_OK)
	{
		adc_value = HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);

	return adc_value;
}

void assertFLT()
{
	// Asserts the FLT line low
	HAL_GPIO_WritePin(FLT_GROUP, FLT_PIN, GPIO_PIN_SET);
}

void assertFLT_NR()
{
	// Asserts the FLT_NR line low
	HAL_GPIO_WritePin(FLT_NR_GROUP, FLT_NR_PIN, GPIO_PIN_SET);
}

int IMDFaulted()
{
	// IMD fault is H/W
	// Returns true if the IMD is faulted
	return (!(HAL_GPIO_ReadPin(IMD_OBSERVE_GROUP, IMD_OBSERVE_PIN)));
}

int BSPDFaulted()
{
	// HW fault
	// Returns true if the BSPD is faulted
	return (!(HAL_GPIO_ReadPin(BSPD_OBSERVE_GROUP, BSPD_OBSERVE_PIN)));
}

int AMSFaulted()
{
	// HW fault
	// Returns true if the AMS is faulted
	return (!(HAL_GPIO_ReadPin(AMS_OBSERVE_GROUP, AMS_OBSERVE_PIN)));
}

int FLTFaulted()
{
	// HW fault
	// Returns true if the FLT line is faulted
	return (!(HAL_GPIO_ReadPin(FLT_OBSERVE_GROUP, FLT_OBSERVE_PIN)));
}

int FLT_NRFaulted()
{
	// HW fault
	// Returns true if the FLT_NR line is faulted
	return (!(HAL_GPIO_ReadPin(FLT_NR_OBSERVE_GROUP, FLT_NR_OBSERVE_PIN)));
}

int Interlock_InFaulted()
{
	// Returns true if the FLT_NR line is faulted
	return (!(HAL_GPIO_ReadPin(INTERLOCK_IN_OBSERVE_GROUP, INTERLOCK_IN_OBSERVE_PIN)));
}

int LVBatteryFaulted()
{
	float battery_voltage = 0;
	uint16_t adc_value = ADC1_read();
	battery_voltage = ((float)adc_value)/((float)0x7D0) * 3.3;	// These numbers are incorrect; just a test
	return (battery_voltage < LV_BATTERY_THRESHOLD);
}
