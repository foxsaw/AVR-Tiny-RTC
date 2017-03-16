//
//
//


#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>



#include "bitm.h"
#include "led.h"
#include "i2c.h"
#include "ds1307.h"
#include "at24c32.h"
#include "ds18b20.h"



#define TIMER1_PRELOAD 65511 //65536-65511=25, 62500/25=2500Hz
//#define TIMER1_PRELOAD 65486 //65536-65511=50, 62500/50=1250Hz
#define TIMER2_PRELOAD 0


//button
#define BUT_HLD  1000 //600
#define BUT_DEB  200 //100

#define BTN_PORT_REG  PORTD
#define BTN_DDR_REG   DDRD
#define BTN_PIN_REG   PIND
#define BTN_PIN       2

#define BTN_INPUT   CLR_BIT(BTN_DDR_REG, BTN_PIN)
#define BTN_OUTPUT  SET_BIT(BTN_DDR_REG, BTN_PIN)
#define BTN_LOW     CLR_BIT(BTN_PORT_REG, BTN_PIN)
#define BTN_HIGH    SET_BIT(BTN_PORT_REG, BTN_PIN)

#define BUTTON_INIT        BTN_INPUT; BTN_HIGH
#define BUTTON_PRESSED     BIT_IS_CLR(BTN_PIN_REG, BTN_PIN)
#define BUTTON_NOTPRESSED  BIT_IS_SET(BTN_PIN_REG, BTN_PIN)


volatile uint8_t butst=0;  //0-not pressed, 1-short press, 2-long press
volatile uint16_t butcnt=0; //counter for button processing

volatile uint16_t scr=0; //counter for display refresh

uint8_t data[8]; //ds1307 data

uint8_t temp[3];



//-----------------------------------------------------------------------------
ISR(INT0_vect)
	{
	//wakeup
	}


//-----------------------------------------------------------------------------
ISR(TIMER1_OVF_vect) //Timer1 overflow interrupt
	{
	TCNT1=TIMER1_PRELOAD;

	if(scr) scr--;

	static uint8_t buthold=0;

	if(BUTTON_PRESSED) //if button is pressed
		{
		if(++butcnt>=BUT_HLD)
			{
			butcnt=0;
			butst=2; //long press
			buthold=1;
			}
		}
	else
		{
		if(butcnt>=BUT_DEB && buthold==0) butst=1; //short press
		buthold=0;
		butcnt=0;
		}


	led_scan(); //mux LED
	}


//-----------------------------------------------------------------------------
void button_reset(void) //reset the button state
	{
	butst=0;
	butcnt=0;
	}


//-----------------------------------------------------------------------------
void get_batt_volt(uint16_t *reg)
	{
	SET_BIT(ADCSRA, ADSC);             //start adc
	while(BIT_IS_CLR(ADCSRA, ADIF));   //wait the end of adc
	*reg = ((109UL*1024UL)/ADC);       //U = 1.1*1024/ADC
	}


//-----------------------------------------------------------------------------
void sleep(void)
	{
	CLR_BIT(PORTB,5); //board LED off

	ADCSRA=0b00000111; //disable ADC

j1:	led_dots(1,1,1);  //turn off the dots  //--<<<--------jump------<<<---
	_delay_ms(200);
	led_dots(1,1,0);
	_delay_ms(100);
	led_dots(1,0,0);
	_delay_ms(100);
	led_dots(0,0,0);

	TIMSK1=0b00000000; //stop timer
	led_init();

	EICRA=0b00000010; //int0 on falling edge
	EIMSK=0b00000001; //enable int0
	EIFR=0;
	SMCR=0b00000101; //set sleep mode

	sleep_cpu();

	EIMSK=0b00000000; //disable int0
	TIMSK1=0b00000001; //start timer

	led_dots(1,0,0);
	_delay_ms(80);
	led_dots(1,1,0);
	_delay_ms(80);
	led_dots(1,1,1);
	_delay_ms(200);

	if(BUTTON_NOTPRESSED) goto j1;   //----->>>------jump------->>>-------

	led_fill();

	ADCSRA=0b10000111;

	_delay_ms(400);

	while(BUTTON_PRESSED);

	scr=0;

	button_reset();
	}


