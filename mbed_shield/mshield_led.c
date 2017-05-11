#include "mshield.h"
#include "nrf.h"
#include "nrf_gpio.h"

#if (MBEDSHIELD_ENABLE_LED == 1)

static uint16_t m_pwm_led_buf1[32*4];
static uint16_t m_pwm_led_buf2[32*4];

typedef struct {
    float r;       // percent
    float g;       // percent
    float b;       // percent
} rgb;

typedef struct {
    float h;       // angle in degrees
    float s;       // percent
    float v;       // percent
} hsv;

void hsv_to_rgb(hsv in, rgb *out)
{
    float       hh, p, q, t, ff;
    uint32_t    i;

    if(in.s <= 0.0f) 
    {      
        out->r = in.v;
        out->g = in.v;
        out->b = in.v;
        return;
    }
    hh = in.h;
    if(hh >= 360.0f) 
    {
        hh = 0.0f;
    }
    hh /= 60.0f;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0f - in.s);
    q = in.v * (1.0f - (in.s * ff));
    t = in.v * (1.0f - (in.s * (1.0f - ff)));

    switch(i) 
    {
        case 0:
            out->r = in.v;
            out->g = t;
            out->b = p;
            break;
        case 1:
            out->r = q;
            out->g = in.v;
            out->b = p;
            break;
        case 2:
            out->r = p;
            out->g = in.v;
            out->b = t;
            break;
        case 3:
            out->r = p;
            out->g = q;
            out->b = in.v;
            break;
        case 4:
            out->r = t;
            out->g = p;
            out->b = in.v;
            break;
        case 5:
        default:
            out->r = in.v;
            out->g = p;
            out->b = q;
            break;
    }   
}

void mshield_led_init()
{
    //GPIO mapping
    NRF_PWM0->PSEL.OUT[0] = (MBEDSHIELD_PIN_LED_RED << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
    NRF_PWM0->PSEL.OUT[1] = (MBEDSHIELD_PIN_LED_GREEN << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
    NRF_PWM0->PSEL.OUT[2] = (MBEDSHIELD_PIN_LED_BLUE << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
    NRF_PWM0->PSEL.OUT[3] = (0 << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Disconnected << PWM_PSEL_OUT_CONNECT_Pos);

    //Enable PWM instance
    NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);

    //Waveform counter configuration
    //  Resulting PWM pulses: 10000.0 ns period (100.000 Hz)
    NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
    NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_16 << PWM_PRESCALER_PRESCALER_Pos);
    NRF_PWM0->COUNTERTOP = (10000 << PWM_COUNTERTOP_COUNTERTOP_Pos);

    //Decoder configuration
    NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);

    //Sequencer configuration
    NRF_PWM0->LOOP = (0 << PWM_LOOP_CNT_Pos);
    NRF_PWM0->SEQ[0].PTR = ((uint32_t)(m_pwm_led_buf1) << PWM_SEQ_PTR_PTR_Pos);
    NRF_PWM0->SEQ[0].CNT = ((sizeof(m_pwm_led_buf1) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);
    NRF_PWM0->SEQ[0].REFRESH = 0;
    NRF_PWM0->SEQ[0].ENDDELAY = 0;
    NRF_PWM0->SEQ[1].PTR = ((uint32_t)(m_pwm_led_buf2) << PWM_SEQ_PTR_PTR_Pos);
    NRF_PWM0->SEQ[1].CNT = ((sizeof(m_pwm_led_buf2) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);
    NRF_PWM0->SEQ[1].REFRESH = 0;
    NRF_PWM0->SEQ[1].ENDDELAY = 1000;

    //Shortcuts
    NRF_PWM0->SHORTS = ( (PWM_SHORTS_SEQEND0_STOP_Disabled << PWM_SHORTS_SEQEND0_STOP_Pos) |
                         (PWM_SHORTS_SEQEND1_STOP_Disabled << PWM_SHORTS_SEQEND1_STOP_Pos) |
                         (PWM_SHORTS_LOOPSDONE_SEQSTART0_Disabled << PWM_SHORTS_LOOPSDONE_SEQSTART0_Pos) |
                         (PWM_SHORTS_LOOPSDONE_SEQSTART1_Enabled << PWM_SHORTS_LOOPSDONE_SEQSTART1_Pos) |
                         (PWM_SHORTS_LOOPSDONE_STOP_Disabled << PWM_SHORTS_LOOPSDONE_STOP_Pos) );
}

void mshield_led_set_color_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    m_pwm_led_buf1[0] = (uint16_t)((float)r * (float)MBEDSHIELD_PWM_RELOAD / 255.0f * 0.5f);
    m_pwm_led_buf1[1] = (uint16_t)((float)g * (float)MBEDSHIELD_PWM_RELOAD / 255.0f * 0.9f);
    m_pwm_led_buf1[2] = (uint16_t)((float)b * (float)MBEDSHIELD_PWM_RELOAD / 255.0f * 1.0f);
    
    NRF_PWM0->SEQ[0].CNT = 4;
    NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_NextStep << PWM_DECODER_MODE_Pos);
    
    NRF_PWM0->TASKS_SEQSTART[0] = 1;
    NRF_PWM0->TASKS_NEXTSTEP = 1;    
}

void mshield_led_set_color_hsv(float h, float s, float v)
{
    hsv in_color = {h * 360.0f, s, v};
    rgb out_color;
    hsv_to_rgb(in_color, &out_color);
    mshield_led_set_color_rgb((uint8_t)(out_color.r * 255.0f), (uint8_t)(out_color.g * 255.0f), (uint8_t)(out_color.b * 255.0f));
}

void mshield_led_set_color(uint32_t led_color)
{
    mshield_led_set_color_rgb(COLOR_CHANNEL_R(led_color), COLOR_CHANNEL_G(led_color), COLOR_CHANNEL_B(led_color));
}

void mshield_led_set_blink(uint32_t color1, uint32_t color2, uint32_t blink_speed_ms, uint32_t blink_delay_ms)
{
    // NOT IMPLEMENTED
}

void mshield_led_off(void)
{
    mshield_led_set_color(0x00000000);
}

#endif
