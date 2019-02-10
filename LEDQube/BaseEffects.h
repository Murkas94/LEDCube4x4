/*
 * BaseEffects.h
 *
 * Created: 02.11.2015 01:11:15
 *  Author: Markus
 */ 


#ifndef BASEEFFECTS_H_
#define BASEEFFECTS_H_

void SetVoxel(char PosX, char PosY, char PosZ, char Value);
void SetVoxelOverloadSafe(char PosX, char PosY, char PosZ, char Value);

void XLine(char Y, char Z,char StartX, char EndX, char Value);
void XLineComplete(char Y, char Z, char Value);
void YLine(char X, char Z, char StartY, char EndY, char Value);
void YLineComplete(char X, char Z, char Value);
void ZLine(char X, char Y, char StartZ, char EndZ, char Value);
void ZLineComplete(char X, char Y, char Value);

void XLayer(int X, char Value);
void YLayer(int Y, char Value);
void ZLayer(int Z, char Value);

void ShiftXRight(void);
void ShiftXLeft(void);
void ShiftYUp(void);
void ShiftYDown(void);
void ShiftZBackward(void);
void ShiftZForward(void);

void RotateXAxis(int count);
void RotateYAxis(int count);
void RotateZAxis(int count);

void MirrorXAxis(void);
void MirrorYAxis(void);
void MirrorZAxis(void);

void Inverse(void);

void LightAll(char Value);

void CubeWireFrame(char StartX, char EndX, char StartY, char EndY, char StartZ, char EndZ, char value);

void LightDown(int Step);

void DrawAndDelay(float TimeMS);

void DrawRotatedAndDelay(int Dir, float Delay);

#endif /* BASEEFFECTS_H_ */