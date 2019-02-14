#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Event/EventSystem.hpp"

//////////////////////////////////////////////////////////////////////////
//STATIC std::map<std::string, EventSubscribers> EventSystem::s_events;
EventSystem* g_Event = new EventSystem();
////////////////////////////////
EventSystem::~EventSystem()
{
	Shutdown();
}

////////////////////////////////
void EventSystem::Startup()
{

}

////////////////////////////////
void EventSystem::Shutdown()
{
	//Currently nothing
	//All subscription has an auto/static storage duration
}

////////////////////////////////
void EventSystem::BeginFrame()
{
	Trigger("BeginFrame");
}

////////////////////////////////
void EventSystem::EndGrame()
{
	Trigger("EndFrame");
}

////////////////////////////////
void EventSystem::SubscribeEventCallback(const std::string& event, EventCallback callback)
{
	m_events[event].push_back(callback);
}

////////////////////////////////
void EventSystem::UnsubscribeEventCallback(const std::string& event, EventCallback callback)
{
	auto eventFound = m_events.find(event);
	if (eventFound != m_events.end()) {
		EventSubscribtionList subscribers = eventFound->second;
		for (auto subscribtionIter = subscribers.begin(); subscribtionIter != subscribers.end(); ++subscribtionIter) {
			if (*subscribtionIter == callback) {
				subscribers.erase(subscribtionIter);
				break;
			}
		}
	}
}

////////////////////////////////
int EventSystem::Trigger(const std::string& event)
{
	int eventTriggered = 0;
	EventParam emptyParam;
	auto eventFound = m_events.find(event);
	if (eventFound != m_events.end()) {
		EventSubscribtionList subscribers = eventFound->second;
		for (auto& eachSubscribtion : subscribers) {
			bool isConsumed = eachSubscribtion.m_callback(emptyParam);
			++eventTriggered;
			if (isConsumed) {
				break;
			}
		}
	}
	return eventTriggered;
}

////////////////////////////////
int EventSystem::Trigger(const std::string& event, EventParam& param)
{
	int eventTriggered = 0;
	auto eventFound = m_events.find(event);
	if (eventFound != m_events.end()) {
		EventSubscribtionList subscribers = eventFound->second;
		for (auto& eachSubscribtion : subscribers) {
			bool isConsumed = eachSubscribtion.m_callback(param);
			++eventTriggered;
			if (isConsumed) {
				break;
			}
		}
	}
	return eventTriggered;

}

////////////////////////////////
std::vector<std::string> EventSystem::GetAllEventNames() const
{
	std::vector<std::string> allKey;
	allKey.reserve(m_events.size());
	for (auto eachEvent : m_events) {
		allKey.push_back(eachEvent.first);
	}
	return std::move(allKey);
}

////////////////////////////////
EventSubscribtion::EventSubscribtion(EventCallback callback)
	:m_callback(callback)
{
}

////////////////////////////////
const EventSubscribtion& EventSubscribtion::operator=(const EventSubscribtion& copy)
{
	m_callback = copy.m_callback;
	return *this;
}

////////////////////////////////
bool EventSubscribtion::operator==(const EventSubscribtion& compareWith) const
{
	return m_callback.target<bool(*)(EventParam&)>() == compareWith.m_callback.target<bool(*)(EventParam&)>();
}
