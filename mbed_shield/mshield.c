#include "mshield.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"
#if (MBEDSHIELD_ENABLE_LCD == 1)
#include "nrf_drv_spi.h"
#endif

#if (MBEDSHIELD_ENABLE_BUZZER == 1)
#define MBEDSHIELD_BUZZER_NUM_NOTES  100
static uint32_t mshield_buzzer_note_table[MBEDSHIELD_BUZZER_NUM_NOTES];
#endif

uint32_t mshield_init(void)
{
    uint32_t err_code = 0;
    
#if (MBEDSHIELD_ENABLE_LCD == 1)
    lcd_configure();
#endif

#if (MBEDSHIELD_ENABLE_LED == 1)
    nrf_gpio_cfg_output(MBEDSHIELD_PIN_LED_RED);
    nrf_gpio_cfg_output(MBEDSHIELD_PIN_LED_GREEN);
    nrf_gpio_cfg_output(MBEDSHIELD_PIN_LED_BLUE);

    mshield_led_init();
#endif

#if (MBEDSHIELD_ENABLE_JOYSTICK == 1)
    nrf_gpio_cfg_input(MBEDSHIELD_PIN_JOY_UP, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(MBEDSHIELD_PIN_JOY_DOWN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(MBEDSHIELD_PIN_JOY_LEFT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(MBEDSHIELD_PIN_JOY_RIGHT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(MBEDSHIELD_PIN_JOY_CENTER, NRF_GPIO_PIN_NOPULL);                                    
#endif

#if (MBEDSHIELD_ENABLE_POT == 1) 
    mshield_pot_init();
#endif

#if (MBEDSHIELD_ENABLE_TEMP == 1 || MBEDSHIELD_ENABLE_ACC == 1)
    mshield_temp_acc_init();
#endif

#if (MBEDSHIELD_ENABLE_BUZZER == 1)
    nrf_pwm_config_t pwm_config = PWM_DEFAULT_CONFIG;
    pwm_config.mode = PWM_MODE_BUZZER_255;
    pwm_config.num_channels = 1;
    pwm_config.gpio_num[0] = MBEDSHIELD_PIN_BUZZER;
    nrf_pwm_init(&pwm_config);
    
    float note_frequency = 100.0f;
    for(int i = 0; i < MBEDSHIELD_BUZZER_NUM_NOTES; i++)
    {
        mshield_buzzer_note_table[i] = (uint32_t)(16000000.0f / note_frequency);
        note_frequency *= 1.41f;
    }
#endif
    return err_code;
}

void mshield_lcd_print(char *text, uint8_t x, uint8_t y)
{
    lcd_at(x, y);
    lcd_print((uint8_t*)text);
}

void mshield_lcd_clear(mbedshield_lcd_line_t line)
{
    lcd_cls(line);
}

uint32_t mshield_joystick_read(void)
{
    uint32_t retval = 0;
    if(nrf_gpio_pin_read(MBEDSHIELD_PIN_JOY_UP) == 1) retval |= MBEDSHIELD_JOYSTICK_UP;
    if(nrf_gpio_pin_read(MBEDSHIELD_PIN_JOY_DOWN) == 1) retval |= MBEDSHIELD_JOYSTICK_DOWN;
    if(nrf_gpio_pin_read(MBEDSHIELD_PIN_JOY_LEFT) == 1) retval |= MBEDSHIELD_JOYSTICK_LEFT;
    if(nrf_gpio_pin_read(MBEDSHIELD_PIN_JOY_RIGHT) == 1) retval |= MBEDSHIELD_JOYSTICK_RIGHT;
    if(nrf_gpio_pin_read(MBEDSHIELD_PIN_JOY_CENTER) == 1) retval |= MBEDSHIELD_JOYSTICK_CENTER;
    return retval;
}

#if (MBEDSHIELD_ENABLE_BUZZER == 1)
void mshield_buzzer_set_frequency(uint32_t frequency)
{
    uint32_t reloadvalue = 16000000 / frequency / 2;
    if(reloadvalue > 0xFFFF) reloadvalue = 0xFFFF;

    nrf_pwm_set_value(0, reloadvalue / 2);
        nrf_pwm_set_max_value(reloadvalue);
}

void mshield_buzzer_off(void)
{
    nrf_pwm_set_value(0, 0);
}
#endif
