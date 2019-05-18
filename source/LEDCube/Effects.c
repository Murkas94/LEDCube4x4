/*
* Effects.c
*
* Created: 02.11.2015 00:02:30
*  Author: Markus
*/
#include "Clockdef.h"
#include "LEDCube4x4.h"
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "Effects.h"
#include "BaseEffects.h"

#ifndef CUBESIZE
#error "\"CUBESIZE\" is not defined"
#endif

#ifndef SLEEPMULTIPLICATOR
#define SLEEPMULTIPLICATOR 1
#endif

void LayerRandomInterrupt(void){
	for(int h=0;h<16;h++){
		XLayer(0,h);
		DrawAndDelay(30);
	}
	char StopValues[CUBESIZE][CUBESIZE];
	while(!CancelEffect){
		XLayer(0,15);
		for(int a=0;a<CUBESIZE;a++){
			for(int b=0;b<CUBESIZE;b++){
				StopValues[a][b]=rand()%CUBESIZE;
			}
		}
		for(int i=1;i<CUBESIZE;i++){
			for(int a=0;a<CUBESIZE;a++){
				for(int b=0;b<CUBESIZE;b++){
					if(i<=StopValues[a][b]){
						SetVoxel(i,a,b,15);
						SetVoxel(i-1,a,b,0);
					}
				}
			}
			DrawAndDelay(30);
		}
		DrawAndDelay(250);
		for(int i=1;i<CUBESIZE;i++){
			for(int a=0;a<CUBESIZE;a++){
				for(int b=0;b<CUBESIZE;b++){
					if(i>StopValues[a][b]){
						SetVoxel(i,a,b,15);
						SetVoxel(i-1,a,b,0);
					}
				}
			}
			DrawAndDelay(30);
		}
		DrawAndDelay(250);
		if(CancelEffect){
			for(int h=15;h>=0;h--){
				XLayer(CUBESIZE-1,h);
				DrawAndDelay(30);
			}
			return;
		}
		
		for(int a=0;a<CUBESIZE;a++){
			for(int b=0;b<CUBESIZE;b++){
				StopValues[a][b]=rand()%CUBESIZE;
			}
		}
		for(int i=CUBESIZE-2;i>=0;i--){
			for(int a=0;a<CUBESIZE;a++){
				for(int b=0;b<CUBESIZE;b++){
					if(i>=StopValues[a][b]){
						SetVoxel(i,a,b,15);
						SetVoxel(i+1,a,b,0);
					}
				}
			}
			DrawAndDelay(30);
		}
		DrawAndDelay(250);
		for(int i=CUBESIZE-2;i>=0;i--){
			for(int a=0;a<CUBESIZE;a++){
				for(int b=0;b<CUBESIZE;b++){
					if(i<StopValues[a][b]){
						SetVoxel(i,a,b,15);
						SetVoxel(i+1,a,b,0);
					}
				}
			}
			DrawAndDelay(30);
		}
		DrawAndDelay(250);
		LightAll(0);
	}
	for(int h=15;h>=0;h--){
		XLayer(0,h);
		DrawAndDelay(30);
	}
}

void Flash(void){
	for(int i=0;i<16;i++){
		DrawAndDelay(50);
		LightAll(i);
	}
	for(int i=15;i>=0;i--){
		DrawAndDelay(50);
		LightAll(i);
	}
}

