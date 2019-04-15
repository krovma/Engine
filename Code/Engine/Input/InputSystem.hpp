#pragma once
#include "XboxController.hpp"
//////////////////////////////////////////////////////////////////////////
constexpr int MAX_XBOX_CONTROLLERS = 4;
/////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

constexpr unsigned char KEY_ESC = VK_ESCAPE;
constexpr unsigned char KEY_SPACE = VK_SPACE;
constexpr unsigned char KEY_UPARROW = VK_UP;
constexpr unsigned char KEY_LEFTARROW = VK_LEFT;
constexpr unsigned char KEY_DOWNARROW = VK_DOWN;
constexpr unsigned char KEY_RIGHTARROW = VK_RIGHT;
constexpr unsigned char KEY_F1 = VK_F1;
constexpr unsigned char KEY_F2 = VK_F2;
constexpr unsigned char KEY_F3 = VK_F3;
constexpr unsigned char KEY_F4 = VK_F4;
constexpr unsigned char KEY_F6 = VK_F6;
constexpr unsigned char KEY_F8 = VK_F8;
constexpr unsigned char KEY_A = 'A';
constexpr unsigned char KEY_D = 'D';
constexpr unsigned char KEY_E = 'E';
constexpr unsigned char KEY_N = 'N';
constexpr unsigned char KEY_M = 'M';
constexpr unsigned char KEY_R = 'R';
constexpr unsigned char KEY_S = 'S';
constexpr unsigned char KEY_W = 'W';
constexpr unsigned char KEY_PERIOD = VK_OEM_PERIOD;
constexpr unsigned char KEY_COMMA = VK_OEM_COMMA;
constexpr unsigned char KEY_ENTER = VK_RETURN;
constexpr unsigned char KEY_SLASH = VK_OEM_2; // /?
constexpr unsigned char KEY_BACKSPACE = VK_BACK;
constexpr unsigned char KEY_TAB = VK_TAB;
constexpr unsigned char KEY_DELETE = VK_DELETE;
//////////////////////////////////////////////////////////////////

class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	void StartUp();
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	const XboxController &GetXboxController(int controllerID) const;
	//const ButtonKeyState &GetKeyState(unsigned char key);
private:
	XboxController _controllers[MAX_XBOX_CONTROLLERS] = {
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3),
	};
	//ButtonKeyState _keys[255];
};