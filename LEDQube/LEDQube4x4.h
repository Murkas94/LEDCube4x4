/*
 * LEDQube4x4.h
 *
 * Created: 02.11.2015 01:04:27
 *  Author: Markus
 */ 

#ifndef LEDQUBE4X4_H_
#define LEDQUBE4X4_H_
	void DrawCurrentState(void);
	extern volatile int CancelEffect;
	extern unsigned char cube[4][4][4];
#endif /* LEDQUBE4X4_H_ */