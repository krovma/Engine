#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

////////////////////////////////
InputSystem::InputSystem()
{
}

////////////////////////////////
InputSystem::~InputSystem()
{
}

////////////////////////////////
void InputSystem::StartUp()
{
	for (int i = 0; i < MAX_XBOX_CONTROLLERS; ++i) {
		_controllers[i].update();
	}
}

////////////////////////////////
void InputSystem::BeginFrame()
{
	for (int i = 0; i < MAX_XBOX_CONTROLLERS; ++i) {
		_controllers[i].update();
	}
}

////////////////////////////////
void InputSystem::EndFrame()
{

}

////////////////////////////////
void InputSystem::Shutdown()
{

}

////////////////////////////////
const XboxController &InputSystem::GetXboxController(int controllerID) const
{
	return _controllers[controllerID];
}
