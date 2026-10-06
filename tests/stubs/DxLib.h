#pragma once

#define KEY_INPUT_W 1
#define KEY_INPUT_A 2
#define KEY_INPUT_S 3
#define KEY_INPUT_D 4
#define KEY_INPUT_SPACE 5
#define KEY_INPUT_LSHIFT 6
#define KEY_INPUT_LCONTROL 7
#define KEY_INPUT_ESCAPE 8
#define KEY_INPUT_R 9
#define KEY_INPUT_E 10
#define MOUSE_INPUT_LEFT 1
#define MOUSE_INPUT_RIGHT 2
#define MOUSE_INPUT_MIDDLE 4
int CheckHitKey(int key);
int GetMouseInput();
int GetMousePoint(int* x, int* y);
