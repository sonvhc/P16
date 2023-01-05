//file handle_interupt.c
//DangLHb

#include "main.h"
#include "ic_audio.h"
#include "flash_memory.h"
#include "string.h"
#include "stdio.h"
#include <stdint.h>
#include "handle_interupt.h"
#include "ir_remote.h"

#define MAX_TIME_RECORD 60
#define TIME_GO_TO_STANDBY 600
#define TIME_GO_TO_SLEEP 2
uint16_t time_default = 0, time_standy = 0;

uint32_t pin_control = 0, stt_play = 0, full_record = 0;
extern event event_interupt; 
extern status_t status;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern uint8_t interupt_timer_3;


void handel_no_event(void)
{
	//char msg[59];
	//sprintf(msg, "\n time_standy = %d",time_standy );
	//HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	//HAL_Delay(1000);
	if( (status != PLAYING) && (status != RECORDING))
	{
		event_interupt = SLEEP_EVENT;
		HAL_UART_Transmit(&huart1, (uint8_t *)"Enter sleep mode                 ", 33 , HAL_MAX_DELAY);
		HAL_TIM_Base_Stop_IT(&htim2);		//stop timer 2
		HAL_Delay(50);
		HAL_TIM_Base_Start_IT(&htim3);	//start timer 3
		HAL_Delay(50);
		event_interupt = NO_EVENT;
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();
		HAL_UART_Transmit(&huart1, (uint8_t *)"Exit sleep mode                  ", 33 , HAL_MAX_DELAY);
		HAL_TIM_Base_Stop_IT(&htim3);
		HAL_Delay(50);
		HAL_TIM_Base_Start_IT(&htim2);
		//event_interupt = NO_EVENT;
		
		HAL_Delay(500);
		if(event_interupt!= NO_EVENT)
		{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Time out- Enter standby   ", 26, HAL_MAX_DELAY);
			//interupt_timer_3 = 0;
		//Enter_Standby_Mode();
		}
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t button)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)"have interrupt-event       ", 27, HAL_MAX_DELAY);
	if(event_interupt == NO_EVENT)
	{		
		switch(button)
		{
			case GPIO_PIN_12:	//BACK
				event_interupt = BACK_EVENT;
				HAL_UART_Transmit(&huart1, (uint8_t *)"interrupt-back_press = 1       ", 31, HAL_MAX_DELAY);
			break;
			
			case GPIO_PIN_15:	//PLAY
				event_interupt = PLAY_EVENT;				
				HAL_UART_Transmit(&huart1, (uint8_t *)"interrupt-play_press = 1       ", 31, HAL_MAX_DELAY);				
			break;
			
			case GPIO_PIN_11:	//RECORD
				event_interupt = RECORD_EVENT;							
				HAL_UART_Transmit(&huart1, (uint8_t *)"interrupt-record_press = 1     ", 31, HAL_MAX_DELAY);							
			break;
			
			case GPIO_PIN_0:	//IR
				if(!encode_ir(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)))
				{
					uint32_t data_ir = get_result_ir();
					HAL_UART_Transmit(&huart1, (uint8_t *)"---------------------", 21, HAL_MAX_DELAY);
					if((data_ir == 0xf7c03f) /*|| (is_button)*/)
					{
						HAL_UART_Transmit(&huart1, (uint8_t *)"get_result_ir", 14, HAL_MAX_DELAY);
						data_ir = 0;
						//	is_button = 0;
						event_interupt = IR_EVENT;
					}
				}				
			break;
				
			default:
			break;		
		}			
	}
	char msg[59];
	sprintf(msg, "\n event_interupt = %d, status =%d",event_interupt,status);
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}


//Ham xu li khi co ngat timer
void Handle_Timer_Interupt(void)
{
		HAL_UART_Transmit(&huart1, (uint8_t *)"hi", 2, HAL_MAX_DELAY);

	uint32_t time_record = stt_play_to_time_record(stt_play) ;
	if(time_default < MAX_TIME_RECORD)
		time_default  +=1;
	if(time_standy < TIME_GO_TO_STANDBY)
		time_standy +=1;
	if(time_default == MAX_TIME_RECORD && ( status == RECORDING))
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Time out- stop record     ", 26, HAL_MAX_DELAY);		
		//Stop_record();
		//event_interupt = NO_EVENT;
		event_interupt = RECORD_EVENT;
	}
	if((time_default == time_record) && (status == PLAYING))
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Time out- stop play     ", 26, HAL_MAX_DELAY);		
		//Stop_play();
		event_interupt = PLAY_EVENT;
	}
	

	//char msg[59];
	//sprintf(msg, "\n time_standy = %d, ",time_standy);
	//HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
	/*
	if(time_standy == TIME_GO_TO_STANDBY)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Time out- Enter standby   ", 26, HAL_MAX_DELAY);		
		Enter_Standby_Mode();
	}
	*/
	//event_interupt = NO_EVENT;
}


