
#include "main.h"
#include "device.h"
#include <stdint.h>

#define DEVICE_MAX 352
#define PWM_NEUTRAL 480
#define DEVICE_MIN 608

void UpdateDeviceSettings(device_settings *device_struct)
{

	TIM17->CCR1  =  PWM_NEUTRAL+device_struct->PWM_Duty;
}

