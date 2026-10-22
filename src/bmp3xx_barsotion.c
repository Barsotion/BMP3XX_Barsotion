#include "bmp3xx_barsotion.h"
#include "bmp3xx_barsotion_regtypes.h"
#include "bmp3xx_barsotion_wheels.h"

#define _ROE(res)	do {\
	if (res != 0)\
	{\
		return 1;\
	}\
} while(0);

#define readRegister		desc->read_register
#define writeRegister		desc->write_register

static int _read_calibration_data(struct bmp3xx_desc *desc)
{
	uint8_t data[21];
	_ROE(readRegister(BMP3XX_T1_L, data, 21));
	desc->_par.t1 = (float)(((uint16_t)data[1] << 8) | ((uint16_t)data[0]));
	desc->_par.t2 = (float)(((uint16_t)data[3] << 8) | ((uint16_t)data[2]));
	desc->_par.t3 = (float)((int8_t)data[4]);
	desc->_par.p1 = (float)(int16_t)(((int16_t)data[6] << 8) | ((int16_t)data[5]));
	desc->_par.p2 = (float)(int16_t)(((int16_t)data[8] << 8) | ((int16_t)data[7]));
	desc->_par.p3 = (float)((uint8_t)data[9]);
	desc->_par.p4 = (float)((int8_t)data[10]);
	desc->_par.p5 = (float)(((uint16_t)data[12] << 8) | ((uint16_t)data[11]));
	desc->_par.p6 = (float)(((uint16_t)data[14] << 8) | ((uint16_t)data[13]));
	desc->_par.p7 = (float)((int8_t)data[15]);
	desc->_par.p8 = (float)((int8_t)data[16]);
	desc->_par.p9 = (float)(int16_t)(((int16_t)data[18] << 8) | ((int16_t)data[17]));
	desc->_par.p10 = (float)((int8_t)data[19]);
	desc->_par.p11 = (float)((int8_t)data[20]);
	desc->_par.t1 = desc->_par.t1 * powf(2, 8);
	desc->_par.t2 = desc->_par.t2 * powf(2, -30);
	desc->_par.t3 = desc->_par.t3 * powf(2, -48);
	desc->_par.p1 = (desc->_par.p1 - 16384.0) * powf(2, -20);
	desc->_par.p2 = (desc->_par.p2 - 16384.0) * powf(2, -29);
	desc->_par.p3 = desc->_par.p3 * powf(2, -32);
	desc->_par.p4 = desc->_par.p4 * powf(2, -37);
	desc->_par.p5 = desc->_par.p5 * powf(2, 3);
	desc->_par.p6 = desc->_par.p6 * powf(2, -6);
	desc->_par.p7 = desc->_par.p7 * powf(2, -8);
	desc->_par.p8 = desc->_par.p8 * powf(2, -15);
	desc->_par.p9 = desc->_par.p9 * powf(2, -48);
	desc->_par.p10 = desc->_par.p10 * powf(2, -48);
	desc->_par.p11 = desc->_par.p11 * powf(2, -65);
	return 0;
}

static void _compensate_temp(struct bmp3xx_desc *desc)
{
	float data1;
	float data2;
	data1 = (float)(desc->uncomp_temp - desc->_par.t1);
	data2 = (float)(data1 * desc->_par.t2);
	// Update the compensated temperature in calib structure since
	// this is needed for pressure calculation
	desc->temp = data2 + (data1 * data1) * desc->_par.t3;
}

