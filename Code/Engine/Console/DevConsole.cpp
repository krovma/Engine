#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

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
STATIC bool DevConsole::Command_ClearScreen(EventParam& param)
{
	UNUSED(param);
	g_theConsole->Clear();
	return true;
}

////////////////////////////////
bool DevConsole::Command_Exit(EventParam& param)
{
	UNUSED(param);
	g_theConsole->SetConsoleMode(CONSOLE_OFF);
	return true;
}

////////////////////////////////
bool DevConsole::Command_Help(EventParam& param)
{
	UNUSED(param);
	std::vector<std::string> allEvent = g_Event->GetAllEventNames();
	for (auto eachEvent : allEvent) {
		g_theConsole->Print(eachEvent);
	}
	return true;
}

////////////////////////////////
STATIC bool DevConsole::Command_ClearHistory(EventParam& param)
{
	UNUSED(param);
	g_theConsole->ClearHistroy();
	return true;
}

////////////////////////////////
DevConsole::DevConsole(RenderContext* renderer, int line, int column)
	: m_renderer(renderer)
	, m_layout(column, line)
	, m_consoleRect(0, 0,
		s_consoleFont->GetGlyphAspect('m') * column, (float) line)
	, m_consoleCamera(m_consoleRect.Min, m_consoleRect.Max, true)
{
}

////////////////////////////////
void DevConsole::Startup()
{
	m_frameFromStartup = 0;
	g_Event->SubscribeEventCallback("test", DevConsole::Command_Test);
	g_Event->SubscribeEventCallback("cls", DevConsole::Command_ClearScreen);
	g_Event->SubscribeEventCallback("exit", DevConsole::Command_Exit);
	g_Event->SubscribeEventCallback("help", DevConsole::Command_Help);
	g_Event->SubscribeEventCallback("clearhistory", DevConsole::Command_ClearHistory);
}

////////////////////////////////
void DevConsole::BeginFrame()
{

}

////////////////////////////////
void DevConsole::Update(float deltaSecond)
{
	static float upTime = 0.f;
	upTime += deltaSecond;
	if (upTime > 0.3f) {
		if (m_currentCaretColor == Rgba::TRANSPARENT_WHITE) {
			m_currentCaretColor = Rgba::WHITE;
		} else {
			m_currentCaretColor = Rgba::TRANSPARENT_WHITE;
		}
		upTime = 0;
	}
	m_isOpenLastFrame = (m_mode == CONSOLE_PASSIVE);
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
void DevConsole::Input(char input)
{
	if (!m_isOpenLastFrame)
		return;
	if (std::isprint(input)) {
		IntVec2 sliceBeforeCaret(0, m_caretPos);
		IntVec2 sliceAfterCaret(
			m_caretPos,
			(int)m_inputBuffer.length()
		);
		m_inputBuffer = m_inputBuffer.substr(sliceBeforeCaret.x, sliceBeforeCaret.y)
			+ input + m_inputBuffer.substr(sliceAfterCaret.x, sliceAfterCaret.y);
		++m_caretPos;
	}
}

////////////////////////////////
void DevConsole::KeyPress(ConsoleKeys key)
{
	switch (key) {
	case CONSOLE_ESC: {
		if (m_inputBuffer.empty()) {
			SetConsoleMode(CONSOLE_OFF);
		} else {
			m_inputBuffer.clear();
			m_caretPos = 0;
		}
		}
	case CONSOLE_ENTER: {
		Print(m_inputBuffer, Rgba::GRAY);
		if (m_history.size() > 0) {
			if (m_history[m_history.size() - 1] != m_inputBuffer
			&&	m_history[0] != m_inputBuffer) {
				m_history.push_back(m_inputBuffer);
			}
		} else {
			m_history.push_back(m_inputBuffer);
		}
		RunCommandString(m_inputBuffer);
		m_inputBuffer.clear();
		m_caretPos = 0;
		m_historyCursor = 0;
		break;
		}
	case CONSOLE_BACKSPACE: {
		if (m_inputBuffer.empty() || m_caretPos == 0)
			break;
		IntVec2 sliceBeforeCaret(0, m_caretPos - 1);
		IntVec2 sliceAfterCaret(
			Clamp(m_caretPos, 0, (int)m_inputBuffer.length()),
			(int)m_inputBuffer.length()
		);
		m_inputBuffer = m_inputBuffer.substr(sliceBeforeCaret.x, sliceBeforeCaret.y)
			+ m_inputBuffer.substr(sliceAfterCaret.x, sliceAfterCaret.y);
		--m_caretPos;
		break;
		}
	case CONSOLE_DELETE: {
		IntVec2 sliceBeforeCaret(0, m_caretPos);
		IntVec2 sliceAfterCaret(
			Clamp(m_caretPos + 1, 0, (int)m_inputBuffer.length()),
			(int)m_inputBuffer.length()
		);
		m_inputBuffer = m_inputBuffer.substr(sliceBeforeCaret.x, sliceBeforeCaret.y)
			+ m_inputBuffer.substr(sliceAfterCaret.x, sliceAfterCaret.y);
		break;
		}
	case CONSOLE_LEFT: {
		--m_caretPos;
		m_caretPos = Clamp(m_caretPos, 0, (int)m_inputBuffer.length());
		break;
	}
	case CONSOLE_RIGHT: {
		++m_caretPos;
		m_caretPos = Clamp(m_caretPos, 0, (int)m_inputBuffer.length());
		break;
	}
	case CONSOLE_UP: {
		--m_historyCursor;
		if (m_historyCursor < 0) {
			m_historyCursor = (int)m_history.size() - 1;
		}
		if (m_historyCursor >= 0) {
			m_inputBuffer = m_history[m_historyCursor];
			m_caretPos = (int)m_inputBuffer.size();
		}
		break;
	}
	case CONSOLE_DOWN: {
		++m_historyCursor;
		if (m_historyCursor > m_history.size()) {
			m_historyCursor = 0;
		}
		if (m_historyCursor < m_history.size()) {
			m_inputBuffer = m_history[m_historyCursor];
			m_caretPos = (int)m_inputBuffer.size();
		}
		break;
	}

	}
}

////////////////////////////////
void DevConsole::Clear()
{
	m_output.clear();
}

////////////////////////////////
void DevConsole::RunCommandString(std::string cmd)
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
			int numTriggered = g_Event->Trigger(cmdName, args);
			if (numTriggered <= 0) {
				Print(Stringf("%s is not a valid command", cmdName.c_str()), Rgba::RED);
			}
		}
	}
}

