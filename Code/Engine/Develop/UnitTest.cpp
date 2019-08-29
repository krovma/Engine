#include "Engine/Develop/UnitTest.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <cstring>
struct UnitTest
{
	const char* name;
	const char* filter;
	UnitTestCallback callback;
	inline bool operator()() { return callback(); }
};

static const int _MAX_UNIT_TESTS = 2048;
static UnitTest _allUnitTest[_MAX_UNIT_TESTS];
static int _unitTestCount = 0;

////////////////////////////////
UnitTestRegistrar::UnitTestRegistrar(const char* name, UnitTestCallback cb, const char* filter)
{
	if (_unitTestCount >= _MAX_UNIT_TESTS) {
		__debugbreak();
	} else {
		UnitTest& test = _allUnitTest[_unitTestCount];
		++_unitTestCount;

		test.name = name;
		test.filter = filter;
		test.callback = cb;
	}
}

////////////////////////////////
void RunUnitTest(const std::string& filter)
{
	for (int i = 0; i < _unitTestCount; ++i) {
		UnitTest& test = _allUnitTest[i];
		if ((filter.empty() || (test.filter == filter)) && !test()) {
			ERROR_AND_DIE(Stringf("Unit test %s FAILED!", test.name).c_str());
		}
	}
}