//Ham Enter Sleep mode
void StanbyMode(void)
{
	//MX_GPIO_Deinit();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWR_EnterSTANDBYMode();
}


// ham goi vao che do standby
void Enter_Standby_Mode(void)
{

	if(status == RECORDING)
		Stop_record();
	if(status == PLAYING)
		Stop_play();
	//lock_interupt = 0;
		//goi ham vao che do standy.
		__HAL_RCC_PWR_CLK_ENABLE();
   if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
	 {
		 __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU|PWR_FLAG_SB);
	 } 
	//sprintf(msg, "\nENTER standby mode");
	//HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 100);
	StanbyMode();	
	//}
}


//Ham xu li khi co su kien bam nut RECORD
void Handle_Record_Button_Event(void)
{
	if(status == PLAYING)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Record_Button_Event - enter stop play      ", 50, HAL_MAX_DELAY);
		Stop_play();
	}
	if(status != RECORDING)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Record_Button_Event - enter start record   ", 50, HAL_MAX_DELAY);
		Start_record();
		event_interupt = NO_EVENT;
	}
	else
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Record_Button_Event - enter stop record    ", 50, HAL_MAX_DELAY);
		Stop_record();
		event_interupt = NO_EVENT;			
	}
}


//Ham xu li khi co su kien bam nut Play
void Handle_Play_Button_Event(void)
{
	
		HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Play_Button_Event - stt_play     ", 40, HAL_MAX_DELAY);
		if(status == RECORDING)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Play_Button_Event - enter Stop_record      ", 50, HAL_MAX_DELAY);			
			Stop_record();
		}
		//HAL_Delay(10);
		if(stt_play == 0)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Play_Button_Event - stt_play = 0           ", 50, HAL_MAX_DELAY);
			//play_press = 0;
			time_default = 0;
			time_standy = 0;
			HAL_Delay(1000);
			event_interupt = NO_EVENT;			
			return;
		}
		if(status != PLAYING)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Play_Button_Event - enter Start_play       ", 50, HAL_MAX_DELAY);			
			Start_play();
			event_interupt = NO_EVENT;
		}			
		else
			Stop_play();
			event_interupt = NO_EVENT;
}


//Ham xu li khi co su kien bam nut Back
void Handle_Back_Button_Event(void)
{
	if(status != RECORDING)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Back_Button_Event - !is_record        ", 45, HAL_MAX_DELAY);
		if(status == PLAYING)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"Handle_Back_Button_Event - enter Stop_play        ", 50, HAL_MAX_DELAY);
			Stop_play();
		}		
		if(stt_play > 1 && full_record ==0)
			stt_play -=1;
		else if(full_record == 1)
			stt_play -=1;
		else if(stt_play == 1 && pin_control > 0 && full_record == 0)
			stt_play = pin_control; 
		
	char msg[51];
	sprintf(msg, "\n full_record = %d, stt_play =%d, pin_control = %d",full_record,stt_play,pin_control );
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		if(stt_play == 0 && full_record == 1)
			stt_play = 5;
		if(stt_play > 0)
		Start_play();
		time_default = 0;
		time_standy = 0;
		//event_interupt = 0;
		HAL_Delay(1000);		
		event_interupt = NO_EVENT;		
	} else 
			//back_press =0;
	write_flash(&stt_play, STT_PLAY_T, (sizeof(stt_play)/4));	// Luu gia tri vao flash
	HAL_Delay(500);	
	event_interupt = NO_EVENT;
}


void handle_event(event even_t)
{
	//HAL_UART_Transmit(&huart1, (uint8_t *)"handle_event", 12, HAL_MAX_DELAY);
	switch(even_t)
	{
		case NO_EVENT:
		// check status
		// enter sleep
			//HAL_UART_Transmit(&huart1, (uint8_t *)"NO_EVENT     ", 12, HAL_MAX_DELAY);
			//handel_no_event();
		
			break;
		case RECORD_EVENT:
			HAL_UART_Transmit(&huart1, (uint8_t *)"RECORD_EVENT", 12, HAL_MAX_DELAY);
			Handle_Record_Button_Event();
			break;
		case BACK_EVENT:
			HAL_UART_Transmit(&huart1, (uint8_t *)"BACK_EVENT", 10, HAL_MAX_DELAY);
			Handle_Back_Button_Event();
			break;
		case PLAY_EVENT:
			HAL_UART_Transmit(&huart1, (uint8_t *)"PLAY_EVENT", 10, HAL_MAX_DELAY);
			Handle_Play_Button_Event();
			break;
		case IR_EVENT:
			Enter_Standby_Mode();
			break;
		//case TIMER_INTERUPT:
			//Handle_Timer_Interupt();
			//break;
		default:
			break;
	}
		
}