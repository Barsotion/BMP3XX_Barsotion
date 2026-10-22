#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

int bmp3xx_read_register(uint8_t address, uint8_t *value, unsigned len);
int bmp3xx_write_register(uint8_t address, const uint8_t *value, unsigned len);

#if defined(__cplusplus)
}
#endif
