#include "common.h"

#ifdef __ZEPHYR__
#include <zephyr.h>
#include <arch/arm/cortex_m/cmsis.h>
uint32_t get_system_seed(void)
{
    return SysTick->VAL;
}
#else
uint32_t get_system_seed(void)
{
    return rand();
}
#endif

static uint32_t randseed = 12345;

void srand(uint32_t seed)
{
    randseed = seed;
}

void rand_reseed(uint32_t seed)
{
    randseed ^= seed;
}

static volatile uint32_t _cnt = 0;
uint32_t rand32(void)
{
    _cnt++;
    if (17 == _cnt) {
        rand_reseed(get_system_seed());
        _cnt = 0;
    }
    return (uint32_t)(randseed = randseed * 1664525UL + 1013904223UL);
}

void rand_bytes(uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint32_t rand_u32 = 0;

    for (i = 0; i < (len / 4); i++) {
        rand_u32    = rand32();
        data[i + 0] = ((rand_u32 >> 0) & 0xff);
        data[i + 1] = ((rand_u32 >> 8) & 0xff);
        data[i + 2] = ((rand_u32 >> 16) & 0xff);
        data[i + 3] = ((rand_u32 >> 24) & 0xff);
    }

    for (i = len / 4; i < len; i++) {
        data[i] = ((rand32()) & 0xff);
    }
    return;
}

uint8_t _valid_chars[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '1', '2', '3', '4', '5', '6',

    '7', '8', '9', '0', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', '@', '#',
};

#define _CHAR_ARRAY_SZ    (sizeof(_valid_chars))
uint8_t rand_char(void)
{
    uint32_t i = 0;
    uint32_t tmp_data = 0;
    do {
        tmp_data = rand32();
        tmp_data += tmp_data >> 8;
        tmp_data ^= ((tmp_data >> 16) & 0xff);
        i = (tmp_data & (_CHAR_ARRAY_SZ - 1));
    } while (i >= _CHAR_ARRAY_SZ);

    return _valid_chars[i];
}
/* exclude \0 */
void rand_string_no_efs(uint8_t *data, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        data[i] = rand_char();
    }
}
