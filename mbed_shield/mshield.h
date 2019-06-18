#ifndef MBEDSHIELD_H__
#define MBEDSHIELD_H__

#include "mshield_config.h"
#include <stdint.h>
#include "mshield_lcd.h"
#include "nrf_drv_twi.h"

#if defined(NRF52832_XXAA) 
#define MBEDSHIELD_PIN_LED_RED          16
#define MBEDSHIELD_PIN_LED_GREEN        20
#define MBEDSHIELD_PIN_LED_BLUE         19

#define MBEDSHIELD_PIN_BUZZER           17

#define MBEDSHIELD_PIN_JOY_UP           28
#define MBEDSHIELD_PIN_JOY_DOWN         29
#define MBEDSHIELD_PIN_JOY_LEFT         30
#define MBEDSHIELD_PIN_JOY_RIGHT        31
#define MBEDSHIELD_PIN_JOY_CENTER       15

#define MBEDSHIELD_PIN_SCK              25
#define MBEDSHIELD_PIN_MOSI             23
#define MBEDSHIELD_PIN_MISO             0xFF
#define MBEDSHIELD_PIN_CSN              22

#define MBEDSHIELD_PIN_SCL              27
#define MBEDSHIELD_PIN_SDA              26

#define LCD_PIN_A0	                18
#define LCD_PIN_RESET	                24

#define MBEDSHIELD_PIN_POT_LEFT         3
#define MBEDSHIELD_PIN_POT_RIGHT        4
#define MBEDSHIELD_ANA_POT_LEFT         NRF_ADC_CONFIG_INPUT_2
#define MBEDSHIELD_ANA_POT_RIGHT        NRF_ADC_CONFIG_INPUT_3

#elif defined(NRF52840_XXAA)
#define MBEDSHIELD_PIN_LED_RED          32+6
#define MBEDSHIELD_PIN_LED_GREEN        32+11
#define MBEDSHIELD_PIN_LED_BLUE         32+10

#define MBEDSHIELD_PIN_BUZZER           32+7

#define MBEDSHIELD_PIN_JOY_UP           28
#define MBEDSHIELD_PIN_JOY_DOWN         29
#define MBEDSHIELD_PIN_JOY_LEFT         30
#define MBEDSHIELD_PIN_JOY_RIGHT        31
#define MBEDSHIELD_PIN_JOY_CENTER       32+5

#define MBEDSHIELD_PIN_SCK              32+15
#define MBEDSHIELD_PIN_MOSI             32+13
#define MBEDSHIELD_PIN_MISO             0xFF
#define MBEDSHIELD_PIN_CSN              32+12

#define MBEDSHIELD_PIN_SCL              27
#define MBEDSHIELD_PIN_SDA              26

#define LCD_PIN_A0	                32+8
#define LCD_PIN_RESET	                32+14

#define MBEDSHIELD_PIN_POT_LEFT         3
#define MBEDSHIELD_PIN_POT_RIGHT        4
#define MBEDSHIELD_ANA_POT_LEFT         NRF_ADC_CONFIG_INPUT_2
#define MBEDSHIELD_ANA_POT_RIGHT        NRF_ADC_CONFIG_INPUT_3
#else 
#error Chip type not supported, or define is missing in project settings
#endif

#define MBEDSHIELD_PWM_BUF_SIZE         32
#define MBEDSHIELD_PWM_RELOAD           10000
#define COLOR_CHANNEL_R(a)              (((a) >> 16) & 0x000000FF)
#define COLOR_CHANNEL_G(a)              (((a) >> 8) & 0x000000FF)
#define COLOR_CHANNEL_B(a)              (((a) >> 0) & 0x000000FF)

#define MBEDSHIELD_JOYSTICK_UP          0x01
#define MBEDSHIELD_JOYSTICK_DOWN        0x02
#define MBEDSHIELD_JOYSTICK_LEFT        0x04
#define MBEDSHIELD_JOYSTICK_RIGHT       0x08
#define MBEDSHIELD_JOYSTICK_CENTER      0x10

#define MBEDSHIELD_LM75_TWI_ADDR        0x48
#define MBEDSHIELD_MMA7660_TWI_ADDR     0x4C

#define MBEDSHIELD_COLOR_BLACK          0x00000000
#define MBEDSHIELD_COLOR_RED            0x00FF0000
#define MBEDSHIELD_COLOR_GREEN          0x0000FF00
#define MBEDSHIELD_COLOR_YELLOW         0x00FFFF00
#define MBEDSHIELD_COLOR_BLUE           0x000000FF
#define MBEDSHIELD_COLOR_PURPLE         0x00FF00FF
#define MBEDSHIELD_COLOR_INDIGO         0x00FFFF00
#define MBEDSHIELD_COLOR_WHITE          0x00FFFFFF

typedef enum {MBEDSHIELD_AXIS_X, MBEDSHIELD_AXIS_Y, MBEDSHIELD_AXIS_Z} mbedshield_axis_t;
typedef enum {MBEDSHIELD_POT_LEFT, MBEDSHIELD_POT_RIGHT} mbedshield_pot_t;

uint32_t mshield_init(void);

#if (MBEDSHIELD_ENABLE_LCD == 1)
void mshield_lcd_print(char *text, uint8_t x, uint8_t y);
void mshield_lcd_clear(mbedshield_lcd_line_t line);
#endif

#if (MBEDSHIELD_ENABLE_LED == 1)
void mshield_led_init(void);
void mshield_led_set_color(uint32_t led_color);
void mshield_led_set_color_rgb(uint8_t, uint8_t g, uint8_t b);
void mshield_led_set_color_hsv(float h, float s, float v);
void mshield_led_set_blink(uint32_t color1, uint32_t color2, uint32_t blink_speed_ms, uint32_t blink_delay_ms);
void mshield_led_off(void);
#endif

#if (MBEDSHIELD_ENABLE_JOYSTICK == 1)
uint32_t mshield_joystick_read(void);
#endif

#if (MBEDSHIELD_ENABLE_POT == 1)   
void mshield_pot_init(void);
uint32_t mshield_pot_read(uint32_t pot_num);
float mshield_pot_read_f(uint32_t pot_num);
#endif

#if (MBEDSHIELD_ENABLE_TEMP == 1 || MBEDSHIELD_ENABLE_ACC == 1)
uint32_t mshield_temp_acc_init(void);
#endif

#if (MBEDSHIELD_ENABLE_TEMP == 1)     
int32_t mshield_temp_read(void);
float mshield_temp_read_f(void);
#endif

#if (MBEDSHIELD_ENABLE_ACC == 1)
void mshield_acc_init(void);
int8_t mshield_acc_read(mbedshield_axis_t axis);
void mshield_acc_read_all(int8_t *data_ptr);
#endif

#if (MBEDSHIELD_ENABLE_BUZZER == 1)
void mshield_buzzer_set_frequency(uint32_t frequency);
void mshield_buzzer_off(void);
#endif

#endif
