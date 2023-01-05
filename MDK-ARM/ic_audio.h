#ifndef _HAL_IC_AUDIO_H_
#define _HAL_IC_AUDIO_H_

#include "main.h"

typedef enum {M0 = 0, M1, M2, M3, M4, M5, M6, M7} pin_control_t;

typedef enum 
{
NONE =0,
RECORDING,
PLAYING,
SLEEPING	
}status_t;	

void Convert_pin_control_or_stt_play_to_pin(uint8_t pin,uint8_t setorreset);
void Start_record(void);
void Stop_record(void);
void Start_play(void);
void Stop_play(void);
void Handle_Record_Button_Event(void);
void Handle_Play_Button_Event(void);
void Handle_Back_Button_Event(void);
uint32_t stt_play_to_time_record (uint32_t stt_play_t);
void convert_pin_control_to_time_record(uint32_t pin_control_t);
#endif