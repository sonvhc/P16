#include "main.h"
#include <stdio.h>
#include <string.h>
#include "flash_memory.h"

extern UART_HandleTypeDef huart1;
extern uint32_t time_record_1, time_record_2, time_record_3 , time_record_4, time_record_5;
extern uint32_t pin_control, stt_play, full_record;

/*******************DangLHb add ham reset và read variable from flash**************/
void resest_flash (void)
{
	pin_control =0;
	full_record = 0;
	stt_play = 0;
	time_record_1 = 0;
	time_record_2 = 0;
	time_record_3 = 0;
	time_record_4 = 0;
	time_record_5 = 0;
	
	write_flash(&pin_control, PIN_CONTROL_T, (sizeof(pin_control)/4));
	HAL_Delay(100);
	write_flash(&full_record, FULL_RECORD, (sizeof(full_record)/4));
	HAL_Delay(100);
	write_flash(&stt_play, STT_PLAY_T, (sizeof(stt_play)/4));			
	HAL_Delay(100);			
	write_flash(&time_record_1, TIME_RECORD_1, (sizeof(time_record_1)/4));
	HAL_Delay(100);			
	write_flash(&time_record_2, TIME_RECORD_2, (sizeof(time_record_1)/4));
	HAL_Delay(100);			
	write_flash(&time_record_3, TIME_RECORD_3, (sizeof(time_record_1)/4));
	HAL_Delay(100);			
	write_flash(&time_record_4, TIME_RECORD_4, (sizeof(time_record_1)/4));
	HAL_Delay(100);			
	write_flash(&time_record_5, TIME_RECORD_5, (sizeof(time_record_1)/4));
}

void read_variable_from_flash(void)
{
	read_flash(&stt_play, STT_PLAY_T);
	HAL_Delay(10);
	read_flash(&pin_control, PIN_CONTROL_T);
	HAL_Delay(10);
	read_flash(&full_record, FULL_RECORD);
	HAL_Delay(10);
	char msg[59];
	sprintf(msg, "\n stt_play = %d, pin_control =%d, full_record = %d",stt_play,pin_control,full_record );
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

	read_flash(&time_record_1, TIME_RECORD_1);
	HAL_Delay(10);
	read_flash(&time_record_2, TIME_RECORD_2);
	HAL_Delay(10);
	read_flash(&time_record_3, TIME_RECORD_3);
	HAL_Delay(10);
	read_flash(&time_record_4, TIME_RECORD_4);
	HAL_Delay(10);
	read_flash(&time_record_5, TIME_RECORD_5);
}
/***********************************************************************************/


uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	int sofar=0;


	   HAL_FLASH_Unlock();



	  uint32_t StartPage = StartPageAddress;
	  uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
	  uint32_t EndPage = EndPageAdress;

	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	   EraseInitStruct.PageAddress = StartPage;
	   EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

	   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	   {
		  return HAL_FLASH_GetError ();
	   }



	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartPageAddress += 4;  
	    	 sofar++;
	     }
	     else
	     {
	    	 return HAL_FLASH_GetError ();
	     }
	   }


	   HAL_FLASH_Lock();

	   return 0;
}

void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartPageAddress;
		StartPageAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}


