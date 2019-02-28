/*
* LEDCube.c
*
* Created: 20.04.2015 22:52:28
*  Author: Markus
* Fuses:
*	high:	0xD9	avrdude -c usbtiny -p m328p -U hfuse:w:0xD9:m
*	low:	0xE2	avrdude -c usbtiny -p m328p -U lfuse:w:0xE2:m
* Ports:
* A:	LED 1-8
* C:	LED 9-16
* B0-3: LayerOutput
* B4-5: Buttons
* B6:	Reset
*/
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Clockdef.h"
#include "LEDCube4x4.h"
#include "Effects.h"
#include "BaseEffects.h"
#include <string.h>

#define LAYER_SELECT PORTC
#define BUTTON1 0x20
#define BUTTON2 0x10
#define BUTTON3 0x40

void SetLayerSelect(char layer);
void EnableCubeDraw(void);
void Loop(void);
void Button1Pressed(void);
void Button2Pressed(void);

unsigned char cube[4][4][4];
volatile unsigned char drawbuffer[4][4][4];
//dim, Layer/y, x, Wert: z
volatile unsigned int BAMCounter=0;	//the brigthness counter (0-15)
volatile unsigned int BAMBit=0;		//the brigthness bit (0-3)
volatile int CurrentLayer=0;		//the current lightend layer (0-3)
volatile int CancelEffect=0;		//should the current effect be canceled

int SecondsTimer=0;						//when this reaches 391, then one second passed
volatile int SecondsCounter=0;			//this actually counts the passed seconds
volatile int SecondsAbortTime=30;		//this is the time, after which the current effects should be canceled
volatile char EnableSecondsCompare=1;

volatile unsigned int DebounceButtonCounter=0xFFFF;

int main(void)
{
	DDRB=0xFF;			//Complete PortA is for Output for LEDs 1-8
	PORTB=0x00;			//Everything clear
	DDRD=0xFF;			//Complete PortC is for Output for LEDs 9-16
	PORTD=0x00;			//Everything clear
	DDRC=0x0F;			//0-3 for layer output, 4-5 for buttons, 6 for reset
	PORTC=0x70;			//Pullups on buttons enabled
	SetLayerSelect(0x00);	//Disable all Layers
	OCR2A=10;			//interrupt at count 10
	TCCR2A|=_BV(WGM21);	//CTC-Mode (Clear TCNT2, when OCR2 is reached) "Clear Timer on Compare"
	TCNT2=0x00;			//Counter for timer2
	TIMSK2|= _BV(OCIE2A);	//Enable CTC-Interrupt
	
	PCICR|=_BV(PCIE1);	//Enable PinChangeInterrupt 1 (For buttons)
	PCMSK1|= _BV(PCINT12)|_BV(PCINT13);	//Enable PCI on int12/13
	
	sei();
	
	/*
	DDRC=0x0;		//PortC for reading
	PORTC=0xFFF;	//Enabled internal pull-ups
	if(PINC==0x1){	//Read PortC
	
	}
	*/
	EnableCubeDraw();
	
	while(1)
	{
		Loop();
	}
}

void SetLayerSelect(char layer){
	LAYER_SELECT=(LAYER_SELECT&0xF0)|(layer&0x0F);
}

void EnableCubeDraw(void){
	//TCCR2B|=(1<<CS20)|(1<<CS21);
	TCCR2B|=(1<<CS22);
}
void DisableCubeDraw(void){
	//TCCR2B&=~((1<<CS20)|1<<CS21);
	TCCR2B&=~(1<<CS22);
}

void DrawCurrentState(void){
	memcpy(drawbuffer,cube,64);
}

