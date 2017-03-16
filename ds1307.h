// RTC DS1307
//
// AVR Toolchain
//
// file: ds1307.h
//
// 05:16 16-01-2017



#include <stdint.h>



//I2C address
#define DS1307 0b11010000
#define WRITE  0b00000000
#define READ   0b00000001



//-----------------------------------------------------------------------------
void _rtc_conv(uint8_t *d, uint8_t m)  //d-data //m-mask
	{
	*d=(((((*d & m)>>4)<<1)+(((*d & m)>>4)<<3))+(*d & 0x0F));
	}


//-----------------------------------------------------------------------------
void rtc_read(uint8_t *data)
	{
	i2c_start();
	i2c_send_byte(DS1307 | WRITE);

	i2c_send_byte(0x00);

	i2c_start();

	i2c_send_byte(DS1307 | READ);

	for(uint8_t i=0; i<7; i++) data[i] = i2c_read_byte(ACK);

	data[7] = i2c_read_byte(NACK);

	i2c_stop();

	_rtc_conv(&data[0], 0x70);
	_rtc_conv(&data[1], 0x70);
	_rtc_conv(&data[2], 0x30);
	_rtc_conv(&data[4], 0x30);
	_rtc_conv(&data[5], 0x10);
	_rtc_conv(&data[6], 0xF0);
	}


//-----------------------------------------------------------------------------
void rtc_write_reg(uint8_t addr, uint8_t val)
	{
	i2c_start();
	i2c_send_byte(DS1307 | WRITE);

	i2c_send_byte(addr);
	i2c_send_byte(val);

	i2c_stop();
	}


//-----------------------------------------------------------------------------
uint8_t rtc_read_reg(uint8_t addr)
	{
	i2c_start();
	i2c_send_byte(DS1307 | WRITE);

	i2c_send_byte(addr);

	i2c_start();
	i2c_send_byte(DS1307 | READ);

	uint8_t data = i2c_read_byte(NACK);

	i2c_stop();

	return data;
	}


//-----------------------------------------------------------------------------
void rtc_init(void)
	{
	uint8_t temp=0;

	temp = rtc_read_reg(0x00);
	rtc_write_reg(0x00, temp & 0b01111111); //rtc start

	temp = rtc_read_reg(0x02);
	rtc_write_reg(0x02, temp & 0b10111111); //set 24-hour mode

	rtc_write_reg(0x07, 0b10000000);
	}

