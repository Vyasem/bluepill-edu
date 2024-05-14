#include "lcd.h"
#include <string.h>

#include "main.h"


extern SPI_HandleTypeDef hspi1;

Lcd1309::Lcd1309()
{
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &pageStartAddress, 1, 1);
	HAL_SPI_Transmit(&hspi1, &displayOn, 1, 1);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

void Lcd1309::Print(const char * string)
{
	for(const char * ch = string;  *ch != '\0'; ++ch)
	{
		if(*ch == '\n')
		{
			SwitchNewLine();
			continue;
		}
		Write(*ch);
	}
}

void Lcd1309::Write(uint8_t ch)
{
	uint16_t index = (ch - 32) * Lcd1309::FONT_8;
	uint8_t symbolWidth = font8[index];
	uint16_t counter = index + 1;

	if(columnStartAddress > 120)
	{
		SwitchNewLine();
		if(ch == ' ')
			return;
	}

	uint8_t freeSpace = 2;
	if(ch == 32)
	{
		freeSpace = 0;
	}

	while(counter < (index + (symbolWidth * 2) + freeSpace))
	{
		uint8_t ch = font8[counter];
		HAL_SPI_Transmit(&hspi1, &ch, 1, 10);
		++columnStartAddress;
		ChangePage(pageStartAddress + 1, (columnStartAddress - 1));
		++counter;
		ch = font8[counter];
		HAL_SPI_Transmit(&hspi1, &ch, 1, 10);
		ChangePage(pageStartAddress, columnStartAddress);
		++counter;
	}
}

void Lcd1309::SwitchNewLine()
{
	pageStartAddress += 2;
	if(pageStartAddress > 0xB7)
	{
		pageStartAddress = 0xB0;
	}
	columnStartAddress = 0x0;
	ChangePage(pageStartAddress, columnStartAddress);
}

void Lcd1309::ChangePage(uint8_t page, uint8_t column)
 {
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
	positionSetAddress[0] = page;
	positionSetAddress[2] = column;
	HAL_SPI_Transmit(&hspi1, positionSetAddress, 4, 10);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
 }

void Lcd1309::Clear()
{
	uint8_t pageAddress = pageStartAddress;
	uint8_t message1 = 0x0;
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	for(int page = 0; page < 8; ++page)
	{
		for(int col = 0; col < 128; ++col)
		{
			HAL_SPI_Transmit(&hspi1, &message1, 1, 1);
		}
		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
		pageAddress++;
		HAL_SPI_Transmit(&hspi1, &pageAddress, 1, 10);
		HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &pageStartAddress, 1, 1);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
}