static void _compensate_press(struct bmp3xx_desc *desc)
{
	// Temporary variables used for compensationstruct bmp3xx_desc *desc
	float data1;
	float data2;
	float data3;
	float data4;
	float out1;
	float out2;
	float T_lin2 = desc->temp * desc->temp;
	float T_lin3 = T_lin2 * desc->temp;
	float F_uncomp_press = (float)(desc->uncomp_press & 0xFFFFFFUL);
	// Calibration data
	data1 = desc->_par.p6 * desc->temp;
	data2 = desc->_par.p7 * T_lin2;
	data3 = desc->_par.p8 * T_lin3;
	out1 = desc->_par.p5 + data1 + data2 + data3;
	data1 = desc->_par.p2 * desc->temp;
	data2 = desc->_par.p3 * T_lin2;
	data3 = desc->_par.p4 * T_lin3;
	out2 = F_uncomp_press * (desc->_par.p1 + data1 + data2 + data3);
	data1 = F_uncomp_press * F_uncomp_press;
	data2 = desc->_par.p9 + desc->_par.p10 * desc->temp;
	data3 = data1 * data2;
	data4 = data3 + (data1 * F_uncomp_press) * desc->_par.p11;
	desc->press = out1 + out2 + data4;
}

int bmp3xx_get_chip_id(struct bmp3xx_desc *desc, uint8_t *id)
{
	if (desc == NULL) return 1;
	uint8_t chip_id = 0;
	_ROE(readRegister(BMP3XX_CHIP_ID, &chip_id, 1));
	*id = chip_id;
	return 0;
}

int bmp3xx_get_fatal_err_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_err_reg_t err_reg;
	_ROE(readRegister(BMP3XX_ERR_REG, (uint8_t*)&err_reg, 1));
	*status = (bool)err_reg.fatal_err;
	return 0;
}

int bmp3xx_get_cmd_err_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_err_reg_t err_reg;
	_ROE(readRegister(BMP3XX_ERR_REG, (uint8_t*)&err_reg, 1));
	*status = (bool)err_reg.cmd_err;
	return 0;
}

int bmp3xx_get_conf_err_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_err_reg_t err_reg;
	_ROE(readRegister(BMP3XX_ERR_REG, (uint8_t*)&err_reg, 1));
	*status = (bool)err_reg.conf_err;
	return 0;
}

int bmp3xx_get_cmd_rdy_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_status_t st;
	_ROE(readRegister(BMP3XX_STATUS, (uint8_t*)&st, 1));
	*status = (bool)st.cmd_rdy;
	return 0;
}

int bmp3xx_get_drdy_press_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_status_t st;
	_ROE(readRegister(BMP3XX_STATUS, (uint8_t*)&st, 1));
	*status = (bool)st.drdy_press;
	return 0;
}

int bmp3xx_get_drdy_temp_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_status_t st;
	_ROE(readRegister(BMP3XX_STATUS, (uint8_t*)&st, 1));
	*status = (bool)st.drdy_temp;
	return 0;
}

int bmp3xx_get_por_detected_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_event_t event;
	_ROE(readRegister(BMP3XX_EVENT, (uint8_t*)&event, 1));
	*status = (bool)event.por_detected;
	return 0;
}

int bmp3xx_get_fifo_wm_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_status_t int_status;
	_ROE(readRegister(BMP3XX_INT_STATUS, (uint8_t*)&int_status, 1));
	*status = (bool)int_status.fwm_int;
	return 0;
}

int bmp3xx_get_fifo_full_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_status_t int_status;
	_ROE(readRegister(BMP3XX_INT_STATUS, (uint8_t*)&int_status, 1));
	*status = (bool)int_status.ffull_int;
	return 0;
}

int bmp3xx_get_drdy_status(struct bmp3xx_desc *desc, bool *status)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_status_t int_status;
	_ROE(readRegister(BMP3XX_INT_STATUS, (uint8_t*)&int_status, 1));
	*status = (bool)int_status.drdy;
	return 0;
}

int bmp3xx_set_fifo_config(struct bmp3xx_desc *desc, const struct bmp3xx_fifo_cfg *cfg)
{
	if (desc == NULL || cfg == NULL) return 1;
	_bmp3xx_barsotion_fifo_config_1_t fifo_config_1;
	_ROE(readRegister(BMP3XX_FIFO_CONFIG_1, (uint8_t*)&fifo_config_1, 1));
	fifo_config_1.fifo_stop_on_full = cfg->stop_on_full_en & 1;
	fifo_config_1.fifo_time_en = cfg->time_en & 1;
	fifo_config_1.fifo_press_en = cfg->press_en & 1;
	fifo_config_1.fifo_temp_en = cfg->temp_en & 1;
	_ROE(writeRegister(BMP3XX_FIFO_CONFIG_1, (uint8_t*)&fifo_config_1, 1));
	return 0;
}