//-----------------------------------------------------------------------------
void setup(void)
	{
	SET_BIT(DDRB,5); //LED
	CLR_BIT(PORTB,5);

	CLKPR=0;
	SREG=0;

	ACSR=0b10000000; //analog comparator disable
	ADMUX=0b01001110; //voltage reference //select channel
	ADCSRA=0b10000111; //prescaler 16M/128=125k

	TIMSK1=0; //timer1 off
	TCCR1A=0; //OC1A/OC1B disconnected
	TCCR1B=0b00000100; //prescaler 16M/256=62500Hz //8M/256=31250Hz
	TCCR1C=0;
	TCNT1=TIMER1_PRELOAD;

	TIMSK2=0; //timer2 off
	TCCR2A=0;
	TCCR2B=0;
	TCNT2=TIMER2_PRELOAD;

	BUTTON_INIT;

	TIMSK1=0b00000001; //start Timer 1

	sei();

	led_init();
	led_fill();

	i2c_init();
	rtc_init();

	_delay_ms(400);

	while(BUTTON_PRESSED);

	button_reset();
	}


//-----------------------------------------------------------------------------
int main(void)
	{
	uint8_t pwroff=0; //flag: 1-power off
	uint8_t mode=0; //display mode: 1-voltage, 0-clock
	uint8_t dsp=0; //0-hours, 1-minutes
	uint8_t set=0; //mode of settings: 0-no, 1-seconds, 2-hours, 3-minutes
	uint8_t tmp=0;
	uint16_t volt=0; //battery

	setup();

	while(1)
		{
		if(pwroff) { sleep(); /*wakeup*/ pwroff=0; mode=0; dsp=0; set=0; }

		if(scr==0)
			{
			get_batt_volt(&volt);

			if(dsp==0 || set)
				{
				rtc_read(data);
				trm_get_temp(temp);
				}

			if(volt<300) { pwroff=1; mode=1; }  //battery low

			if(mode) //show voltage or settings
				{
				if(set) led_dots(0,0,0);
				else led_dots(1,0,pwroff); //decimal dot for voltage

				switch(set)
					{
					case 0: sprintf(strbuff,"%03u",volt); break;     //volt
					case 1: sprintf(strbuff," %02u",data[0]); break; //sec
					case 2: sprintf(strbuff,"%02u-",data[2]); break; //hrs
					case 3: sprintf(strbuff,"-%02u",data[1]); break; //min
					}
				}
			else //show clock
				{
				if(dsp==2) led_dots(0,1,0); //decimal dot for temperature
				else led_dots(0,0,0);

				switch(dsp)
					{
					case 0: sprintf(strbuff,"%02u-",data[2]); break; //hours
					case 1: sprintf(strbuff,"-%02u",data[1]); break; //minutes
					case 2: sprintf(strbuff,"%2u%1u",temp[1],temp[2]); break;
					}
				}

			led_print(0,strbuff);

			if(pwroff==1 && mode==1)  //if battery low
				{
				_delay_ms(2000);
				led_clear();
				button_reset();
				}

			if(++dsp>2) dsp=0;

			if(set) scr=500;
			else scr=2500;
			}

		switch(butst)  /*** button ***/
			{
			case 2:  /******************** long press *******************/
				if(mode) //if voltage mode
					{
					if(++set>3) set=0; //selection of settings
					led_fill();
					_delay_ms(300);
					while(BUTTON_PRESSED);
					}
				else //if clock mode
					{
					led_clear();
					led_dots(1,1,1);
					while(BUTTON_PRESSED);
					pwroff=1; //set power off flag
					}

				dsp=0;
				scr=0;
				button_reset();
				break;

			case 1:  /******************* short press *******************/
				switch(set)
					{
					case 0: //change mode
						if(++mode>1) mode=0;
						led_fill();
						_delay_ms(300);
						break;

					case 1: //reset sec
						rtc_write_reg(0x00, 0 & 0b01111111);
						break;

					case 2:  //set hrs
						tmp = data[2];

						if(++tmp>59) tmp=0;

						tmp = ((tmp/10)<<4)+(tmp%10);
						rtc_write_reg(0x02, tmp & 0b10111111);
						break;

					case 3: //set min
						tmp = data[1];

						if(++tmp>59) tmp=0;

						tmp = ((tmp/10)<<4)+(tmp%10);
						rtc_write_reg(0x01, tmp);
						break;
					}

				dsp=0;
				scr=0;
				button_reset();
				break;
			}
		}
	}