void CubeWireEffect(void){
	for(int a=0;a<16;a++){
		LightAll(0);
		CubeWireFrame(0,CUBESIZE-1,0,CUBESIZE-1,0,CUBESIZE-1,a);
		DrawAndDelay(30);
	}
	if(!CancelEffect){
		DrawAndDelay(500);
	}
	char StartX;
	char StartY;
	char StartZ;
	char Direction=0x00;
	char LastDirection=0x00;
	while(!CancelEffect){
		while(Direction==LastDirection){
			Direction=rand()%8;
		}
		LastDirection=Direction;
		StartX=0;
		StartY=0;
		StartZ=0;
		#if(CUBESIZE==4)
		for(char l=CUBESIZE-1;l>0;l--){
			#else
			for(char l=CUBESIZE-1;l>1;l--){
				#endif
				LightAll(0);
				CubeWireFrame(StartX,StartX+l,StartY,StartY+l,StartZ,StartZ+l,15);
				if((Direction&0x01)==0x01){
					StartX++;
				}
				if((Direction&0x02)==0x02){
					StartY++;
				}
				if((Direction&0x04)==0x04){
					StartZ++;
				}
				DrawAndDelay(50);
				#if(CUBESIZE==4)
			}
			#else
		}
		#endif
		DrawAndDelay(50);
		#if(CUBESIZE==4)
		for(char l=1;l<CUBESIZE;l++){
			#else
			for(char l=2;l<CUBESIZE;l++){
				#endif
				if((Direction&0x01)==0x01){
					StartX--;
				}
				if((Direction&0x02)==0x02){
					StartY--;
				}
				if((Direction&0x04)==0x04){
					StartZ--;
				}
				LightAll(0);
				CubeWireFrame(StartX,StartX+l,StartY,StartY+l,StartZ,StartZ+l,15);
				DrawAndDelay(50);
				#if(CUBESIZE==4)
			}
			#else
		}
		#endif
	}
	
	LightAll(0);
	for(int a=15;a>=0;a--){
		CubeWireFrame(0,CUBESIZE-1,0,CUBESIZE-1,0,CUBESIZE-1,a);
		DrawAndDelay(30);
		LightAll(0);
	}
}

void RainEffect(void){
	LightAll(0);
	char xpos;
	char zpos;
	char count;
	while(!CancelEffect){
		count=rand()%5;
		for(int a=0;a<count;a++){
			xpos = rand()%CUBESIZE;
			zpos = rand()%CUBESIZE;
			SetVoxel(xpos,CUBESIZE-1,zpos,15);
		}
		DrawAndDelay(40);
		ShiftYDown();
	}
	for(int i=0;i<CUBESIZE-1;i++){
		ShiftYDown();
		DrawAndDelay(40);
	}
}

void WaveEffect(void){
	int Rotation;
	LightAll(0);
	while(!CancelEffect){
		Rotation=rand()%24;
		char buffer[4][4][4];
		for(int a=1;a<16;a++){
			XLayer(0,a);
			DrawRotatedAndDelay(Rotation,25);
			ShiftXRight();
		}
		for(int a=14;a>0;a--){
			XLayer(0,a);
			DrawRotatedAndDelay(Rotation,25);
			ShiftXRight();
		}
		for(int a=0;a<16;a++){
			DrawRotatedAndDelay(Rotation,25);
			ShiftXRight();
		}
		LightAll(0);
	}
}

void UpDownLayerSwitch(void){
	#if (CUBESIZE>8)
	#error "This Effects is not able to use cubes larger than 8"
	#endif
	
	LightAll(0);
	for(int a=1;a<16;a++){
		LightAll(0);
		YLayer(CUBESIZE-1,a);
		DrawAndDelay(25);
	}
	_delay_ms(250*SLEEPMULTIPLICATOR);
	
	unsigned char UpperLayer[CUBESIZE];		//determining if the voxel is currently up (1) or down (0)
	for(int i=0;i<CUBESIZE;i++){
		UpperLayer[i]=0xFF;
	}
	char x=0;
	char z=0;
	
	char lastX=!x;
	char lastZ=!z;
	
	while(!CancelEffect){
		do{
			x=rand()%CUBESIZE;				//random x position
			z=rand()%CUBESIZE;				//random z position
		}while(lastX==x||lastZ==z);
		lastZ=z;
		lastX=x;
		if((UpperLayer[x]&_BV(z))==_BV(z)){	//if the voxel is up
			UpperLayer[x]&=~_BV(z);			//remove it logically from there
			for(int y=CUBESIZE-1;y>0;y--){	//move it down
				SetVoxel(x,y,z,0);
				SetVoxel(x,y-1,z,15);
				DrawAndDelay(20);
			}
			}else{							//otherwise
			UpperLayer[x]|=_BV(z);	//add it logically there
			for(int y=0;y<CUBESIZE-1;y++){		//move it up
				SetVoxel(x,y,z,0);
				SetVoxel(x,y+1,z,15);
				DrawAndDelay(20);
			}
		}
	}
	
	for(int a=1;a<16;a++){
		LightDown(1);
		DrawAndDelay(25);
	}
}

