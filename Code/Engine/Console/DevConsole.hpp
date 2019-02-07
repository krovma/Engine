#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Event/EventSystem.hpp"
#include <string>
#include <vector>


enum ConsoleMode
{
	CONSOLE_OFF = 0,
	//CONSOLE_PASSIVE_CONSISE,
	CONSOLE_PASSIVE,
	//CONSOLE_PROMPT,
	//CONSOLE_CONSISE,
	CONSOLE_FULL,
	CONSOLE_ONLY,

	NUM_CONSOLE_MODE
};

class ConsoleOutputItem
{
	friend class DevConsole;

private:
	std::string m_message;
	Rgba m_color;
	int m_messageFrame;
};

class DevConsole
{
public:
	static bool Command_Test(EventParam& param);
public:
	static BitmapFont* s_consoleFont;

public:
	DevConsole(RenderContext* renderer,int line, int column);
	
	void Startup();
	void BeginFrame();
	void RenderConsole();
	void EndFrame();
	void Shutdown();

	void SetConsoleMode(ConsoleMode mode);
	ConsoleMode GetConsoleMode() const { return m_mode; }
	void Print(std::string text, const Rgba& color=Rgba::WHITE);
	void Clear();
	void RunCommandString(std::string cmd) const;

private:
	ConsoleMode m_mode = CONSOLE_OFF;
	IntVec2 m_layout;
	AABB2 m_consoleRect;
	std::vector<ConsoleOutputItem> m_output;
	RenderContext* m_renderer;
	Camera m_consoleCamera;
	int m_frameFromStartup;
};

extern DevConsole* g_theConsole;