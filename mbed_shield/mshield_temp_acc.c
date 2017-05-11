#include "mshield.h"

#if (MBEDSHIELD_ENABLE_TEMP == 1 || MBEDSHIELD_ENABLE_ACC == 1)
#include "nrf_drv_twi.h"
nrf_drv_twi_t twi = NRF_DRV_TWI_INSTANCE(MBEDSHIELD_TWI_INDEX);

uint32_t mshield_temp_acc_init(void)
{
    nrf_drv_twi_config_t twi_config;
    twi_config.scl = MBEDSHIELD_PIN_SCL;
    twi_config.sda = MBEDSHIELD_PIN_SDA;
    twi_config.frequency = NRF_TWI_FREQ_100K;
    twi_config.interrupt_priority = 3;
    twi_config.clear_bus_init = false;     
    twi_config.hold_bus_uninit = false;  
    uint32_t err_code = nrf_drv_twi_init(&twi, &twi_config, NULL, NULL);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    nrf_drv_twi_enable(&twi);
    return 0;
}
#endif

#if (MBEDSHIELD_ENABLE_TEMP == 1)

int32_t mshield_temp_read(void)
{
    uint8_t buf[2] = {0, 0};
    uint32_t result = 0;

    // read 2 bytes and finish transaction
    nrf_drv_twi_rx(&twi, MBEDSHIELD_LM75_TWI_ADDR, buf, 2);
    
    result = buf[0]; // set MSB
    result <<= 8;
    result |= buf[1]; // set LSB
    if(result & 0x8000) result = (result >> 5) | 0xFFFFF800;
    else result >>= 5;
    return result;
}

float mshield_temp_read_f(void)
{
    return (float)mshield_temp_read() / 8.0f;
}

#endif

#if (MBEDSHIELD_ENABLE_ACC == 1)

void mshield_acc_init(void)
{
    uint8_t acc_init_data[] = {0x07, 0x01};

    // switch accelerometer into active mode
    nrf_drv_twi_tx(&twi, MBEDSHIELD_MMA7660_TWI_ADDR, acc_init_data, 2, false);
}

uint8_t acc_read_register(uint8_t reg)
{
    uint8_t data = 0;
    
    if(nrf_drv_twi_tx(&twi, MBEDSHIELD_MMA7660_TWI_ADDR, &reg, 1, true) == NRF_SUCCESS)
    {
        // read 1 byte and finish transaction
        nrf_drv_twi_rx(&twi, MBEDSHIELD_MMA7660_TWI_ADDR, &data, 1);
    }
    return(data);
}

int8_t mshield_acc_read(mbedshield_axis_t axis)
{
    uint8_t data;

    do{
        data = acc_read_register(axis);
    }while(data & 0x40); // repeat read if alert bit is set
    data |= (data & 0x20) << 1;
    data |= (data & 0x20) << 2; // copy sign bit

    return( (int8_t)data); // return value as signed
}

void mshield_acc_read_all(int8_t *data_ptr)
{
    data_ptr[0] = mshield_acc_read(MBEDSHIELD_AXIS_X);
    data_ptr[1] = mshield_acc_read(MBEDSHIELD_AXIS_Y);
    data_ptr[2] = mshield_acc_read(MBEDSHIELD_AXIS_Z);
}

#endif
