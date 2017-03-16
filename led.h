// the 3-digit 7-segment LED display
//
// compiler: AVR Toolchain 3.4.2.939
// device:   ATmega328P @ 16MHz
//
//  _____
// |     |---------> segment a
// | MCU | *
// |     | *
// |     | *
// |     |---------> segment h
// |     |
// |     |-[100R]--> common cathode 1
// |     |-[100R]--> common cathode 2
// |_____|-[100R]--> common cathode 3
//
//
// 28-12-2016
//
// file: led.h
//



#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>



//*****************************************************************************

#define SEG_A_PIN   1
#define SEG_A_PORT  PORTB
#define SEG_A_TRIS  DDRB

#define SEG_B_PIN   0
#define SEG_B_PORT  PORTD
#define SEG_B_TRIS  DDRD

#define SEG_C_PIN   5
#define SEG_C_PORT  PORTD
#define SEG_C_TRIS  DDRD

#define SEG_D_PIN   7
#define SEG_D_PORT  PORTD
#define SEG_D_TRIS  DDRD

#define SEG_E_PIN   0
#define SEG_E_PORT  PORTB
#define SEG_E_TRIS  DDRB

#define SEG_F_PIN   3
#define SEG_F_PORT  PORTD
#define SEG_F_TRIS  DDRD

#define SEG_G_PIN   4
#define SEG_G_PORT  PORTD
#define SEG_G_TRIS  DDRD

#define SEG_H_PIN   6
#define SEG_H_PORT  PORTD
#define SEG_H_TRIS  DDRD


#define COM_1_PIN   3
#define COM_1_PORT  PORTB
#define COM_1_TRIS  DDRB

#define COM_2_PIN   4
#define COM_2_PORT  PORTB
#define COM_2_TRIS  DDRB

#define COM_3_PIN   1
#define COM_3_PORT  PORTD
#define COM_3_TRIS  DDRD

//*****************************************************************************



#define SEG_A_CLR     CLR_BIT(SEG_A_PORT, SEG_A_PIN)
#define SEG_A_SET     SET_BIT(SEG_A_PORT, SEG_A_PIN)
#define SEG_A_OUTPUT  SET_BIT(SEG_A_TRIS, SEG_A_PIN)
#define SEG_A_INPUT   CLR_BIT(SEG_A_TRIS, SEG_A_PIN)

#define SEG_B_CLR     CLR_BIT(SEG_B_PORT, SEG_B_PIN)
#define SEG_B_SET     SET_BIT(SEG_B_PORT, SEG_B_PIN)
#define SEG_B_OUTPUT  SET_BIT(SEG_B_TRIS, SEG_B_PIN)
#define SEG_B_INPUT   CLR_BIT(SEG_B_TRIS, SEG_B_PIN)

#define SEG_C_CLR     CLR_BIT(SEG_C_PORT, SEG_C_PIN)
#define SEG_C_SET     SET_BIT(SEG_C_PORT, SEG_C_PIN)
#define SEG_C_OUTPUT  SET_BIT(SEG_C_TRIS, SEG_C_PIN)
#define SEG_C_INPUT   CLR_BIT(SEG_C_TRIS, SEG_C_PIN)

#define SEG_D_CLR     CLR_BIT(SEG_D_PORT, SEG_D_PIN)
#define SEG_D_SET     SET_BIT(SEG_D_PORT, SEG_D_PIN)
#define SEG_D_OUTPUT  SET_BIT(SEG_D_TRIS, SEG_D_PIN)
#define SEG_D_INPUT   CLR_BIT(SEG_D_TRIS, SEG_D_PIN)

#define SEG_E_CLR     CLR_BIT(SEG_E_PORT, SEG_E_PIN)
#define SEG_E_SET     SET_BIT(SEG_E_PORT, SEG_E_PIN)
#define SEG_E_OUTPUT  SET_BIT(SEG_E_TRIS, SEG_E_PIN)
#define SEG_E_INPUT   CLR_BIT(SEG_E_TRIS, SEG_E_PIN)

