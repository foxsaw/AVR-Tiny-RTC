// 1-Wire interface for DS18B20
//
// compiler: AVR Toolchain
// device:   ATmega328P @ 16MHz
//
//  ___________             _______
// |    MCU    |           |DS18B20|
// |           |---power---| +V    |
// | -[pullup]-|---data----| Data  |
// |           |        |--| GND   |
// |           |           |_______|
// |___________|
//
//
// 28-12-16
//
// file: trm.h
//


#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


//****************************** data line ***************************//
#define DAT_PORT_REG  PORTC
#define DAT_DDR_REG   DDRC
#define DAT_PIN_REG   PINC
#define DAT_PIN       1


#define DAT_INPUT   CLR_BIT(DAT_DDR_REG, DAT_PIN)
#define DAT_OUTPUT  SET_BIT(DAT_DDR_REG, DAT_PIN)
#define DAT_LOW     CLR_BIT(DAT_PORT_REG, DAT_PIN)
#define DAT_HIGH    SET_BIT(DAT_PORT_REG, DAT_PIN)

#define DAT_READ    BIT_IS_SET(DAT_PIN_REG, DAT_PIN)

#define DAT_INPUT_PULL  DAT_INPUT; DAT_HIGH
#define DAT_LOW_OUTPUT  DAT_LOW; DAT_OUTPUT


#define CMD_CONVERTTEMP 0x44
#define CMD_RSCRATCHPAD 0xbe
#define CMD_SKIPROM 0xcc



//-----------------------------------------------------------------------------
uint8_t trm_reset(void)
	{
	DAT_LOW_OUTPUT;
	_delay_us(480);

	DAT_INPUT_PULL;
	_delay_us(60);

	uint8_t i = DAT_READ;
	_delay_us(420);

	return i;  //0 = OK
	}


//-----------------------------------------------------------------------------
void trm_write_bit(uint8_t bit)
	{
	cli();  //<<< disable all interrupts <<<

	DAT_LOW_OUTPUT;
	_delay_us(1);

	if(bit) { DAT_INPUT_PULL; }

	_delay_us(60);
	DAT_INPUT_PULL;
	sei();
	}


//-----------------------------------------------------------------------------
uint8_t trm_read_bit(void)
	{
	uint8_t bit=0;

	cli();  //<<< disable all interrupts <<<

	DAT_LOW_OUTPUT;
	_delay_us(1);

	DAT_INPUT_PULL;
	_delay_us(14);

	if(DAT_READ) bit=1;

	_delay_us(45);

	sei();
	return bit;
	}


//-----------------------------------------------------------------------------
void trm_read_byte(uint8_t *b) //read byte
	{
	for(uint8_t k=0; k<8; k++) *b|=(trm_read_bit()<<k);
	}


//-----------------------------------------------------------------------------
void trm_write_byte(uint8_t byte) //send byte
	{
	for(uint8_t mask=1; mask; mask<<=1) trm_write_bit(byte&mask);
	}


//-----------------------------------------------------------------------------
void trm_get_temp(uint8_t *arr)  //array[3]: 0-"+/-", 1-"XX.x", 2-"xx.X"
	{
	uint8_t reg0 = 0;
	uint8_t reg1 = 0;

	trm_reset();
	trm_write_byte(CMD_SKIPROM);
	trm_write_byte(CMD_RSCRATCHPAD);

	trm_read_byte(&reg0);
	trm_read_byte(&reg1);

	if(reg1 & 0x80)  //if "-"
		{
		arr[0]=1;

		uint16_t tmp = (~((reg1<<8)|reg0))+1;

		reg0 = tmp;
		reg1 = tmp>>8;
		}
	else arr[0]=0;

	arr[1] = ((reg1 & 0b111)<<4)|(reg0>>4);  //integer part of temperature
	arr[2] = (((reg0 & 0b1111)<<1)+((reg0 & 0b1111)<<3))>>4; //fractional

	trm_reset();
	trm_write_byte(CMD_SKIPROM);
	trm_write_byte(CMD_CONVERTTEMP);
	}

