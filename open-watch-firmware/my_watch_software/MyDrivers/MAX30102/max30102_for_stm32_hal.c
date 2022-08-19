#include "max30102_for_stm32_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Built-in plotting function. Called during an interrupt to print/plot the current sample.
 * @note Override this in your main.c if you do not use printf() for printing.
 * @param ir_sample
 * @param red_sample
 */
__weak void max30102_plot(uint32_t ir_sample, uint32_t red_sample)
{
    
}

/**
 * @brief MAX30102 initiation function.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param hi2c Pointer to I2C object handle
 */
void max30102_init(max30102_t *obj, I2C_HandleTypeDef *hi2c)
{
    obj->_ui2c = hi2c;
    obj->_interrupt_flag = 0;
    memset(obj->_ir_samples, 0, MAX30102_SAMPLE_LEN_MAX * sizeof(uint32_t));
    memset(obj->_red_samples, 0, MAX30102_SAMPLE_LEN_MAX * sizeof(uint32_t));
}

/**
 * @brief Write buffer of buflen bytes to a register of the MAX30102.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param reg Register address to write to.
 * @param buf Pointer containing the bytes to write.
 * @param buflen Number of bytes to write.
 */
void max30102_write(max30102_t *obj, uint8_t reg, uint8_t *buf, uint16_t buflen)
{
    uint8_t *payload = (uint8_t *)malloc((buflen + 1) * sizeof(uint8_t));
    *payload = reg;
    if (buf != NULL && buflen != 0)
        memcpy(payload + 1, buf, buflen);
    HAL_I2C_Master_Transmit(obj->_ui2c, MAX30102_I2C_ADDR << 1, payload, buflen + 1, MAX30102_I2C_TIMEOUT);
    free(payload);
}

/**
 * @brief Read buflen bytes from a register of the MAX30102 and store to buffer.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param reg Register address to read from.
 * @param buf Pointer to the array to write to.
 * @param buflen Number of bytes to read.
 */
void max30102_read(max30102_t *obj, uint8_t reg, uint8_t *buf, uint16_t buflen)
{
    uint8_t reg_addr = reg;
    HAL_I2C_Master_Transmit(obj->_ui2c, MAX30102_I2C_ADDR << 1, &reg_addr, 1, MAX30102_I2C_TIMEOUT);
    HAL_I2C_Master_Receive(obj->_ui2c, MAX30102_I2C_ADDR << 1, buf, buflen, MAX30102_I2C_TIMEOUT);
}

/**
 * @brief Reset the sensor.
 *
 * @param obj Pointer to max30102_t object instance.
 */
void max30102_reset(max30102_t *obj)
{
    uint8_t val = 0x40;
    max30102_write(obj, MAX30102_MODE_CONFIG, &val, 1);
}

/**
 * @brief Enable A_FULL interrupt.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param enable Enable (1) or disable (0).
 */
void max30102_set_a_full(max30102_t *obj, uint8_t enable)
{
    uint8_t reg = 0;
    max30102_read(obj, MAX30102_INTERRUPT_ENABLE_1, &reg, 1);
    reg &= ~(0x01 << MAX30102_INTERRUPT_A_FULL);
    reg |= ((enable & 0x01) << MAX30102_INTERRUPT_A_FULL);
    max30102_write(obj, MAX30102_INTERRUPT_ENABLE_1, &reg, 1);
}

/**
 * @brief Enable PPG_RDY interrupt.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param enable Enable (1) or disable (0).
 */
void max30102_set_ppg_rdy(max30102_t *obj, uint8_t enable)
{
    uint8_t reg = 0;
    max30102_read(obj, MAX30102_INTERRUPT_ENABLE_1, &reg, 1);
    reg &= ~(0x01 << MAX30102_INTERRUPT_PPG_RDY);
    reg |= ((enable & 0x01) << MAX30102_INTERRUPT_PPG_RDY);
    max30102_write(obj, MAX30102_INTERRUPT_ENABLE_1, &reg, 1);
}

/**
 * @brief Enable ALC_OVF interrupt.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param enable Enable (1) or disable (0).
 */
void max30102_set_alc_ovf(max30102_t *obj, uint8_t enable)
{
    uint8_t reg = 0;
    max30102_read(obj, MAX30102_INTERRUPT_ENABLE_1, &reg, 1);
    reg &= ~(0x01 << MAX30102_INTERRUPT_ALC_OVF);
    reg |= ((enable & 0x01) << MAX30102_INTERRUPT_ALC_OVF);
    max30102_write(obj, MAX30102_INTERRUPT_ENABLE_1, &reg, 1);
}

/**
 * @brief Enable DIE_TEMP_RDY interrupt.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param enable Enable (1) or disable (0).
 */
void max30102_set_die_temp_rdy(max30102_t *obj, uint8_t enable)
{
    uint8_t reg = (enable & 0x01) << MAX30102_INTERRUPT_DIE_TEMP_RDY;
    max30102_write(obj, MAX30102_INTERRUPT_ENABLE_2, &reg, 1);
}

/**
 * @brief Enable temperature measurement.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param enable Enable (1) or disable (0).
 */