int bmp3xx_get_fifo_config(struct bmp3xx_desc *desc, struct bmp3xx_fifo_cfg *cfg)
{
	if (desc == NULL || cfg == NULL) return 1;
	_bmp3xx_barsotion_fifo_config_1_t fifo_config_1;
	_ROE(readRegister(BMP3XX_FIFO_CONFIG_1, (uint8_t*)&fifo_config_1, 1));
	cfg->stop_on_full_en = fifo_config_1.fifo_stop_on_full;
	cfg->time_en = fifo_config_1.fifo_time_en;
	cfg->press_en = fifo_config_1.fifo_press_en;
	cfg->temp_en = fifo_config_1.fifo_temp_en;
	return 0;
}

int bmp3xx_set_fifo_subsampling(struct bmp3xx_desc *desc, uint8_t subsampling)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_fifo_config_2_t fifo_config_2;
	_ROE(readRegister(BMP3XX_FIFO_CONFIG_2, (uint8_t*)&fifo_config_2, 1));
	fifo_config_2.fifo_subsampling = subsampling & 0x7;
	_ROE(writeRegister(BMP3XX_FIFO_CONFIG_2, (uint8_t*)&fifo_config_2, 1));
	return 0;
}

int bmp3xx_get_fifo_subsampling(struct bmp3xx_desc *desc, uint8_t *subsampling)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_fifo_config_2_t fifo_config_2;
	_ROE(readRegister(BMP3XX_FIFO_CONFIG_2, (uint8_t*)&fifo_config_2, 1));
	*subsampling = fifo_config_2.fifo_subsampling;
	return 0;
}

int bmp3xx_set_data_select(struct bmp3xx_desc *desc, enum BMP3XX_DATA data_sel)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_fifo_config_2_t fifo_config_2;
	_ROE(readRegister(BMP3XX_FIFO_CONFIG_2, (uint8_t*)&fifo_config_2, 1));
	fifo_config_2.data_select = data_sel;
	_ROE(writeRegister(BMP3XX_FIFO_CONFIG_2, (uint8_t*)&fifo_config_2, 1));
	return 0;
}

int bmp3xx_get_data_select(struct bmp3xx_desc *desc, enum BMP3XX_DATA *data_sel)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_fifo_config_2_t fifo_config_2;
	_ROE(readRegister(BMP3XX_FIFO_CONFIG_2, (uint8_t*)&fifo_config_2, 1));
	*data_sel = fifo_config_2.data_select;
	return 0;
}

int bmp3xx_set_int_output_drive(struct bmp3xx_desc *desc, enum BMP3XX_INT_O od)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.int_od = od;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_output_drive(struct bmp3xx_desc *desc, enum BMP3XX_INT_O *od)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	*od = int_ctrl.int_od;
	return 0;
}

int bmp3xx_set_int_level(struct bmp3xx_desc *desc, enum BMP3XX_INT_LVL lvl)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.int_level = lvl;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_level(struct bmp3xx_desc *desc, enum BMP3XX_INT_LVL *lvl)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	*lvl = int_ctrl.int_level;
	return 0;
}

