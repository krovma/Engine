#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
struct ScopeProfiler
{
	uint64 m_hpc;
	const char* m_scopeName;
	ScopeProfiler(const char* scopeName)
		:m_scopeName(scopeName)
	{
		m_hpc = (GetCurrentHPC());
	}
	~ScopeProfiler()
	{
		uint64 currentHPC = GetCurrentHPC();
		double timeElapse = HPCToSeconds(currentHPC - m_hpc);
		DebuggerPrintf("Scope %s Used %fs\n", m_scopeName, timeElapse);
	}

};

#define ___CONCAT___(a, b) a##b
#define ___CONCAT(a, b) ___CONCAT___(a, b)
#define __PROFILE_SCOPE_IMPL(scope_name, line)\
	ScopeProfiler ___CONCAT(__scope_profiler_, line)(scope_name);
#define PROFILE_SCOPE(scope_name) __PROFILE_SCOPE_IMPL(scope_name, __LINE__)