#define SEG_F_CLR     CLR_BIT(SEG_F_PORT, SEG_F_PIN)
#define SEG_F_SET     SET_BIT(SEG_F_PORT, SEG_F_PIN)
#define SEG_F_OUTPUT  SET_BIT(SEG_F_TRIS, SEG_F_PIN)
#define SEG_F_INPUT   CLR_BIT(SEG_F_TRIS, SEG_F_PIN)

#define SEG_G_CLR     CLR_BIT(SEG_G_PORT, SEG_G_PIN)
#define SEG_G_SET     SET_BIT(SEG_G_PORT, SEG_G_PIN)
#define SEG_G_OUTPUT  SET_BIT(SEG_G_TRIS, SEG_G_PIN)
#define SEG_G_INPUT   CLR_BIT(SEG_G_TRIS, SEG_G_PIN)

#define SEG_H_CLR     CLR_BIT(SEG_H_PORT, SEG_H_PIN)
#define SEG_H_SET     SET_BIT(SEG_H_PORT, SEG_H_PIN)
#define SEG_H_OUTPUT  SET_BIT(SEG_H_TRIS, SEG_H_PIN)
#define SEG_H_INPUT   CLR_BIT(SEG_H_TRIS, SEG_H_PIN)


#define COM_1_SET     SET_BIT(COM_1_PORT, COM_1_PIN)
#define COM_1_CLR     CLR_BIT(COM_1_PORT, COM_1_PIN)
#define COM_1_OUTPUT  SET_BIT(COM_1_TRIS, COM_1_PIN)
#define COM_1_INPUT   CLR_BIT(COM_1_TRIS, COM_1_PIN)

#define COM_2_SET     SET_BIT(COM_2_PORT, COM_2_PIN)
#define COM_2_CLR     CLR_BIT(COM_2_PORT, COM_2_PIN)
#define COM_2_OUTPUT  SET_BIT(COM_2_TRIS, COM_2_PIN)
#define COM_2_INPUT   CLR_BIT(COM_2_TRIS, COM_2_PIN)

#define COM_3_SET     SET_BIT(COM_3_PORT, COM_3_PIN)
#define COM_3_CLR     CLR_BIT(COM_3_PORT, COM_3_PIN)
#define COM_3_OUTPUT  SET_BIT(COM_3_TRIS, COM_3_PIN)
#define COM_3_INPUT   CLR_BIT(COM_3_TRIS, COM_3_PIN)



char strbuff[4];  //string buffer

volatile uint8_t sbuff[] = {  //videobuffer
  0x00,
  0x00,
  0x00
};

const uint8_t scode[] PROGMEM = {  //symbols  //map: (bit7)g-f-e-d-c-b-a-h(bit0)
  0b01111110, //0  //0
  0b00001100, //1  //1
  0b10110110, //2  //2
  0b10011110, //3  //3  
  0b11001100, //4  //4
  0b11011010, //5  //5
  0b11111010, //6  //6
  0b00001110, //7  //7
  0b11111110, //8  //8
  0b11011110, //9  //9
  0b00000000, //10 //space
  0b10000000, //11 //-
  };



//---------------------------------------------------------------------------------------
void led_init(void) //port init
	{
	SEG_A_OUTPUT; SEG_A_CLR;
	SEG_B_OUTPUT; SEG_B_CLR;
	SEG_C_OUTPUT; SEG_C_CLR;
	SEG_D_OUTPUT; SEG_D_CLR;
	SEG_E_OUTPUT; SEG_E_CLR;
	SEG_F_OUTPUT; SEG_F_CLR;
	SEG_G_OUTPUT; SEG_G_CLR;
	SEG_H_OUTPUT; SEG_H_CLR;
	
	COM_1_OUTPUT; COM_1_CLR;
	COM_2_OUTPUT; COM_2_CLR;
	COM_3_OUTPUT; COM_3_CLR;
	}


