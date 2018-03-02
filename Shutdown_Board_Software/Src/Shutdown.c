#include "Shutdown.h"

// Global Variables
uint16_t msg;
can_msg_t can_msg;

void mainloop()
{
	/* The main "thread" of shutdown board operation*/



	// send CAN messages necessary
	displayFaultStatus();
	uint16_t lv_battery_fault = (uint16_t) LVBatteryFaulted();
	uint16_t interlock_in_fault = (uint16_t) Interlock_InFaulted();
	uint16_t flt_fault = (uint16_t) FLTFaulted();
	uint16_t flt_nr_fault = (uint16_t) FLT_NRFaulted();
	uint16_t imd_fault = (uint16_t) IMDFaulted();
	uint16_t ams_fault = (uint16_t) AMSFaulted();
	uint16_t bspd_fault = (uint16_t) BSPDFaulted();
	msg = ((lv_battery_fault << 6) | (interlock_in_fault << 5) | (flt_fault << 4) |
		  (flt_nr_fault << 3) | (imd_fault << 2) | (ams_fault << 1) |
		  (bspd_fault));
	CAN_short_msg(&can_msg, create_ID(BID_SHUTDOWN, MID_FAULT_STATUS), msg);
	CAN_queue_transmit(&can_msg);

}

void resetFaults()
{
	/* Resets the Processor-Controlled faults and pulls the reset line */

	// Stop asserting FLT and FLT_NR
	HAL_GPIO_WritePin(FLT_NR_GROUP, FLT_NR_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FLT_GROUP, FLT_PIN, GPIO_PIN_RESET);
	// Pull the reset line low
	HAL_GPIO_WritePin(INTERLOCK_RESET_GROUP, INTERLOCK_RESET_PIN, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim3);
}

void displayFaultStatus()
{
	/* Sets the LEDs according to the current states of fault lines */

	// IMD
	if (IMDFaulted())
	{
		HAL_GPIO_WritePin(IMD_STATUS_GROUP, IMD_STATUS_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(IMD_STATUS_GROUP, IMD_STATUS_PIN, GPIO_PIN_RESET);
	}

	// BSPD
	if (BSPDFaulted())
	{
		HAL_GPIO_WritePin(BSPD_STATUS_GROUP, BSPD_STATUS_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(BSPD_STATUS_GROUP, BSPD_STATUS_PIN, GPIO_PIN_RESET);
	}

	// AMS
	if (AMSFaulted())
	{
		HAL_GPIO_WritePin(AMS_STATUS_GROUP, AMS_STATUS_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(AMS_STATUS_GROUP, AMS_STATUS_PIN, GPIO_PIN_RESET);
	}

	// FLT_NR
	if (FLT_NRFaulted())
	{
		HAL_GPIO_WritePin(FLT_NR_STATUS_GROUP, FLT_NR_STATUS_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(FLT_NR_STATUS_GROUP, FLT_NR_STATUS_PIN, GPIO_PIN_RESET);
	}

	// FLT
	if (FLTFaulted())
	{
		HAL_GPIO_WritePin(FLT_STATUS_GROUP, FLT_STATUS_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(FLT_STATUS_GROUP, FLT_STATUS_PIN, GPIO_PIN_RESET);
	}

	// Interlock In
	if (Interlock_InFaulted())
	{
		HAL_GPIO_WritePin(INTERLOCK_IN_STATUS_GROUP, INTERLOCK_IN_STATUS_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(INTERLOCK_IN_STATUS_GROUP, INTERLOCK_IN_STATUS_PIN, GPIO_PIN_RESET);
	}
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
	// Returns true if the IMD is faulted
	return (!(HAL_GPIO_ReadPin(IMD_OBSERVE_GROUP, IMD_OBSERVE_PIN)));
}

int BSPDFaulted()
{
	// Returns true if the BSPD is faulted
	return (!(HAL_GPIO_ReadPin(BSPD_OBSERVE_GROUP, BSPD_OBSERVE_PIN)));
}

int AMSFaulted()
{
	// Returns true if the AMS is faulted
	return (!(HAL_GPIO_ReadPin(AMS_OBSERVE_GROUP, AMS_OBSERVE_PIN)));
}

int FLTFaulted()
{
	// Returns true if the FLT line is faulted
	return (!(HAL_GPIO_ReadPin(FLT_OBSERVE_GROUP, FLT_OBSERVE_PIN)));
}

int FLT_NRFaulted()
{
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
	battery_voltage = ((float)adc_value)/((float)0x7D0) * 3.3;
	if (battery_voltage < LV_BATTERY_THRESHOLD)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
