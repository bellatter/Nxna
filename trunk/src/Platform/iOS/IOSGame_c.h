#ifndef NXNA_PLATFORM_IOS_IOSGAME_C_H
#define NXNA_PLATFORM_IOS_IOSGAME_C_H

// Objective-C doesn't support namespaces,
// so this provides a C-style interface to the IOSGame class.

void IOSGame_Init();
void IOSGame_Update(float elapsedTime);
void IOSGame_Draw(float elapsedTime);
void IOSGame_TouchDown(float x, float y);
void IOSGame_TouchUp(float x, float y);
void IOSGame_TouchMoved(float x, float y);


#endif // NXNA_PLAFORM_IOS_IOSGAME_C_H
