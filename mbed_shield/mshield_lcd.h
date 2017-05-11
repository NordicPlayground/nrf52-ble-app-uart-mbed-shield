#ifndef __MSHIELD_LCD_H
#define __MSHIELD_LCD_H

#include <stdint.h>

typedef enum {MBEDSHIELD_LCD_LINE_1 = 0x00, MBEDSHIELD_LCD_LINE_2 = 0x01, MBEDSHIELD_LCD_LINE_3 = 0x02, MBEDSHIELD_LCD_LINE_4 = 0x03, MBEDSHIELD_LCD_LINE_ALL = 0xFF} mbedshield_lcd_line_t;

void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
uint32_t lcd_configure(void);
void lcd_page(uint8_t page);
void lcd_from_buffer(uint8_t *buffer, uint32_t size);
void lcd_print(uint8_t *text);
void lcd_at(uint8_t x, uint8_t y);
void lcd_cls(mbedshield_lcd_line_t line_mask);

#endif