void MovingCube(void){
	//light center up
	for(int a=1;a<16;a++){
		LightAll(0);
		CubeWireFrame(1,2,1,2,1,2,a);
		DrawAndDelay(25);
	}
	if(!CancelEffect){
		char PossibleDirections;	// x+/- z+/- y+/- n.D./n.D.
		char MoveDirection=rand()%8;
		char LastMoveDirectionOpposite=0x00;
		
		//move it from center into edge and activate the opposite direction
		if((MoveDirection&0x01)==0x01){
			ShiftXRight();
			PossibleDirections=0x02;
			}else{
			ShiftXLeft();
			PossibleDirections=0x01;
		}
		if((MoveDirection&0x02)==0x02){
			ShiftZBackward();
			PossibleDirections|=0x08;
			}else{
			ShiftZForward();
			PossibleDirections|=0x04;
		}
		if((MoveDirection&0x04)==0x04){
			ShiftYUp();
			PossibleDirections|=0x20;
			}else{
			ShiftYDown();
			PossibleDirections|=0x10;
		}
		
		DrawAndDelay(250);
		
		while (!CancelEffect){
			do{
				MoveDirection=_BV(rand()%6);
			} while (((~PossibleDirections)&MoveDirection)==MoveDirection);
			PossibleDirections|=LastMoveDirectionOpposite; //Add opposite direction from last round
			//Move into direction and save opposite direction
			for(int move=0;move<2;move++){
				if((MoveDirection&0x01)==0x01){
					ShiftXRight();
					LastMoveDirectionOpposite=0x02;
					}else if((MoveDirection&0x02)==0x02){
					ShiftXLeft();
					LastMoveDirectionOpposite=0x01;
					}else if((MoveDirection&0x04)==0x04){
					ShiftZBackward();
					LastMoveDirectionOpposite=0x08;
					}else if((MoveDirection&0x08)==0x08){
					ShiftZForward();
					LastMoveDirectionOpposite=0x04;
					}else if((MoveDirection&0x10)==0x10){
					ShiftYUp();
					LastMoveDirectionOpposite=0x20;
					}else if((MoveDirection&0x20)==0x20){
					ShiftYDown();
					LastMoveDirectionOpposite=0x10;
				}
				DrawAndDelay(100);
			}
			PossibleDirections&=~MoveDirection;		//deactivate done movement direction
		}
	}
	
	for(int a=1;a<16;a++){
		LightDown(1);
		DrawAndDelay(25);
	}
}

void Snake(void){
	#ifndef SnakeCount
	#define SnakeCount 2
	#endif
	char Positions[SnakeCount][3][7]={{{0}}};	//count of snakes/x,y,z/length of snakes
	char InitialisedCount=1;
	char buffer=0;
	char DirBuffer=0;
	char NewPos[3];
	char TryCount=0;
	char Failed=0;
	for(int d=0;d<3;d++){
		for(int b=0;b<SnakeCount;b++){
			Positions[b][d][0]=rand()%CUBESIZE;		//initialise positions
		}
	}
	for(int a=1;a<16;a++){
		for(int b=0;b<SnakeCount;b++){
			SetVoxel(Positions[b][0][0],Positions[b][1][0],Positions[b][2][0],a);	//light up
		}
		DrawAndDelay(25);
	}
	_delay_ms(200*SLEEPMULTIPLICATOR);
	while(!CancelEffect){
		if(InitialisedCount<7){
			InitialisedCount++;
		}
		for(int b=0;b<SnakeCount;b++){
			//update old positions
			for(int a=InitialisedCount-1;a>0;a--){
				for(int d=0;d<3;d++){
					Positions[b][d][a]=Positions[b][d][a-1];
				}
			}
			//move
			TryCount=0;
			do{
				Failed=0;
				NewPos[0]=Positions[b][0][0];			//move positions into posBuffer
				NewPos[1]=Positions[b][1][0];
				NewPos[2]=Positions[b][2][0];
				DirBuffer=rand()%6;						//chose one of 6 possible directions;
				for(int d=0;d<3;d++){
					buffer=NewPos[d];					//move x/y/z coordinate into buffer
					if(DirBuffer==d && buffer<CUBESIZE-1){	//modify coordinate
						buffer++;
					}
					if(DirBuffer==(3+d) && buffer>0){
						buffer--;
					}
					NewPos[d]=buffer;
				}
				for(int a=0;a<InitialisedCount;a++){	//check if new Position is already in use and mark it with failed
					for(int c=0;c<SnakeCount;c++){
						if(NewPos[0]==Positions[c][0][a]&&NewPos[1]==Positions[c][1][a]&&NewPos[2]==Positions[c][2][a]){
							Failed=1;
							break;
						}
					}
					if(Failed){
						TryCount++;						//if failed, then try again
						if(TryCount==50){				//if we tryed 50 times, we take a new position
							for(int d=0;d<3;d++){
								NewPos[d]=rand()%CUBESIZE;
							}
							TryCount=0;					//we reset everything
							Failed=0;
							}else{
							break;
						}
					}
				}
			}while(Failed);
			Positions[b][0][0]=NewPos[0];
			Positions[b][1][0]=NewPos[1];
			Positions[b][2][0]=NewPos[2];
		}
		//Draw current state
		LightAll(0);
		for(int a=0;a<InitialisedCount;a++){
			for(int b=0;b<SnakeCount;b++)
			SetVoxel(Positions[b][0][a],Positions[b][1][a],Positions[b][2][a],16-2*a);
		}
		DrawAndDelay(50);
	}
	for(int a=1;a<16;a++){
		LightDown(1);
		DrawAndDelay(25);
	}
}

