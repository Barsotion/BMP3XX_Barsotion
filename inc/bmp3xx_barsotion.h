#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <math.h>

#include "bmp3xx_barsotion_regconst.h"
#include "bmp3xx_barsotion_regmap.h"
#include "bmp3xx_barsotion_const.h"
#include "bmp3xx_barsotion_types.h"

int bmp3xx_get_chip_id(struct bmp3xx_desc *desc, uint8_t *id);
int bmp3xx_get_fatal_err_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_cmd_err_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_conf_err_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_cmd_rdy_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_drdy_press_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_drdy_temp_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_por_detected_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_fifo_wm_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_fifo_full_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_get_drdy_status(struct bmp3xx_desc *desc, bool *status);
int bmp3xx_set_fifo_config(struct bmp3xx_desc *desc, const struct bmp3xx_fifo_cfg *cfg);
int bmp3xx_get_fifo_config(struct bmp3xx_desc *desc, struct bmp3xx_fifo_cfg *cfg);
int bmp3xx_set_fifo_subsampling(struct bmp3xx_desc *desc, uint8_t subsampling);
int bmp3xx_get_fifo_subsampling(struct bmp3xx_desc *desc, uint8_t *subsampling);
int bmp3xx_set_data_select(struct bmp3xx_desc *desc, enum BMP3XX_DATA data_sel);
int bmp3xx_get_data_select(struct bmp3xx_desc *desc, enum BMP3XX_DATA *data_sel);
int bmp3xx_set_int_output_drive(struct bmp3xx_desc *desc, enum BMP3XX_INT_O od);
int bmp3xx_get_int_output_drive(struct bmp3xx_desc *desc, enum BMP3XX_INT_O *od);
int bmp3xx_set_int_level(struct bmp3xx_desc *desc, enum BMP3XX_INT_LVL lvl);
int bmp3xx_get_int_level(struct bmp3xx_desc *desc, enum BMP3XX_INT_LVL *lvl);
int bmp3xx_set_int_latch_en(struct bmp3xx_desc *desc, bool latch_en);
int bmp3xx_get_int_latch_en(struct bmp3xx_desc *desc, bool *latch_en);
int bmp3xx_set_int_fwtm_en(struct bmp3xx_desc *desc, bool fwtm_en);
int bmp3xx_get_int_fwtm_en(struct bmp3xx_desc *desc, bool *fwtm_en);
int bmp3xx_set_int_ffull_en(struct bmp3xx_desc *desc, bool ffull_en);
int bmp3xx_get_int_ffull_en(struct bmp3xx_desc *desc, bool *ffull_en);
int bmp3xx_set_int_drdy_en(struct bmp3xx_desc *desc, bool drdy_en);
int bmp3xx_get_int_drdy_en(struct bmp3xx_desc *desc, bool *drdy_en);
int bmp3xx_set_int_config(struct bmp3xx_desc *desc, const struct bmp3xx_int_cfg *cfg);
int bmp3xx_get_int_config(struct bmp3xx_desc *desc, struct bmp3xx_int_cfg *cfg);
int bmp3xx_set_spi_mode(struct bmp3xx_desc *desc, enum BMP3XX_SPI_MODE mode);
int bmp3xx_get_spi_mode(struct bmp3xx_desc *desc, enum BMP3XX_SPI_MODE *mode);
int bmp3xx_set_i2c_wdt_en(struct bmp3xx_desc *desc, bool en);
int bmp3xx_get_i2c_wdt_en(struct bmp3xx_desc *desc, bool *en);
int bmp3xx_set_i2c_wdt_timeout(struct bmp3xx_desc *desc, enum BMP3XX_I2C_WDT timeout);
int bmp3xx_get_i2c_wdt_timeout(struct bmp3xx_desc *desc, enum BMP3XX_I2C_WDT *timeout);
int bmp3xx_set_press_en(struct bmp3xx_desc *desc, bool en);
int bmp3xx_get_press_en(struct bmp3xx_desc *desc, bool *en);
int bmp3xx_set_temp_en(struct bmp3xx_desc *desc, bool en);
int bmp3xx_get_temp_en(struct bmp3xx_desc *desc, bool *en);
int bmp3xx_set_mode(struct bmp3xx_desc *desc, enum BMP3XX_MODE mode);
int bmp3xx_get_mode(struct bmp3xx_desc *desc, enum BMP3XX_MODE *mode);
int bmp3xx_set_press_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS ovs);
int bmp3xx_get_press_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS *ovs);
int bmp3xx_set_temp_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS ovs);
int bmp3xx_get_temp_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS *ovs);
int bmp3xx_set_odr(struct bmp3xx_desc *desc, enum BMP3XX_ODR odr);
int bmp3xx_get_odr(struct bmp3xx_desc *desc, enum BMP3XX_ODR *odr);
int bmp3xx_set_iir_filt_coef(struct bmp3xx_desc *desc, enum BMP3XX_IIR_FILT_COEF coef);
int bmp3xx_get_iir_filt_coef(struct bmp3xx_desc *desc, enum BMP3XX_IIR_FILT_COEF *coef);
int bmp3xx_send_flush_fifo_cmd(struct bmp3xx_desc *desc);
int bmp3xx_send_softreset_cmd(struct bmp3xx_desc *desc);
int bmp3xx_read_press(struct bmp3xx_desc *desc);
int bmp3xx_read_temp(struct bmp3xx_desc *desc);
int bmp3xx_read_press_temp(struct bmp3xx_desc *desc);
void bmp3xx_calc_temp(struct bmp3xx_desc *desc);
void bmp3xx_calc_press(struct bmp3xx_desc *desc);
uint32_t bmp3xx_get_raw_temp(struct bmp3xx_desc *desc);
uint32_t bmp3xx_get_raw_press(struct bmp3xx_desc *desc);
float bmp3xx_get_temp(struct bmp3xx_desc *desc);
float bmp3xx_get_press(struct bmp3xx_desc *desc);
int bmp3xx_init(struct bmp3xx_desc *desc, const struct bmp3xx_cfg *cfg);

#if defined(__cplusplus)
}
#endif

