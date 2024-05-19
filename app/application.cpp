#include "main.h"
#include "application.h"
#include "lcd.h"

void BlinkLed()
{
	while(1)
	{
		LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    	HAL_Delay(100);
	}
}

void PrintLcdText()
{
	LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
	Lcd1309* lcd_display = new Lcd1309;
	lcd_display->Clear();
	//ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	lcd_display->Print("Andrei Fursenko: First of all, in terms of the transition to a new education system.");
}

void Application()
{
	LL_Init1msTick(SysTick_CTRL_COUNTFLAG_Msk);
	PrintLcdText();
}