void Firework(void){
	int PosX;
	int PosZ;
	int DownStep=(int)8/CUBESIZE;
	while(!CancelEffect){
		PosX=rand()%CUBESIZE;
		PosZ=rand()%CUBESIZE;
		for(int i=0;i<CUBESIZE;i++){
			LightDown(DownStep);
			SetVoxel(PosX,i,PosZ,7);
			DrawAndDelay(100);
		}
		#if CUBESIZE==8
		for(int i=0;i<4;i++){
			#elif CUBESIZE==4
			for(int i=0;i<2;i++){
				#else
				#error "\"CUBESIZE\" is not valid for this effect"
				#endif
				LightDown(DownStep);
				DrawAndDelay(100);
			}
			SetVoxel(PosX,CUBESIZE-1,PosZ,15);
			DrawAndDelay(100);
			LightDown(DownStep);
			SetVoxelOverloadSafe(PosX+1,CUBESIZE-1,PosZ,15-DownStep);
			SetVoxelOverloadSafe(PosX-1,CUBESIZE-1,PosZ,15-DownStep);
			SetVoxelOverloadSafe(PosX,CUBESIZE-1,PosZ+1,15-DownStep);
			SetVoxelOverloadSafe(PosX,CUBESIZE-1,PosZ-1,15-DownStep);
			#if CUBESIZE>4
			SetVoxelOverloadSafe(PosX+1,CUBESIZE-1,PosZ+1,15-DownStep);
			SetVoxelOverloadSafe(PosX-1,CUBESIZE-1,PosZ-1,15-DownStep);
			SetVoxelOverloadSafe(PosX-1,CUBESIZE-1,PosZ+1,15-DownStep);
			SetVoxelOverloadSafe(PosX+1,CUBESIZE-1,PosZ-1,15-DownStep);
			DrawAndDelay(100);
			LightDown(DownStep);
			SetVoxelOverloadSafe(PosX+2,CUBESIZE-2,PosZ,15-2*DownStep);
			SetVoxelOverloadSafe(PosX-2,CUBESIZE-2,PosZ,15-2*DownStep);
			SetVoxelOverloadSafe(PosX,CUBESIZE-2,PosZ+2,15-2*DownStep);
			SetVoxelOverloadSafe(PosX,CUBESIZE-2,PosZ-2,15-2*DownStep);
			SetVoxelOverloadSafe(PosX+2,CUBESIZE-2,PosZ+2,15-DownStep);
			SetVoxelOverloadSafe(PosX-2,CUBESIZE-2,PosZ-2,15-DownStep);
			SetVoxelOverloadSafe(PosX-2,CUBESIZE-2,PosZ+2,15-DownStep);
			SetVoxelOverloadSafe(PosX+2,CUBESIZE-2,PosZ-2,15-DownStep);
			#else
			DrawAndDelay(100);
			LightDown(DownStep);
			#endif
			DrawAndDelay(100);
			for(int i=3;15-i*DownStep>0;i++){
				LightDown(DownStep);
				DrawAndDelay(100);
			}
			LightAll(0);
			DrawAndDelay(100);
			#if(CUBESIZE==4)
		}
		#else
	}
	#endif
}