void max30102_set_die_temp_en(max30102_t *obj, uint8_t enable)
{
    uint8_t reg = (enable & 0x01) << MAX30102_DIE_TEMP_EN;
    max30102_write(obj, MAX30102_DIE_TEMP_CONFIG, &reg, 1);
}

/**
 * @brief Set interrupt flag on interrupt. To be called in the corresponding external interrupt handler.
 *
 * @param obj Pointer to max30102_t object instance.
 */
void max30102_on_interrupt(max30102_t *obj)
{
    obj->_interrupt_flag = 1;
}

/**
 * @brief Check whether the interrupt flag is active.
 *
 * @param obj Pointer to max30102_t object instance.
 * @return uint8_t Active (1) or inactive (0).
 */
uint8_t max30102_has_interrupt(max30102_t *obj)
{
    return obj->_interrupt_flag;
}

/**
 * @brief Read interrupt status registers (0x00 and 0x01) and perform corresponding tasks.
 *
 * @param obj Pointer to max30102_t object instance.
 */
void max30102_interrupt_handler(max30102_t *obj)
{
    uint8_t reg[2] = {0x00};
    // Interrupt flag in registers 0x00 and 0x01 are cleared on read
    max30102_read(obj, MAX30102_INTERRUPT_STATUS_1, reg, 2);

    if ((reg[0] >> MAX30102_INTERRUPT_A_FULL) & 0x01)
    {
        // FIFO almost full
        max30102_read_fifo(obj);
    }

    if ((reg[0] >> MAX30102_INTERRUPT_PPG_RDY) & 0x01)
    {
        // New FIFO data ready
    }

    if ((reg[0] >> MAX30102_INTERRUPT_ALC_OVF) & 0x01)
    {
        // Ambient light overflow
    }

    if ((reg[1] >> MAX30102_INTERRUPT_DIE_TEMP_RDY) & 0x01)
    {
        // Temperature data ready
        int8_t temp_int;
        uint8_t temp_frac;
        max30102_read_temp(obj, &temp_int, &temp_frac);
        // float temp = temp_int + 0.0625f * temp_frac;
    }

    // Reset interrupt flag
    obj->_interrupt_flag = 0;
}

/**
 * @brief Shutdown the sensor.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param shdn Shutdown bit.
 */
void max30102_shutdown(max30102_t *obj, uint8_t shdn)
{
    uint8_t config;
    max30102_read(obj, MAX30102_MODE_CONFIG, &config, 1);
    config = (config & 0x7f) | (shdn << MAX30102_MODE_SHDN);
    max30102_write(obj, MAX30102_MODE_CONFIG, &config, 1);
}

/**
 * @brief Set measurement mode.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param mode Measurement mode enum (max30102_mode_t).
 */
void max30102_set_mode(max30102_t *obj, max30102_mode_t mode)
{
    uint8_t config;
    max30102_read(obj, MAX30102_MODE_CONFIG, &config, 1);
    config = (config & 0xf8) | mode;
    max30102_write(obj, MAX30102_MODE_CONFIG, &config, 1);
    max30102_clear_fifo(obj);
}

/**
 * @brief Set sampling rate.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param sr Sampling rate enum (max30102_spo2_st_t).
 */
void max30102_set_sampling_rate(max30102_t *obj, max30102_sr_t sr)
{
    uint8_t config;
    max30102_read(obj, MAX30102_SPO2_CONFIG, &config, 1);
    config = (config & 0x63) << MAX30102_SPO2_SR;
    max30102_write(obj, MAX30102_SPO2_CONFIG, &config, 1);
}

/**
 * @brief Set led pulse width.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param pw Pulse width enum (max30102_led_pw_t).
 */
void max30102_set_led_pulse_width(max30102_t *obj, max30102_led_pw_t pw)
{
    uint8_t config;
    max30102_read(obj, MAX30102_SPO2_CONFIG, &config, 1);
    config = (config & 0x7c) | (pw << MAX30102_SPO2_LEW_PW);
    max30102_write(obj, MAX30102_SPO2_CONFIG, &config, 1);
}

/**
 * @brief Set ADC resolution.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param adc ADC resolution enum (max30102_adc_t).
 */
void max30102_set_adc_resolution(max30102_t *obj, max30102_adc_t adc)
{
    uint8_t config;
    max30102_read(obj, MAX30102_SPO2_CONFIG, &config, 1);
    config = (config & 0x1f) | (adc << MAX30102_SPO2_ADC_RGE);
    max30102_write(obj, MAX30102_SPO2_CONFIG, &config, 1);
}

/**
 * @brief Set LED current.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param ma LED current float (0 < ma < 51.0).
 */
void max30102_set_led_current_1(max30102_t *obj, float ma)
{
    uint8_t pa = ma / 0.2;
    max30102_write(obj, MAX30102_LED_IR_PA1, &pa, 1);
}

/**
 * @brief Set LED current.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param ma LED current float (0 < ma < 51.0).
 */
void max30102_set_led_current_2(max30102_t *obj, float ma)
{
    uint8_t pa = ma / 0.2;
    max30102_write(obj, MAX30102_LED_RED_PA2, &pa, 1);
}

