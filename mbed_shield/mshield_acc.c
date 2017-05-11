#include "mshield.h"

#if (MBEDSHIELD_ENABLE_ACC == 1)

#include "nrf_drv_twi.h"

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
        nrf_drv_twi_rx(&twi, MBEDSHIELD_MMA7660_TWI_ADDR, &data, 1, false);
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
