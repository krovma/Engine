#pragma once
#include <string>

using UnitTestCallback = bool(*)();

struct UnitTestRegistrar
{
	UnitTestRegistrar(const char* name, UnitTestCallback cb, const char* filter);
};

void RunUnitTest(const std::string& filter);

#define UNIT_TEST(name, filter) \
	static bool ___unit_test___##name();		\
	static UnitTestRegistrar ___unit_test_obj___##name(#name, ___unit_test___##name, filter);		\
	static bool ___unit_test___##name()
#define ALL_UNIT_TEST "" // DON'T CHANGE IT