int bmp3xx_set_int_latch_en(struct bmp3xx_desc *desc, bool latch_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.int_latch_en = latch_en & 1;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_latch_en(struct bmp3xx_desc *desc, bool *latch_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	*latch_en = int_ctrl.int_latch_en;
	return 0;
}

int bmp3xx_set_int_fwtm_en(struct bmp3xx_desc *desc, bool fwtm_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.fwtm_en = fwtm_en & 1;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_fwtm_en(struct bmp3xx_desc *desc, bool *fwtm_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	*fwtm_en = int_ctrl.fwtm_en;
	return 0;
}

int bmp3xx_set_int_ffull_en(struct bmp3xx_desc *desc, bool ffull_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.ffull_en = ffull_en & 1;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_ffull_en(struct bmp3xx_desc *desc, bool *ffull_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	*ffull_en = int_ctrl.ffull_en;
	return 0;
}

int bmp3xx_set_int_drdy_en(struct bmp3xx_desc *desc, bool drdy_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.drdy_en = drdy_en & 1;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_drdy_en(struct bmp3xx_desc *desc, bool *drdy_en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	*drdy_en = int_ctrl.drdy_en;
	return 0;
}

int bmp3xx_set_int_config(struct bmp3xx_desc *desc, const struct bmp3xx_int_cfg *cfg)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	int_ctrl.int_od = cfg->drive;
	int_ctrl.int_level = cfg->level;
	int_ctrl.int_latch_en = cfg->latch_en;
	int_ctrl.fwtm_en = cfg->fwtm_en;
	int_ctrl.ffull_en = cfg->ffull_en;
	int_ctrl.drdy_en = cfg->drdy_en;
	_ROE(writeRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	return 0;
}

int bmp3xx_get_int_config(struct bmp3xx_desc *desc, struct bmp3xx_int_cfg *cfg)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_int_ctrl_t int_ctrl;
	_ROE(readRegister(BMP3XX_INT_CTRL, (uint8_t*)&int_ctrl, 1));
	cfg->drive = int_ctrl.int_od;
	cfg->level = int_ctrl.int_level;
	cfg->latch_en = int_ctrl.int_latch_en;
	cfg->fwtm_en = int_ctrl.fwtm_en;
	cfg->ffull_en = int_ctrl.ffull_en;
	cfg->drdy_en = int_ctrl.drdy_en;
	return 0;
}

int bmp3xx_set_spi_mode(struct bmp3xx_desc *desc, enum BMP3XX_SPI_MODE mode)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_if_conf_t if_conf;
	_ROE(readRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	if_conf.spi_mode = mode;
	_ROE(writeRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	return 0;
}

int bmp3xx_get_spi_mode(struct bmp3xx_desc *desc, enum BMP3XX_SPI_MODE *mode)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_if_conf_t if_conf;
	_ROE(readRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	*mode = if_conf.spi_mode;
	return 0;
}

int bmp3xx_set_i2c_wdt_en(struct bmp3xx_desc *desc, bool en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_if_conf_t if_conf;
	_ROE(readRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	if_conf.i2c_wdt_en = (uint8_t)en;
	_ROE(writeRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	return 0;
}

int bmp3xx_get_i2c_wdt_en(struct bmp3xx_desc *desc, bool *en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_if_conf_t if_conf;
	_ROE(readRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	*en = (bool)if_conf.i2c_wdt_en;
	return 0;
}

int bmp3xx_set_i2c_wdt_timeout(struct bmp3xx_desc *desc, enum BMP3XX_I2C_WDT timeout)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_if_conf_t if_conf;
	_ROE(readRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	if_conf.i2c_wdt_sel = timeout;
	_ROE(writeRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	return 0;
}

int bmp3xx_get_i2c_wdt_timeout(struct bmp3xx_desc *desc, enum BMP3XX_I2C_WDT *timeout)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_if_conf_t if_conf;
	_ROE(readRegister(BMP3XX_IF_CONF, (uint8_t*)&if_conf, 1));
	*timeout = if_conf.i2c_wdt_sel;
	return 0;
}

int bmp3xx_set_press_en(struct bmp3xx_desc *desc, bool en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_pwr_ctrl_t pwr_ctrl;
	_ROE(readRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	pwr_ctrl.press_en = (uint8_t)en;
	_ROE(writeRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	return 0;
}

int bmp3xx_get_press_en(struct bmp3xx_desc *desc, bool *en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_pwr_ctrl_t pwr_ctrl;
	_ROE(readRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	*en = (bool)pwr_ctrl.press_en;
	return 0;
}

int bmp3xx_set_temp_en(struct bmp3xx_desc *desc, bool en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_pwr_ctrl_t pwr_ctrl;
	_ROE(readRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	pwr_ctrl.temp_en = (uint8_t)en;
	_ROE(writeRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	return 0;
}

int bmp3xx_get_temp_en(struct bmp3xx_desc *desc, bool *en)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_pwr_ctrl_t pwr_ctrl;
	_ROE(readRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	*en = (bool)pwr_ctrl.temp_en;
	return 0;
}

int bmp3xx_set_mode(struct bmp3xx_desc *desc, enum BMP3XX_MODE mode)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_pwr_ctrl_t pwr_ctrl;
	_ROE(readRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	pwr_ctrl.mode = mode;
	_ROE(writeRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	return 0;
}

int bmp3xx_get_mode(struct bmp3xx_desc *desc, enum BMP3XX_MODE *mode)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_pwr_ctrl_t pwr_ctrl;
	_ROE(readRegister(BMP3XX_PWR_CTRL, (uint8_t*)&pwr_ctrl, 1));
	*mode = pwr_ctrl.mode;
	return 0;
}

int bmp3xx_set_press_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS ovs)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_osr_t osr;
	_ROE(readRegister(BMP3XX_OSR, (uint8_t*)&osr, 1));
	osr.ovs_p = ovs;
	_ROE(writeRegister(BMP3XX_OSR, (uint8_t*)&osr, 1));
	return 0;
}

int bmp3xx_get_press_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS *ovs)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_osr_t osr;
	_ROE(readRegister(BMP3XX_OSR, (uint8_t*)&osr, 1));
	*ovs = osr.ovs_p;
	return 0;
}

int bmp3xx_set_temp_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS ovs)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_osr_t osr;
	_ROE(readRegister(BMP3XX_OSR, (uint8_t*)&osr, 1));
	osr.ovs_t = ovs;
	_ROE(writeRegister(BMP3XX_OSR, (uint8_t*)&osr, 1));
	return 0;
}

int bmp3xx_get_temp_ovs(struct bmp3xx_desc *desc, enum BMP3XX_OVS *ovs)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_osr_t osr;
	_ROE(readRegister(BMP3XX_OSR, (uint8_t*)&osr, 1));
	*ovs = osr.ovs_t;
	return 0;
}

int bmp3xx_set_odr(struct bmp3xx_desc *desc, enum BMP3XX_ODR odr)
{
	if (desc == NULL) return 1;
	uint8_t odr_reg;
	_ROE(readRegister(BMP3XX_ODR, &odr_reg, 1));
	odr_reg = odr;
	_ROE(writeRegister(BMP3XX_ODR, &odr_reg, 1));
	return 0;
}

int bmp3xx_get_odr(struct bmp3xx_desc *desc, enum BMP3XX_ODR *odr)
{
	if (desc == NULL) return 1;
	uint8_t odr_reg;
	_ROE(readRegister(BMP3XX_ODR, &odr_reg, 1));
	*odr = odr_reg;
	return 0;
}

int bmp3xx_set_iir_filt_coef(struct bmp3xx_desc *desc, enum BMP3XX_IIR_FILT_COEF coef)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_config_t config;
	_ROE(readRegister(BMP3XX_CONFIG, (uint8_t*)&config, 1));
	config.iir_filter = coef;
	_ROE(writeRegister(BMP3XX_CONFIG, (uint8_t*)&config, 1));
	return 0;
}

int bmp3xx_get_iir_filt_coef(struct bmp3xx_desc *desc, enum BMP3XX_IIR_FILT_COEF *coef)
{
	if (desc == NULL) return 1;
	_bmp3xx_barsotion_config_t config;
	_ROE(readRegister(BMP3XX_CONFIG, (uint8_t*)&config, 1));
	*coef = config.iir_filter;
	return 0;
}

int bmp3xx_send_flush_fifo_cmd(struct bmp3xx_desc *desc)
{
	if (desc == NULL) return 1;
	uint8_t cmd = 0xB0;
	_ROE(writeRegister(BMP3XX_CMD, &cmd, 1));
	return 0;
}

int bmp3xx_send_softreset_cmd(struct bmp3xx_desc *desc)
{
	if (desc == NULL) return 1;
	uint8_t cmd = 0xB6;
	_ROE(writeRegister(BMP3XX_CMD, &cmd, 1));
	return 0;
}

int bmp3xx_read_press(struct bmp3xx_desc *desc)
{
	if (desc == NULL) return 0;
	uint8_t raw[3];
	_ROE(readRegister(BMP3XX_DATA_0, raw, 3));
	desc->uncomp_press = (uint32_t)raw[2]<<16 | (uint32_t)raw[1]<<8 | raw[0];
	return 0;
}

int bmp3xx_read_temp(struct bmp3xx_desc *desc)
{
	if (desc == NULL) return 0;
	uint8_t raw[3];
	_ROE(readRegister(BMP3XX_DATA_3, raw, 3));
	desc->uncomp_temp = (uint32_t)raw[2]<<16 | (uint32_t)raw[1]<<8 | raw[0];
	return 0;
}

int bmp3xx_read_press_temp(struct bmp3xx_desc *desc)
{
	if (desc == NULL) return 0;
	uint8_t raw[6];
	_ROE(readRegister(BMP3XX_DATA_0, raw, 6));
	desc->uncomp_press = (uint32_t)raw[2]<<16 | (uint32_t)raw[1]<<8 | raw[0];
	desc->uncomp_temp = (uint32_t)raw[5]<<16 | (uint32_t)raw[4]<<8 | raw[3];
	return 0;
}

void bmp3xx_calc_temp(struct bmp3xx_desc *desc)
{
	_compensate_temp(desc);
}

void bmp3xx_calc_press(struct bmp3xx_desc *desc)
{
	_compensate_press(desc);
}

uint32_t bmp3xx_get_raw_temp(struct bmp3xx_desc *desc)
{
	return desc->uncomp_temp;
}

uint32_t bmp3xx_get_raw_press(struct bmp3xx_desc *desc)
{
	return desc->uncomp_press;
}

float bmp3xx_get_temp(struct bmp3xx_desc *desc)
{
	return desc->temp;
}

float bmp3xx_get_press(struct bmp3xx_desc *desc)
{
	return desc->press;
}

int bmp3xx_init(struct bmp3xx_desc *desc, const struct bmp3xx_cfg *cfg)
{
	if (desc == NULL) return 1;
	uint8_t id = 0;
	_ROE(bmp3xx_get_chip_id(desc, &id));
	if (id != BMP388_ID && id != BMP390_ID)
	{
		return 2;
	}
	_ROE(bmp3xx_set_fifo_config(desc, &cfg->fifo));
	_ROE(bmp3xx_set_int_config(desc, &cfg->interrupt));
	_ROE(bmp3xx_set_temp_en(desc, cfg->temp.en));
	if (cfg->temp.en)
	{
		_ROE(bmp3xx_set_temp_ovs(desc, cfg->temp.ovs));
	}
	_ROE(bmp3xx_set_press_en(desc, cfg->press.en));
	if (cfg->press.en)
	{
		_ROE(bmp3xx_set_press_ovs(desc, cfg->press.ovs));
	}
	if (cfg->iir_filt.en)
	{
		_ROE(bmp3xx_set_data_select(desc, BMP3XX_DATA_FILTERED));
		_ROE(bmp3xx_set_iir_filt_coef(desc, cfg->iir_filt.coef));
	}
	else
	{
		_ROE(bmp3xx_set_data_select(desc, BMP3XX_DATA_UNFILTERED));
	}
	_ROE(bmp3xx_set_i2c_wdt_en(desc, cfg->i2c_wdt.en));
	if (cfg->i2c_wdt.en)
	{
		_ROE(bmp3xx_set_i2c_wdt_timeout(desc, cfg->i2c_wdt.timeout));
	}
	_ROE(_read_calibration_data(desc));
	return 0;
}

