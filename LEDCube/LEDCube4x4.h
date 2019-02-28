/*
 * LEDCube4x4.h
 *
 * Created: 02.11.2015 01:04:27
 *  Author: Markus
 */ 

#ifndef LEDCUBE4X4_H_
#define LEDCUBE4X4_H_
	void DrawCurrentState(void);
	extern volatile int CancelEffect;
	extern unsigned char cube[4][4][4];
#endif /* LEDCUBE4X4_H_ */