#include "application.h"
#include "main.h"


void Application()
{
	while(1)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    	HAL_Delay(100);
	}
}