void ConwaysGameOfLife(void){
	char Board[CUBESIZE][CUBESIZE][CUBESIZE];
	char NewBoard[CUBESIZE][CUBESIZE][CUBESIZE];
	char CurrentCount;
	signed char CalcedX;
	signed char CalcedY;
	signed char CalcedZ;
	int LightCount=0;
	int MaxCount=CUBESIZE*CUBESIZE*CUBESIZE;
	while(!CancelEffect){
		//Board generieren
		for(char x=0;x<CUBESIZE;x++){
			for(char y=0;y<CUBESIZE;y++){
				for(char z=0;z<CUBESIZE;z++){
					Board[x][y][z]=(char)(rand()%2);
					if(Board[x][y][z]==1){
						SetVoxel(x,y,z,15);
					}
				}
			}
		}
		DrawAndDelay(250);
		char GameFinished=0;
		while(!CancelEffect&&!GameFinished){
			LightCount=0;
			memset(NewBoard,0,CUBESIZE*CUBESIZE*CUBESIZE);
			LightAll(0);
			for(char x=0;x<CUBESIZE;x++){
				for(char y=0;y<CUBESIZE;y++){
					for(char z=0; z<CUBESIZE; z++){
						CurrentCount=0;
						//go through offsets
						for(signed char xo=-1;xo<2;xo++){
							CalcedX=x+xo;
							if(CalcedX<0){
								//if negative, we use MAX
								CalcedX=CUBESIZE-1;
								}else if(CalcedX >=CUBESIZE){
								//if over MAX, we use 0
								CalcedX=0;
							}
							for(signed char yo=-1;yo<2;yo++){
								CalcedY=y+yo;
								if(CalcedY<0){
									CalcedY=CUBESIZE-1;
									}else if(CalcedY >=CUBESIZE){
									CalcedY=0;
								}
								for(signed char zo=-1;zo<2;zo++){
									if((xo==0)&&(yo==0)&&(zo==0)){
										continue;
									}
									//with the following, only direct neighbors will count
									if(ABS(xo)+ABS(yo)+ABS(yo)>1){
										continue;
									}
									CalcedZ=z+zo;
									if(CalcedZ<0){
										CalcedZ=CUBESIZE-1;
										}else if(CalcedZ >=CUBESIZE){
										CalcedZ=0;
									}
									if(Board[CalcedX][CalcedY][CalcedZ]==1){
										CurrentCount++;
									}
								}
							}
						}
						//calculate and draw new board
						if(Board[x][y][z]==1){
							if(CurrentCount>1&&CurrentCount<4){
								NewBoard[x][y][z]=1;
								SetVoxel(x,y,z,15);
								LightCount++;
							}
							}else{
							if(CurrentCount>2&&CurrentCount<4){
								NewBoard[x][y][z]=1;
								SetVoxel(x,y,z,15);
								LightCount++;
							}
						}
					}
				}
			}
			memcpy(Board,NewBoard,CUBESIZE*CUBESIZE*CUBESIZE);
			DrawAndDelay(250);
			if(LightCount==0||LightCount==MaxCount){
				GameFinished=1;
				_delay_ms(500*SLEEPMULTIPLICATOR);
			}
		}
	}
}

void Whirl(void){
	int X;
	int Y;
	int Z;
	char LayerDir;
	char Inverted=1;
	int Rotation=0;
	while(!CancelEffect){
		Inverted=!Inverted;
		X=1;
		Y=0;
		Z=0;
		LayerDir=0;
		if(!Inverted){
			Rotation=rand()%24;
		}
		for(int i=0;i<4*(CUBESIZE-1)*CUBESIZE;i++){
			if(Inverted){
				for(int l=15;l>=0;l--){
					SetVoxel(X,Y,Z,l);
					DrawRotatedAndDelay(Rotation,2);
				}
			}else{
				for(int l=1;l<16;l++){
					SetVoxel(X,Y,Z,l);
					DrawRotatedAndDelay(Rotation,2);
				}
			}
			if((i+1)%(CUBESIZE-1)==0){
				LayerDir++;
				if(LayerDir==4){
					LayerDir=0;
				}
				if((i+1)%((CUBESIZE-1)*4)==0){
					Y++;
				}
			}
			switch(LayerDir){
				case 0:
				X++;
				break;
				case 1:
				Z++;
				break;
				case 2:
				X--;
				break;
				case 3:
				Z--;
				break;
			}
		}
	}
	for(int i=15;i>=0;i--){
		LightDown(1);
		DrawRotatedAndDelay(Rotation,10);
	}
}

int ABS(int input){
	if(input<0){
		return -input;
	}
	return input;
}