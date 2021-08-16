///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Keyboard
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___KEYBD_H___
#define ___KEYBD_H___

#define KEY_TAB				0x09
#define KEY_ESC				0x1B
#define KEY_ENTER			0x0D
#define KEY_SPACE			0x20
#define KEY_BACKSPACE		0x08

#define KEY_ARROW_UP		0xE048
#define KEY_ARROW_DOWN		0xE050
#define KEY_ARROW_LEFT		0xE04B
#define KEY_ARROW_RIGHT		0xE04D
#define KEY_END				0xE04F
#define KEY_HOME			0xE047
#define KEY_PAGE_UP			0xE049
#define KEY_PAGE_DOWN		0xE051
#define KEY_INSERT			0xE052
#define KEY_DELETE			0xE053

#define CHAR_ARROW_UP		30
#define CHAR_ARROW_DOWN		31
#define CHAR_ARROW_LEFT		17
#define CHAR_ARROW_RIGHT	16

#define CHAR_DESKTOP_1		176
#define CHAR_DESKTOP_2		177
#define CHAR_DESKTOP_3		178

#define SPACE				' '

#ifdef __cplusplus
extern "C" {
#endif

int getkey();

#ifdef __cplusplus
};
#endif

#endif