uint32_t init_flash_memory(void)
{
	uint16_t num_block = 0, num_data = 0, lenOfBlock = 0;
	uint32_t flash_memory[256] = {0};
	uint32_t *ptr = &flash_memory[1];
	int i,j;
	uint8_t typeOfBlock;
	uint32_t none_data = 0;
  //Flash_Write_Data(FLASH_USER_START_ADDR, &num_block, 1);
	HAL_UART_Transmit(&huart1, (uint8_t *)"init_flash_memory", 17, HAL_MAX_DELAY);		
	
	Flash_Read_Data(FLASH_USER_START_ADDR, flash_memory, 1);
	num_block = flash_memory[0] >> 16;
	num_data = flash_memory[0];
			char msg[59];
	if(num_data == num_block)
	{
			Flash_Write_Data(FLASH_USER_START_ADDR, &none_data, 1);
		return 0;
	}
	if(num_data && num_block)
	{
			HAL_UART_Transmit(&huart1, (uint8_t *)"init_flash_memory1", 18, HAL_MAX_DELAY);		
		Flash_Read_Data(FLASH_USER_START_ADDR, flash_memory, (num_data+1));
		for(i = 0;i < num_block;i++)
	  {
	sprintf(msg, "\n num_data = %d ,num_block =%d",num_data,num_block);
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		  typeOfBlock = *ptr >> 16;
		  lenOfBlock = *ptr;
		  num_data -= (lenOfBlock+1);
			ptr += (lenOfBlock + 1);
	  }
	sprintf(msg, "\n num_data = %d ",num_data );
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

		
		if(num_data == 0)
		{
			num_data = flash_memory[0];
			Flash_Write_Data(FLASH_USER_START_ADDR, flash_memory, (num_data + 1));
			return 0;
		}
	}
	Flash_Write_Data(FLASH_USER_START_ADDR, &none_data, 1);
		
		
		
	return 0;
}



int write_flash(uint32_t * data, uint8_t type, uint16_t len)
{
	uint16_t i = 0, j = 0, lenOfBlock;
	uint16_t num_block = 0, num_data = 0;
	uint32_t flash_memory[256] = {0};
	uint32_t *ptr = &flash_memory[1];
	uint8_t typeOfBlock;
	if(len == 0)
		return 0;
	Flash_Read_Data(FLASH_USER_START_ADDR, flash_memory, 1);
	num_block = flash_memory[0] >> 16;
	num_data = flash_memory[0];
	if(num_block)
	{
		Flash_Read_Data(FLASH_USER_START_ADDR, flash_memory, (num_data+1));
	  for(i = 0;i < num_block;i++)
	  {
		  typeOfBlock = *ptr >> 16;
		  lenOfBlock = *ptr;
		  if(typeOfBlock == type)
		  {
			  if(len == lenOfBlock)
				{
					for(j = 0;j < len;j++)
					{
						ptr++;
						*ptr = data[j];
					}
					Flash_Write_Data(FLASH_USER_START_ADDR, flash_memory, (num_data + 1));
					return 0;
				}
				else
				{
					num_block--;
					num_data -= (lenOfBlock + 1);
					for(j = 0;j < (flash_memory + num_data - ptr - lenOfBlock);j++)
					{
						*ptr = *(ptr + lenOfBlock + 1);
					}
					break;
				}
		  }
		  else
		  {
			  ptr += (lenOfBlock + 1);
		  }
	  }
	}
	
	ptr = (&flash_memory[1]) + num_data;
	*ptr = 0;
	*ptr |= (type << 16) | len;
	ptr++;
	for(j = 0;j < len;j++)
	{
		*ptr = data[j];
		ptr++;
	}
	num_block++;
	num_data += (len + 1);
	if(num_data > 255)
		return -1;
	flash_memory[0] = 0;
	flash_memory[0] |= (num_block << 16) | num_data;
	
	
	Flash_Write_Data(FLASH_USER_START_ADDR, flash_memory, (num_data + 1));
	return 0;
}

int read_flash(uint32_t * data, uint8_t type)
{
	uint16_t i = 0, j = 0, lenOfBlock;
	uint16_t num_block = 0, num_data = 0;
	uint32_t flash_memory[256] = {0};
	uint32_t *ptr = &flash_memory[1];
	uint8_t typeOfBlock;
	Flash_Read_Data(FLASH_USER_START_ADDR, flash_memory, 1);
	num_block = flash_memory[0] >> 16;
	num_data = flash_memory[0];
	if(num_block)
	{
		Flash_Read_Data(FLASH_USER_START_ADDR, flash_memory, num_data);
		for(i = 0;i < num_block;i++)
	  {
		  typeOfBlock = *ptr >> 16;
		  lenOfBlock = *ptr;
		  if(typeOfBlock == type)
		  {
			  for(j = 0;j < lenOfBlock;j++)
				{
					data[j] = *(++ptr);
				}
				return lenOfBlock;
		  }
		  else
		  {
			  ptr += (lenOfBlock + 1);
		  }
	  }
		return -1;
	}
	else
		return -1;
}













