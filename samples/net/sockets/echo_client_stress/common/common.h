#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

extern uint8_t rand_char(void);
extern void rand_string_no_efs(uint8_t *data, uint32_t len);
extern void rand_bytes(uint8_t *data, uint32_t len);
extern uint32_t rand32(void);

#endif /* __COMMON_H__ */