////////////////////////////////
void DevConsole::ClearHistroy()
{
	m_history.clear();
}

////////////////////////////////
void DevConsole::RenderConsole() 
{
	if (m_mode == CONSOLE_OFF) {
		return;
	}

	std::vector<Vertex_PCU> verts;
	if (m_mode == CONSOLE_PASSIVE) {
		RenderTargetView* renderTarget = m_renderer->GetFrameColorTarget();
		m_consoleCamera.SetRenderTarget(renderTarget);
		m_renderer->BeginCamera(m_consoleCamera);

		AddVerticesOfAABB2D(verts, m_consoleRect, Rgba(0,0,0, 0.6f));
		m_renderer->BindTextureViewWithSampler(0, nullptr);
		m_renderer->DrawVertexArray(verts.size(), verts);
		verts.clear();
		int numPrintedLines = 0;
		for (int line = (int) m_output.size() - 1; line >= 0; --line) {
			if (numPrintedLines >= m_layout.y) {
				break;
			}
			Vec2 min(0, (float)numPrintedLines + 1);
			s_consoleFont->AddVertsForText2D(verts, min, 1.f, 
				m_output[line].m_message, 
				m_output[line].m_color);
			++numPrintedLines;
			//DebuggerPrintf(Stringf("%d line\n", numPrintedLines).c_str());
		}
		Vec2 min(0, 0);
		s_consoleFont->AddVertsForText2D(verts, min, 1.f,
			"> " + m_inputBuffer
		);
		m_renderer->BindTextureViewWithSampler(0, s_consoleFont->GetFontTexture());
		m_renderer->DrawVertexArray(verts.size(), verts);
		verts.clear();
		m_renderer->BindTextureViewWithSampler(0, nullptr);
		AddVerticesOfAABB2D(verts,
			AABB2(m_caretPos + 2.f, 0, m_caretPos + 2.5f, 1),
			m_currentCaretColor
		);
		m_renderer->DrawVertexArray(verts.size(), verts);
		m_renderer->BindTextureViewWithSampler(0, nullptr);
		m_renderer->EndCamera(m_consoleCamera);
	}
}

#endif