ISR(TIMER2_COMPA_vect){
	if(DebounceButtonCounter!=0xFFFF){
		DebounceButtonCounter++;
	}
	//Writes the next layer
	SetLayerSelect(0x00);
	#if REVERSEDLAYERS==1
	PORTD=~((drawbuffer[BAMBit][CurrentLayer][3]&0x0F)|((drawbuffer[BAMBit][CurrentLayer][2]<<4)&0xF0));
	PORTB=~((drawbuffer[BAMBit][CurrentLayer][1]&0x0F)|((drawbuffer[BAMBit][CurrentLayer][0]<<4)&0xF0));
	#else
	PORTD=(drawbuffer[BAMBit][CurrentLayer][3]&0x0F)|((drawbuffer[BAMBit][CurrentLayer][2]<<4)&0xF0);
	PORTB=(drawbuffer[BAMBit][CurrentLayer][1]&0x0F)|((drawbuffer[BAMBit][CurrentLayer][0]<<4)&0xF0);
	#endif
	SetLayerSelect(_BV(CurrentLayer));
	CurrentLayer++;
	if(CurrentLayer==4){			//if all layers were drawn, increase the brightness-counter and possibly the brightness-bit
		CurrentLayer=0;
		BAMCounter++;
		if(BAMCounter==1){
			BAMBit++;
			}else if(BAMCounter==3){
			BAMBit++;
			}else if(BAMCounter==7){
			BAMBit++;
			}else if(BAMCounter==15){
			BAMCounter=0;
			BAMBit=0;
			//the following counts seconds and cancels effects on given time
			if(EnableSecondsCompare){
				SecondsTimer++;
				if(SecondsTimer==195){	//8.000.000/64(prescaler)/10(counter)/4(layer)/16(BAM)
					SecondsTimer=0;
					SecondsCounter++;
					if(SecondsCounter==SecondsAbortTime){
						CancelEffect=1;
					}
				}
			}
		}
	}
}

volatile char LastButtons=0x00;

ISR(PCINT1_vect){
	char CurrentButtons=~PINC;
	if(DebounceButtonCounter>1250){	//8.000.000/64(prescaler)/10(counter) = 25000/Sekunde => 1/5 Sekunde debounce
		//die drücken-flanke wird gezählt, da diese negativ ist, alle folgenden nicht, wegen debounce Zeit
		//die loslassen-flanke wird nicht gezählt(positiv). Alle folgenden auch nicht, da debounce-Zeit läuft
		DebounceButtonCounter=0;
		for(int i=4;i<=6;i++){
			//alle Knöpfe durchzählen, wenn sich ein Knopfzustand vom letzten Unterscheidet und dieser negativ ist, zählt es als Knopfdruck
			if(((CurrentButtons&_BV(i))!=(LastButtons&_BV(i)))&&( (CurrentButtons&_BV(i))==(_BV(i)) )){
				switch(i){
					case 4:
					Button2Pressed();
					LastButtons=CurrentButtons;
					return;
					case 5:
					Button1Pressed();
					LastButtons=CurrentButtons;
					return;
					//case 6:
					//reset
					//return;
				}
			}
		}
	}
	LastButtons=CurrentButtons;
}

void Button1Pressed(void){
	CancelEffect=1;
}
void Button2Pressed(void){
	switch(SecondsAbortTime){
		case 300:
		SecondsAbortTime=60;
		break;
		case 60:
		SecondsAbortTime=30;
		break;
		case 30:
		SecondsAbortTime=15;
		break;
		case 15:
		//wenn der Effekttimer läuft, dann abschalten (Effekt undendlich lang), ansonsten anschalten und auf 300 s setzen
		if(EnableSecondsCompare){
			EnableSecondsCompare=0;
			}else{
			EnableSecondsCompare=1;
			SecondsAbortTime=300;
		}
		break;
	}
	SecondsCounter=0;
}

char LastEffectNumber=0xFF;

void Loop(void){
	CancelEffect=0;
	SecondsCounter=0;
	char EffectNumber;
	if(EnableSecondsCompare){
		//wenn wir einen Effekt-Timer haben, dann wird der nächste Effekt zufällig ausgewählt, ansonsten per Reihenfolge
		do{
			EffectNumber=rand()%10;
		}while(LastEffectNumber==EffectNumber);
		}else{
		if(LastEffectNumber==9){
			EffectNumber=0;
			}else{
			EffectNumber=LastEffectNumber+1;
		}
	}
	LastEffectNumber=EffectNumber;
	switch(EffectNumber){
		case 0:
		WaveEffect();
		break;
		case 1:
		RainEffect();
		break;
		case 2:
		CubeWireEffect();
		break;
		case 3:
		LayerRandomInterrupt();
		break;
		case 4:
		UpDownLayerSwitch();
		break;
		case 5:
		MovingCube();
		break;
		case 6:
		Snake();
		break;
		case 7:
		Firework();
		break;
		case 8:
		ConwaysGameOfLife();
		break;
		case 9:
		Whirl();
		break;
	}
	LightAll(0);
	DrawCurrentState();
}