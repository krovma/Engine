#pragma once
#include <vector>

class UIWidget;

class UIRadioGroup
{
public:
	void Activate(UIWidget*);
private:
	std::vector<UIWidget*> m_group;
	UIWidget* m_current = nullptr;
};