//file handle_interupt.h
//DangLHb

#ifndef _HAL_HANDLE_INTERUPT_H
#define _HAL_HANDLE_INTERUPT_H

#include "main.h"


typedef enum 
{
	NO_EVENT = 0,
	RECORD_EVENT,
	BACK_EVENT,
	PLAY_EVENT,
	IR_EVENT,
	TIMER_INTERUPT,
	SLEEP_EVENT
}event;
//enum event event_interupt;

void handle_event(uint8_t event);
void Handle_Record_Button_Event(void);
void Handle_Play_Button_Event(void);
void Handle_Back_Button_Event(void);
void Enter_Standby_Mode(void);
void Handle_Timer_Interupt(void);



#endif