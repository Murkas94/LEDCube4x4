/*
* BaseEffects.c
*
* Created: 02.11.2015 00:33:13
*  Author: Markus
*/
#include "LEDQube4x4.h"
#include "Clockdef.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "BaseEffects.h"

#ifndef CUBESIZE
#error "\"CUBESIZE\" is not defined"
#endif

#ifndef SLEEPMULTIPLICATOR
#define SLEEPMULTIPLICATOR 1
#endif

unsigned char ReverseByte(unsigned char b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void SetVoxelOverloadSafe(char PosX, char PosY, char PosZ, char Value){
	if(PosX<0||PosY<0||PosZ<0){
		return;
	}
	if(PosX>=CUBESIZE||PosY>=CUBESIZE||PosZ>=CUBESIZE){
		return;
	}
	SetVoxel(PosX,PosY,PosZ,Value);
}

void SetVoxel(char PosX, char PosY, char PosZ, char Value){
	for(int i=0;i<4;i++){
		if((Value&(1<<i))==(1<<i)){
			cube[i][PosY][PosX]|=(1<<PosZ);
			}else{
			cube[i][PosY][PosX]&=~(1<<PosZ);
		}
	}
}

void XLine(char Y, char Z,char StartX, char EndX, char Value){
	for(int X=StartX;X<=EndX;X++){
		SetVoxel(X,Y,Z,Value);
	}
}
void XLineComplete(char Y, char Z, char Value){
	XLine(Y,Z,0,3,Value);
}

void YLine(char X, char Z, char StartY, char EndY, char Value){
	for(int Y=StartY;Y<=EndY;Y++){
		SetVoxel(X,Y,Z,Value);
	}
}
void YLineComplete(char X, char Z, char Value){
	YLine(X,Z,0,3,Value);
}

void ZLine(char X, char Y, char StartZ, char EndZ, char Value){
	for(int Z=StartZ;Z<=EndZ;Z++){
		SetVoxel(X,Y,Z,Value);
	}
}
void ZLineComplete(char X, char Y, char Value){
	ZLine(X,Y,0,3,Value);
}

void XLayer(int X, char Value){
	for(int i=0;i<4;i++){
		if((Value&(1<<i))==(1<<i)){
			for(int Y=0;Y<4;Y++){
				cube[i][Y][X]=0x0F;
			}
			}else{
			for(int Y=0;Y<4;Y++){
				cube[i][Y][X]=0x00;
			}
		}
	}
}

void YLayer(int Y, char Value){
	for(int i=0;i<4;i++){
		if((Value&_BV(i))==_BV(i)){
			for(int X=0;X<4;X++){
				cube[i][Y][X]=0x0F;
			}
			}else{
			for(int X=0;X<4;X++){
				cube[i][Y][X]=0x00;
			}
		}
	}
}

void ZLayer(int Z, char Value){
	for(int i=0;i<4;i++){
		if((Value&(1<<i))==(1<<i)){
			for(int X=0;X<4;X++){
				for(int Y=0;Y<4;Y++){
					cube[i][Y][X]|=(1<<Z);
				}
			}
			}else{
			for(int X=0;X<4;X++){
				for(int Y=0;Y<4;Y++){
					cube[i][Y][X]&=~(1<<Z);
				}
			}
		}
	}
}

void ShiftXRight(void){
	for(int i=0;i<4;i++){
		for(int Y=0;Y<4;Y++){
			for(int X=3;X>0;X--){
				cube[i][Y][X]=cube[i][Y][X-1];
			}
		}
	}
	XLayer(0,0);
}
void ShiftXLeft(void){
	for(int i=0;i<4;i++){
		for(int Y=0;Y<4;Y++){
			for(int X=0;X<3;X++){
				cube[i][Y][X]=cube[i][Y][X+1];
			}
		}
	}
	XLayer(3,0);
}
void ShiftYUp(void){
	for(int i=0;i<4;i++){
		for(int X=0;X<4;X++){
			for(int Y=3;Y>0;Y--){
				cube[i][Y][X]=cube[i][Y-1][X];
			}
		}
	}
	YLayer(0,0);
}
void ShiftYDown(void){
	for(int i=0;i<4;i++){
		for(int X=0;X<4;X++){
			for(int Y=0;Y<3;Y++){
				cube[i][Y][X]=cube[i][Y+1][X];
			}
		}
	}
	YLayer(3,0);
}

void ShiftZBackward(void){
	for(int i=0;i<4;i++){
		for(int Y=0;Y<4;Y++){
			for(int X=0;X<4;X++){
				cube[i][Y][X]=cube[i][Y][X]<<1;
			}
		}
	}
}

void ShiftZForward(void){
	for(int i=0;i<4;i++){
		for(int Y=0;Y<4;Y++){
			for(int X=0;X<4;X++){
				cube[i][Y][X]=cube[i][Y][X]>>1;
			}
		}
	}
}

void LightAll(char Value){
	for(int i=0;i<4;i++){
		if((Value&_BV(i))==_BV(i)){
			for(int X=0;X<4;X++){
				for(int Y=0;Y<4;Y++){
					cube[i][Y][X]=0x0F;
				}
			}
			}else{
			for(int X=0;X<4;X++){
				for(int Y=0;Y<4;Y++){
					cube[i][Y][X]=0x00;
				}
			}
		}
	}
}

int QuadrolizeNumber(int count){
	if(count==0){		//if it is zero, return
		return 0;
	}else if(count<0){	//if it is negative, add 4 until it is positive
		do{
			count+=4;
		}while(count<0);
	}else if(count>3){	//if it is more than 3, remove 4 until it is less than 4 times
		do{
			count-=4;
		}while(count>3);
	}
	return count;
}

void RotateYAxis(int count){
	count=QuadrolizeNumber(count);
	if(count==0){		//if we should rotate it zero times return
		return;
	}
	if(count>=2){		//a rotation of 2 means mirroring on both other axis
		MirrorXAxis();
		MirrorZAxis();
		count-=2;
	}
	if(count==0){		//only counts 1 and 3 come through this
		return;
	}
	//switch X and Z
	unsigned char NewCube[4][4][4]={{{0}}};
	for(int dim =0;dim<4;dim++){
		for(int layer=0;layer<4;layer++){
			for(int x=0;x<4;x++){
				for(int z=0;z<4;z++){
					if((cube[dim][layer][z]&_BV(x))==_BV(x))
					NewCube[dim][layer][x]|=_BV(z);
				}
			}
		}
	}
	memcpy(cube,NewCube,64);
	MirrorXAxis();
}

void RotateXAxis(int count){
	count=QuadrolizeNumber(count);
	if(count==0){		//if we should rotate it zero times return
		return;
	}
	if(count>=2){		//a rotation of 2 means mirroring on both other axis
		MirrorYAxis();
		MirrorZAxis();
		count-=2;
	}
	if(count==0){		//only counts 1 and 3 come through this
		return;
	}
	//switch Y and Z
	unsigned char NewCube[4][4][4]={{{0}}};
	for(int dim =0;dim<4;dim++){
		for(int layer=0;layer<4;layer++){
			for(int x=0;x<4;x++){
				for(int z=0;z<4;z++){
					if((cube[dim][z][x]&_BV(layer))==_BV(layer))
					NewCube[dim][layer][x]|=_BV(z);
				}
			}
		}
	}
	memcpy(cube,NewCube,64);
	MirrorZAxis();
}

void RotateZAxis(int count){
	count=QuadrolizeNumber(count);
	if(count==0){		//if we should rotate it zero times return
		return;
	}
	if(count>=2){		//a rotation of 2 means mirroring on both other axis
		MirrorXAxis();
		MirrorYAxis();
		count-=2;
	}
	if(count==0){		//only counts 1 and 3 come through this
		return;
	}
	//switch X and Y
	char copy[4][4][4];
	memcpy(copy,cube,64);
	for(int dim =0;dim<4;dim++){
		for(int layer=0;layer<4;layer++){
			for(int x=0;x<4;x++){
				cube[dim][layer][x]=copy[dim][x][layer];
			}
		}
	}
	MirrorYAxis();	//Mirror along Y
}

void MirrorXAxis(void){
	char copy[4][4][4];
	memcpy(copy,cube,64);
	for(int dim =0;dim<4;dim++){
		for(int layer=0;layer<4;layer++){
			for(int x=0;x<4;x++){
				cube[dim][layer][x]=copy[dim][layer][3-x];
			}
		}
	}
}

void MirrorYAxis(void){
	char copy[4][4][4];
	memcpy(copy,cube,64);
	for(int dim =0;dim<4;dim++){
		for(int layer=0;layer<4;layer++){
			for(int x=0;x<4;x++){
				cube[dim][layer][x]=copy[dim][3-layer][x];
			}
		}
	}
}

void MirrorZAxis(void){
	//since we only change the same byte we are reading, we don't need a buffer
	for(int dim =0;dim<4;dim++){
		for(int layer=0;layer<4;layer++){
			for(int x=0;x<4;x++){
				cube[dim][layer][x]=ReverseByte(cube[dim][layer][x])>>4;
			}
		}
	}
}

void Inverse(void){
for(int dim=0;dim<4;dim++){
	for(int layer=0;layer<CUBESIZE;layer++){
		for(int x=0;x<CUBESIZE;x++){
			cube[dim][layer][x]=~cube[dim][layer][x];
		}
	}
}
}

void CubeWireFrame(char StartX, char EndX, char StartY, char EndY, char StartZ, char EndZ, char value){
	XLine(StartY,StartZ,StartX,EndX,value);
	XLine(StartY,EndZ,StartX,EndX,value);
	XLine(EndY,StartZ,StartX,EndX,value);
	XLine(EndY,EndZ,StartX,EndX,value);
	YLine(StartX,StartZ,StartY+1,EndY-1,value);
	YLine(StartX,EndZ,StartY+1,EndY-1,value);
	YLine(EndX,StartZ,StartY+1,EndY-1,value);
	YLine(EndX,EndZ,StartY+1,EndY-1,value);
	ZLine(StartX,StartY,StartZ+1,EndZ-1,value);
	ZLine(StartX,EndY,StartZ+1,EndZ-1,value);
	ZLine(EndX,StartY,StartZ+1,EndZ-1,value);
	ZLine(EndX,EndY,StartZ+1,EndZ-1,value);
}

void LightDown(int Step){
	char Intensity;
	for(int x=0;x<4;x++){
		for(int y=0;y<4;y++){
			for(int z=0;z<4;z++){
				Intensity=0;
				for(int dim=0;dim<4;dim++){
					if((cube[dim][x][y]&_BV(z))==_BV(z)){
						Intensity|=_BV(dim);
					}
				}
				if(Intensity>=Step){
					Intensity-=Step;
				}else{
					Intensity=0;
				}
				for(int dim=0;dim<4;dim++){
					if((Intensity&_BV(dim))==_BV(dim)){
						cube[dim][x][y]|=_BV(z);
					}else{
						cube[dim][x][y]&=~_BV(z);
					}
				}
			}
		}
	}
}

void DrawAndDelay(float TimeMS){
	DrawCurrentState();
	TimeMS*=SLEEPMULTIPLICATOR;
	while(TimeMS>=1000){
		_delay_ms(1000);
		TimeMS-=1000;
	}
	while(TimeMS>=100){
		_delay_ms(100);
		TimeMS-=100;
	}
	while(TimeMS>=10){
		_delay_ms(10);
		TimeMS-=10;
	}
	while(TimeMS>=1){
		_delay_ms(1);
		TimeMS-=1;
	}
	TimeMS*=1000;
	//Now TimeMS is Timeµs
	while(TimeMS>=100){
		_delay_us(100);
		TimeMS-=100;
	}
	while(TimeMS>0){
		_delay_us(10);
		TimeMS-=10;
	}
}

void DrawRotatedAndDelay(int Dir, float Delay){
	int Prerotation=Dir-(Dir%4);
	int Rot=Dir-Prerotation;
	char copy[4][CUBESIZE][CUBESIZE];
	memcpy(copy,cube,4*CUBESIZE*CUBESIZE);
	switch(Prerotation){
		case 0:
		RotateYAxis(Rot);
		break;
		case 4:
		RotateXAxis(1);
		RotateZAxis(Rot);
		break;
		case 8:
		RotateXAxis(2);
		RotateYAxis(Rot);
		break;
		case 12:
		RotateXAxis(3);
		RotateZAxis(Rot);
		break;
		case 16:
		RotateZAxis(1);
		RotateXAxis(Rot);
		break;
		case 20:
		RotateZAxis(3);
		RotateXAxis(Rot);
		break;
	}
	DrawAndDelay(Delay);
	memcpy(cube,copy,4*CUBESIZE*CUBESIZE);
}