//---------------------------------------------------------------------------------------
void led_scan(void)  // dynamic lighting
	{
	static uint8_t dig=0;  //counter 0..2 (3 digits)
	static uint8_t seg=0;  //counter 0..7 (7 segments + dot)

	COM_1_INPUT; /* turn OFF all */
	COM_2_INPUT;
	COM_3_INPUT;

	SEG_A_CLR; 
	SEG_B_CLR;
	SEG_C_CLR;
	SEG_D_CLR;
	SEG_E_CLR;
	SEG_F_CLR;
	SEG_G_CLR;
	SEG_H_CLR;
	
	if(BIT_IS_SET(sbuff[dig],0) && seg==0) SEG_H_SET;  /* mux */
	if(BIT_IS_SET(sbuff[dig],1) && seg==1) SEG_A_SET;	
	if(BIT_IS_SET(sbuff[dig],2) && seg==2) SEG_B_SET;
	if(BIT_IS_SET(sbuff[dig],3) && seg==3) SEG_C_SET;
	if(BIT_IS_SET(sbuff[dig],4) && seg==4) SEG_D_SET;
	if(BIT_IS_SET(sbuff[dig],5) && seg==5) SEG_E_SET;
	if(BIT_IS_SET(sbuff[dig],6) && seg==6) SEG_F_SET;
	if(BIT_IS_SET(sbuff[dig],7) && seg==7) SEG_G_SET;

	if(dig==0) { COM_1_OUTPUT; COM_1_CLR; }
	if(dig==1) { COM_2_OUTPUT; COM_2_CLR; }
	if(dig==2) { COM_3_OUTPUT; COM_3_CLR; }

	if(++seg>7) /* count */
		{
		seg=0;
		if(++dig>2) dig=0;
		}
	}


//---------------------------------------------------------------------------------------
void _led_char(uint8_t pos, uint8_t sign) //print a character
	{
	uint8_t tmp=0;

	switch(sign) //select the code of symbol
		{
		case 32: tmp=(pgm_read_byte(&scode[10])); break; //space
		case 45: tmp=(pgm_read_byte(&scode[11])); break; //"-"
		case 48: tmp=(pgm_read_byte(&scode[0])); break; //"0"
		case 49: tmp=(pgm_read_byte(&scode[1])); break; //"1"
		case 50: tmp=(pgm_read_byte(&scode[2])); break; //"2"
		case 51: tmp=(pgm_read_byte(&scode[3])); break; //"3"
		case 52: tmp=(pgm_read_byte(&scode[4])); break; //"4"
		case 53: tmp=(pgm_read_byte(&scode[5])); break; //"5"
		case 54: tmp=(pgm_read_byte(&scode[6])); break; //"6"
		case 55: tmp=(pgm_read_byte(&scode[7])); break; //"7"
		case 56: tmp=(pgm_read_byte(&scode[8])); break; //"8"
		case 57: tmp=(pgm_read_byte(&scode[9])); break; //"9"
		}

	if(pos<=2) sbuff[pos]=(tmp|(sbuff[pos]&0b00000001));
	}


//---------------------------------------------------------------------------------------
void led_print(uint8_t pos, const char *str) //print a string  //position - 0..2
	{
	for(;((*str) && (pos<=2));) _led_char(pos++,*str++);
	}


/*
//---------------------------------------------------------------------------------------
void led_dot(uint8_t pos, uint8_t dot) //position 0..2 //dot 0-off 1-on
	{
	if(pos<=2) sbuff[pos]=(sbuff[pos]&0b11111110)|dot;
	}
*/


//---------------------------------------------------------------------------------------
void led_dots(uint8_t dot0, uint8_t dot1, uint8_t dot2) //dot 0-off 1-on
	{
	sbuff[0]=(sbuff[0]&0b11111110)|dot0;
	sbuff[1]=(sbuff[1]&0b11111110)|dot1;
	sbuff[2]=(sbuff[2]&0b11111110)|dot2;
	}


//---------------------------------------------------------------------------------------
void led_clear(void)  //clear the buffer
	{
	sbuff[0]=0x00;
	sbuff[1]=0x00;
	sbuff[2]=0x00;
	}


//---------------------------------------------------------------------------------------
void led_fill(void)
	{
	sbuff[0]=0xff;
	sbuff[1]=0xff;
	sbuff[2]=0xff;
	}

