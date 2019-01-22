#pragma once
#include <vector>
#include <string>
#include <map>
#include <functional>
#include "Engine/Core/NamedStrings.hpp"

using EventParam = NamedStrings;
using EventCallback = std::function<bool(EventParam& param)>;

class EventSubscribtion
{
	friend class EventSystem;
public:
	bool operator==(const EventSubscribtion& compareWith) const;
	const EventSubscribtion& operator=(const EventSubscribtion& copy);
private:
	EventSubscribtion(EventCallback callback);
private:
	EventCallback m_callback = nullptr;
};

using EventSubscribtionList = std::vector<EventSubscribtion>;

class EventSystem
{
public:
	EventSystem() = default;
	~EventSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndGrame();

	void SubscribeEventCallback(const std::string& event, EventCallback callback);
	void UnsubscribeEventCallback(const std::string& event, EventCallback callback);
	int Trigger(const std::string& event);
	int Trigger(const std::string& event, EventParam& param);
	int GetNumSubscribers(const std::string event) const;
	
private:
	std::map<std::string, EventSubscribtionList> m_events;
};

extern EventSystem* g_Event;