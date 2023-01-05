#ifndef _HAL_FLASH_H_
#define _HAL_FLASH_H_

#include "main.h"

#define FLASH_USER_START_ADDR                           0x0800F810
//#define FLASH_USER_START_ADDR                           0x0800FC00


typedef enum 
{
  STT_PLAY_T,
	PIN_CONTROL_T,
	IS_STANDBY_T,
	FULL_RECORD,
	TIME_RECORD_1,
	TIME_RECORD_2,
	TIME_RECORD_3,
	TIME_RECORD_4,
	TIME_RECORD_5,
	TEST,
} block_type_t;


typedef struct block_data
{
    uint16_t len;
    uint8_t type;
    uint32_t * data;
} block_data_t;

uint32_t init_flash_memory(void);
int write_flash(uint32_t * data, uint8_t type, uint16_t len);
int read_flash(uint32_t * data, uint8_t type);
void resest_flash (void);
void read_variable_from_flash(void);

#endif
