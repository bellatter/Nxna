#ifndef NXNA_PLATFORM_IOS_IOSGAME_C_H
#define NXNA_PLATFORM_IOS_IOSGAME_C_H

// Objective-C doesn't support namespaces,
// so this provides a C-style interface to the IOSGame class.

#ifdef __cplusplus 
extern "C"
{
#endif

void IOSGame_Init();
void IOSGame_Update(float elapsedTime);
void IOSGame_Draw(float elapsedTime);
void IOSGame_TouchDown(float x, float y);
void IOSGame_TouchUp(float x, float y);
void IOSGame_TouchMoved(float x, float y);
void IOSGame_IsActive(bool active);
	
void IOSGame_GetScreenSize(int* width, int* height);

#ifdef __cplusplus
}
#endif

#endif // NXNA_PLAFORM_IOS_IOSGAME_C_H
