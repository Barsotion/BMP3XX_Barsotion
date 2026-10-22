#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

typedef struct bmp3xx_fifo_cfg
{
	uint8_t stop_on_full_en;
	uint8_t time_en;
	uint8_t press_en;
	uint8_t temp_en;
} _bmp3xx_barsotion_fifo_cfg_t;

typedef struct bmp3xx_int_cfg
{
	enum BMP3XX_INT_O drive;
	enum BMP3XX_INT_LVL level;
	bool latch_en;
	bool fwtm_en;
	bool ffull_en;
	bool drdy_en;
} _bmo3xx_barsotion_int_cfg_t;

typedef struct bmp3xx_cfg
{
	struct bmp3xx_fifo_cfg fifo;
	struct bmp3xx_int_cfg interrupt;
	struct
	{
		bool en;
		enum BMP3XX_OVS ovs;
	} press;
	struct
	{
		bool en;
		enum BMP3XX_OVS ovs;
	} temp;
	struct
	{
		bool en;
		enum BMP3XX_IIR_FILT_COEF coef;
	} iir_filt;
	struct
	{
		bool en;
		enum BMP3XX_I2C_WDT timeout;
	} i2c_wdt;
	enum BMP3XX_SPI_MODE spi_mode;
	int (*read_register) (uint8_t, uint8_t*, unsigned);
	int (*write_register) (uint8_t, const uint8_t*, unsigned);
} _bmp3xx_barsotion_cfg_t;

typedef struct bmp3xx_desc
{
	struct
	{
		float t1, t2, t3;
        float p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;
	} _par;
	int (*read_register) (uint8_t, uint8_t*, unsigned);
	int (*write_register) (uint8_t, const uint8_t*, unsigned);
	uint8_t address;
	uint32_t uncomp_press;
	uint32_t uncomp_temp;
	float press;
	float temp;
} _bmp3xx_barsotion_desc_t;

#if defined(__cplusplus)
}
#endif