/**
 * @brief Set slot mode when in multi-LED mode.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param slot1 Slot 1 mode enum (max30102_multi_led_ctrl_t).
 * @param slot2 Slot 2 mode enum (max30102_multi_led_ctrl_t).
 */
void max30102_set_multi_led_slot_1_2(max30102_t *obj, max30102_multi_led_ctrl_t slot1, max30102_multi_led_ctrl_t slot2)
{
    uint8_t val = 0;
    val |= ((slot1 << MAX30102_MULTI_LED_CTRL_SLOT1) | (slot2 << MAX30102_MULTI_LED_CTRL_SLOT2));
    max30102_write(obj, MAX30102_MULTI_LED_CTRL_1, &val, 1);
}

/**
 * @brief Set slot mode when in multi-LED mode.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param slot1 Slot 1 mode enum (max30102_multi_led_ctrl_t).
 * @param slot2 Slot 2 mode enum (max30102_multi_led_ctrl_t).
 */
void max30102_set_multi_led_slot_3_4(max30102_t *obj, max30102_multi_led_ctrl_t slot3, max30102_multi_led_ctrl_t slot4)
{
    uint8_t val = 0;
    val |= ((slot3 << MAX30102_MULTI_LED_CTRL_SLOT3) | (slot4 << MAX30102_MULTI_LED_CTRL_SLOT4));
    max30102_write(obj, MAX30102_MULTI_LED_CTRL_2, &val, 1);
}

/**
 * @brief
 *
 * @param obj Pointer to max30102_t object instance.
 * @param smp_ave
 * @param roll_over_en Roll over enabled(1) or disabled(0).
 * @param fifo_a_full Number of empty samples when A_FULL interrupt issued (0 < fifo_a_full < 15).
 */
void max30102_set_fifo_config(max30102_t *obj, max30102_smp_ave_t smp_ave, uint8_t roll_over_en, uint8_t fifo_a_full)
{
    uint8_t config = 0x00;
    config |= smp_ave << MAX30102_FIFO_CONFIG_SMP_AVE;
    config |= ((roll_over_en & 0x01) << MAX30102_FIFO_CONFIG_ROLL_OVER_EN);
    config |= ((fifo_a_full & 0x0f) << MAX30102_FIFO_CONFIG_FIFO_A_FULL);
    max30102_write(obj, MAX30102_FIFO_CONFIG, &config, 1);
}

/**
 * @brief Clear all FIFO pointers in the sensor.
 *
 * @param obj Pointer to max30102_t object instance.
 */
void max30102_clear_fifo(max30102_t *obj)
{
    uint8_t val = 0x00;
    max30102_write(obj, MAX30102_FIFO_WR_PTR, &val, 3);
    max30102_write(obj, MAX30102_FIFO_RD_PTR, &val, 3);
    max30102_write(obj, MAX30102_OVF_COUNTER, &val, 3);
}

/**
 * @brief Read FIFO content and store to buffer in max30102_t object instance.
 *
 * @param obj Pointer to max30102_t object instance.
 */
void max30102_read_fifo(max30102_t *obj)
{
    // First transaction: Get the FIFO_WR_PTR
    uint8_t wr_ptr = 0, rd_ptr = 0;
    max30102_read(obj, MAX30102_FIFO_WR_PTR, &wr_ptr, 1);
    max30102_read(obj, MAX30102_FIFO_RD_PTR, &rd_ptr, 1);

    int8_t num_samples;

    num_samples = (int8_t)wr_ptr - (int8_t)rd_ptr;
    if (num_samples < 1)
    {
        num_samples += 32;
    }

    // Second transaction: Read NUM_SAMPLES_TO_READ samples from the FIFO
    for (int8_t i = 0; i < num_samples; i++)
    {
        uint8_t sample[6];
        max30102_read(obj, MAX30102_FIFO_DATA, sample, 6);
        uint32_t ir_sample = ((uint32_t)(sample[0] << 16) | (uint32_t)(sample[1] << 8) | (uint32_t)(sample[3])) & 0x3ffff;
        uint32_t red_sample = ((uint32_t)(sample[3] << 16) | (uint32_t)(sample[4] << 8) | (uint32_t)(sample[5])) & 0x3ffff;
        obj->_ir_samples[i] = ir_sample;
        obj->_red_samples[i] = red_sample;
        max30102_plot(ir_sample, red_sample);
    }
}

/**
 * @brief Read die temperature.
 *
 * @param obj Pointer to max30102_t object instance.
 * @param temp_int Pointer to store the integer part of temperature. Stored in 2's complement format.
 * @param temp_frac Pointer to store the fractional part of temperature. Increments of 0.0625 deg C.
 */

void max30102_read_temp(max30102_t *obj, int8_t *temp_int, uint8_t *temp_frac)
{
    max30102_read(obj, MAX30102_DIE_TINT, (uint8_t *)temp_int, 1);
    max30102_read(obj, MAX30102_DIE_TFRAC, temp_frac, 1);
}

#ifdef __cplusplus
}
#endif
