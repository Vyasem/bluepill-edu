#include "main.h"
#include "application.h"


void Application()
{
	while(1)
	{
		LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    	HAL_Delay(100);
	}
}