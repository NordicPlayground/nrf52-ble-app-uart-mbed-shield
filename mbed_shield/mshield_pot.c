#include "mshield.h"

#if (MBEDSHIELD_ENABLE_POT == 1)
#include "nrf_drv_saadc.h"

void nrf_drv_saadc_event_handler(nrf_drv_saadc_evt_t const * p_event)
{
 
}

void mshield_pot_init(void)
{
    nrf_drv_saadc_init(0, nrf_drv_saadc_event_handler);
	
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
    channel_config.gain = NRF_SAADC_GAIN1_4;
    channel_config.reference = NRF_SAADC_REFERENCE_VDD4;
    nrf_drv_saadc_channel_init(0, &channel_config);
    
    nrf_saadc_channel_config_t channel_config2 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);
    channel_config2.gain = NRF_SAADC_GAIN1_4;
    channel_config2.reference = NRF_SAADC_REFERENCE_VDD4;
    nrf_drv_saadc_channel_init(1, &channel_config2);
}
    
uint32_t mshield_pot_read(uint32_t pot_num)
{
    nrf_saadc_value_t return_value;
    nrf_drv_saadc_sample_convert(pot_num, &return_value);
    return (uint32_t)return_value;
}

float mshield_pot_read_f(uint32_t pot_num)
{
    nrf_saadc_value_t return_value;
    nrf_drv_saadc_sample_convert(pot_num, &return_value);
    return (float)return_value / 1024.0f;
}

#endif
