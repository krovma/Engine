#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

//////////////////////////////////////////////////////////////////////////
STATIC BitmapFont* DevConsole::s_consoleFont = nullptr;

////////////////////////////////
STATIC bool DevConsole::Command_Test(EventParam& param)
{
	g_theConsole->Print(Stringf("[Test] Called with arguments:\n"));
	param.DebugPrintToConsole(g_theConsole);
	return true;
}

////////////////////////////////
DevConsole::DevConsole(RenderContext* renderer, int line, int column)
	: m_renderer(renderer)
	, m_layout(column, line)
	, m_consoleRect(0, 0,
		s_consoleFont->GetGlyphAspect('m') * column, (float) line)
	, m_consoleCamera(m_consoleRect.Min, m_consoleRect.Max)
{
}

////////////////////////////////
void DevConsole::Startup()
{
	m_frameFromStartup = 0;
}

////////////////////////////////
void DevConsole::BeginFrame()
{

}

////////////////////////////////
void DevConsole::EndFrame()
{
	++m_frameFromStartup;
}

////////////////////////////////
void DevConsole::Shutdown()
{

}

////////////////////////////////
void DevConsole::SetConsoleMode(ConsoleMode mode)
{
	m_mode = mode;
}

////////////////////////////////
void DevConsole::Print(std::string text, const Rgba& color)
{
	ConsoleOutputItem item;
	item.m_message = text;
	item.m_color = color;
	item.m_messageFrame = m_frameFromStartup;
	m_output.push_back(item);
}

////////////////////////////////
void DevConsole::Clear()
{
	m_output.clear();
}

////////////////////////////////
void DevConsole::RunCommandString(std::string cmd) const
{
	std::vector<std::string> allCmds = Split(cmd.c_str(), '\n');
	for (auto& eachCmd : allCmds) {
		std::vector<std::string> argv = Split(eachCmd.c_str(), ' ');
		int argc = (int)argv.size();
		if (argc > 0) {
			std::string cmdName = argv[0];
			NamedStrings args;
			for (int i = 1; i < argc; ++i) {
				std::vector<std::string> keyValuePair = Split(argv[i].c_str(), '=');
				std::string key = "null";
				std::string value = "null";
				if (keyValuePair.size() > 0) {
					key = keyValuePair[0];
					if (keyValuePair.size() > 1) {
						value = keyValuePair[1];
					}
				}
				args.Set(key, value);
			}
			g_Event->Trigger(cmdName, args);
		}
	}
}

////////////////////////////////
void DevConsole::RenderConsole() 
{
	if (m_mode == CONSOLE_OFF) {
		return;
	}

	std::vector<Vertex_PCU> verts;
	if (m_mode == CONSOLE_PASSIVE) {
		m_renderer->BeginCamera(m_consoleCamera);
		int numPrintedLines = 0;
		for (int line = (int) m_output.size() - 1; line >= 0; --line) {
			if (numPrintedLines >= m_layout.y) {
				break;
			}
			Vec2 min(0, (float)numPrintedLines);
			s_consoleFont->AddVertsForText2D(verts, min, 1.f, 
				Stringf("[%i]", m_output[line].m_messageFrame) + m_output[line].m_message, 
				m_output[line].m_color);
			++numPrintedLines;
			//DebuggerPrintf(Stringf("%d line\n", numPrintedLines).c_str());
		}
		m_renderer->BindTextureViewWithSampler(0, s_consoleFont->GetFontTexture());
		m_renderer->DrawVertexArray(verts.size(), verts);
		m_renderer->BindTextureViewWithSampler(0, nullptr);
		m_renderer->EndCamera(m_consoleCamera);
	}
}

#endif