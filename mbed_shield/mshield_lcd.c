/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
* @defgroup mbed_shield_lcd lcd.c
* @{
* @ingroup mbed_shield_lcd
*
* @brief LCD driver for mbed shield.
*
* This file contains the source code LCD driver that is on mbed application shield.
*
*/

#include "mshield.h"
#include "font8x8.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "bsp.h"

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(MBEDSHIELD_SPI_INDEX);
static volatile bool m_spi_busy = false;

static void spi_master_tx(uint8_t *data, uint32_t size)
{
    nrf_drv_spi_transfer(&m_spi_master_0, data, size, 0, 0);
}    

void lcd_cmd(uint8_t cmd)
{
	nrf_gpio_pin_clear(LCD_PIN_A0);
    spi_master_tx(&cmd, 1);
}

void lcd_data(uint8_t data)
{
	nrf_gpio_pin_set(LCD_PIN_A0);
    spi_master_tx(&data, 1);
}

void lcd_data_buf(uint8_t *databuf, uint32_t size)
{
    nrf_gpio_pin_set(LCD_PIN_A0);
    spi_master_tx(databuf, size);
}

uint32_t lcd_configure(void)
{ 
    uint32_t err_code;
    const uint8_t setup[] = {0xAE, 0xA2, 0xA0, 0xC8, 0x22, 0x2F, 0x40, 0xAF, 0x81, 0x17, 0xA6};
    uint8_t cnt = 0;

    nrf_gpio_pin_dir_set(LCD_PIN_A0, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(LCD_PIN_RESET, NRF_GPIO_PIN_DIR_OUTPUT);

    nrf_gpio_pin_clear(LCD_PIN_A0);
    nrf_gpio_pin_clear(LCD_PIN_RESET);
    nrf_delay_us(50);
    nrf_gpio_pin_set(LCD_PIN_RESET);
   
    nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
    config.frequency = NRF_DRV_SPI_FREQ_4M;
    config.mode      = NRF_DRV_SPI_MODE_0;
    config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    config.mosi_pin  = MBEDSHIELD_PIN_MOSI;
    config.sck_pin   = MBEDSHIELD_PIN_SCK;
    config.ss_pin    = MBEDSHIELD_PIN_CSN;
    err_code = nrf_drv_spi_init(&m_spi_master_0, &config, NULL, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    nrf_delay_us(5000);
    for(cnt=0;cnt<sizeof(setup);cnt++)
    {
        lcd_cmd(setup[cnt]);
    }
    return 0;
}

void lcd_page(uint8_t page)
{
	lcd_cmd(0x00);
	lcd_cmd(0x10);
	lcd_cmd(0xB0 + page);
}

static uint8_t move_bits(uint8_t data, uint8_t pos1, uint8_t pos2)
{
	uint8_t result = data & (1<<pos1);
	
	if(pos1>=pos2)
		result >>= pos1-pos2;
	else
		result <<= pos2-pos1;
	
	return(result);
}

void lcd_from_buffer(uint8_t *buffer, uint32_t size)
{
    uint32_t cnt=0, idx;
    uint8_t data, bit;
	
	while(size--)
	{
		if( (cnt % 128) == 0)
        {
			lcd_page(3 - (cnt / 128));
		}
        bit = 7 - (cnt & 0x07);
		idx = (cnt & 0x180) | ( (cnt & 0x7F) >> 3);
		data = move_bits( buffer[idx], bit, 7) | move_bits( buffer[idx+16], bit, 6 ) | move_bits( buffer[idx+32], bit, 5) | move_bits(buffer[idx+48], bit, 4) 
          | move_bits( buffer[idx+64], bit, 3) | move_bits(buffer[idx+80], bit, 2)   | move_bits( buffer[idx+96], bit, 1) | move_bits(buffer[idx+112], bit, 0);
        lcd_data(data^0xFF);
		cnt++;
	}
}

void lcd_print(uint8_t *text)
{
	uint32_t  chr, len = 0;
	uint8_t const * fnt;
    uint8_t char_buf[8];
	int32_t line, row;
	
	while( (*text != 0) & (len<32) )
	{
		chr = (uint8_t)*text;
		if( (chr>=' ') && (chr<128) )
		{
			chr -= ' ';
			fnt = FONT8x8[chr];

            for(line=0;line < 8;line++)
            {
                char_buf[line] = 0;
                for(row = 0; row < 8; row++)
                {
                    if(fnt[row] & (0x80 >> line))
                    {
                        char_buf[line] |= (1 << row);
                    }
                }
            }
            lcd_data_buf(char_buf, 8);
			len++;
			text++;
		}
	}
}

void lcd_at(uint8_t x, uint8_t y)
{
	if(y<4)
		lcd_page(y);
  if(x<128)
	{
		lcd_cmd(x & 0x0F);
		lcd_cmd( 0x10 | (x >> 4));
	}
}

void lcd_cls(mbedshield_lcd_line_t line_num)
{
    uint32_t inc = 16;
    static uint8_t buf[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t cnt;
	
    for(int line = 0; line < 4; line++)
    {
        if(line_num == MBEDSHIELD_LCD_LINE_ALL || line_num == line)
        {
            lcd_at(0, line);
            cnt = 0;
            while((cnt += inc) < 140)
                lcd_data_buf(buf, inc);   
        }   
    }
}

/** @} */
