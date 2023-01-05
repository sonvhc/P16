#ifndef _HAL_IR_REMOTE_H_
#define _HAL_IR_REMOTE_H_

#include <stdint.h>
#include "main.h"

uint8_t encode_ir(uint8_t state);
uint32_t get_result_ir(void);



#endif

