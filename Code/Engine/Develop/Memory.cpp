#include "Engine/Develop/Memory.hpp"
#include "Engine/Develop/Log.hpp"
#include <mutex>
#include <map>
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp.lib")

std::atomic<size_t> g_allocationCounter = 0;
std::atomic<size_t> g_allocatedSize = 0;
struct __Tracker
{
	using __TrackerMap = std::map<
		void*, MemoryAllocationInfo
		, std::less<void*>
		, EngineUntrackedAllocator<std::pair<void* const, MemoryAllocationInfo>>
	>;
	static std::mutex& GetLock()
	{
		static std::mutex _trackerLock;
		return _trackerLock;
	}
	static __TrackerMap& GetMap()
	{
		static __TrackerMap _map;
		return _map;
	}
};

////////////////////////////////
std::string GetByteSizeString(size_t size)
{
	constexpr size_t Ki = 1024;
	constexpr size_t Mi = 1024 * 1024;
	constexpr size_t Gi = 1024 * 1024 * 1024;
	
	if (size > Gi) {
		const float fsize = static_cast<float>(size) / Gi;
		return Stringf("%g GiB", fsize);
	}
	if (size > Mi) {
		const float fsize = static_cast<float>(size) / Mi;
		return Stringf("%g MiB", fsize);
	}
	if (size > Ki) {
		const float fsize = static_cast<float>(size) / Ki;
		return Stringf("%g KiB", fsize);
	}
	return Stringf("%d Byte", size);
}

////////////////////////////////
size_t GetLiveAllocationCount()
{
	return g_allocationCounter;
}

////////////////////////////////
size_t GetLiveAllocationSize()
{
	return g_allocatedSize;
}
using UntrackedString = std::basic_string<char, std::char_traits<char>, EngineUntrackedAllocator<char>>;
void __print_grouped_details()
{
	struct _LogEntry {
		size_t size;
		int allocs;
		UntrackedString str_callstack;
	};
	std::map<int, _LogEntry, std::less<int>, EngineUntrackedAllocator<std::pair<const int, _LogEntry>>> logmap;
	for (auto& each : __Tracker::GetMap()) {
		if (logmap.find(each.second.m_callstack.m_hash) == logmap.end()) {
			_LogEntry data = _LogEntry();
			data.size = each.second.m_sizeByte;
			data.allocs = 1;
			auto cs = CallstackToString(each.second.m_callstack);
			for (auto& eachFrame : cs) {
				UntrackedString s = "\t";
				s.append(eachFrame.c_str());
				s.append("\n");
				data.str_callstack.append(s.c_str());
			}
			logmap[each.second.m_callstack.m_hash] = data;
		} else {
			logmap[each.second.m_callstack.m_hash].allocs += 1;
			logmap[each.second.m_callstack.m_hash].size += each.second.m_sizeByte;
		}
	}
	std::vector<std::pair<size_t, _LogEntry>, EngineUntrackedAllocator<std::pair<size_t, _LogEntry>>> lk;
	for (auto& each : logmap) {
		lk.push_back(std::make_pair(each.second.size, each.second));
	}
	logmap.clear();

	auto cmp = [=](const std::pair<size_t, _LogEntry>& a, const std::pair<size_t, _LogEntry>& b) -> bool {
		return a.first > b.first;
	};
	std::sort(std::begin(lk), std::end(lk), cmp);

	for (auto& each : lk) {
		DebuggerPrintf(">>> %s from %d allocs <<<\n", GetByteSizeString(each.second.size).c_str(), each.second.allocs);
		DebuggerPrintf("%s\n", each.second.str_callstack.c_str());
	}
}

////////////////////////////////
void LogLiveAllocations()
{
#if (MEM_TRACKING >= MEM_TRACKING_COUNTING)
	DebuggerPrintf("Total Allocations: %d with %s\n", GetLiveAllocationCount(),
		GetByteSizeString(GetLiveAllocationSize()).c_str());
#if (MEM_TRACKING >= MEM_TRACKING_VERBOSE)
	DebuggerPrintf("===Details===\n");
	__print_grouped_details();
	DebuggerPrintf("===  End  ===\n");
#endif
#else
	DebuggerPrintf("Memory tracking disabled\n");
#endif
}

////////////////////////////////
void* UntrackedAlloc(size_t size)
{
	return ::malloc(size);
}

////////////////////////////////
void UntrackedFree(void* p)
{
	::free(p);
}

////////////////////////////////
void* TrackedAlloc(size_t size)
{
#if (MEM_TRACKING > MEM_TRACKING_DISABLE)
	void* p = UntrackedAlloc(size);
	TrackAllocation(p, size);
	return p;
#else
	return UntrackedAlloc(size);
#endif
}

////////////////////////////////
void TrackedFree(void* p)
{
#if (MEM_TRACKING > MEM_TRACKING_DISABLE)
	UntrackAllocation(p);
	UntrackedFree(p);
#else
	return UntrackedFree(p);
#endif
}

////////////////////////////////
void TrackAllocation(void* p, size_t size)
{
#if (MEM_TRACKING > MEM_TRACKING_DISABLE)
	++g_allocationCounter;
#if (MEM_TRACKING > MEM_TRACKING_COUNTING)
	g_allocatedSize += size;
	MemoryAllocationInfo info;
	info.m_originalPointer = p;
	info.m_sizeByte = size;
	GetCallstack(info.m_callstack, 4);
	__Tracker::GetLock().lock();
	__Tracker::GetMap()[p] = info;
	__Tracker::GetLock().unlock();
#endif
#endif
}

////////////////////////////////
void UntrackAllocation(void* p)
{
#if (MEM_TRACKING > MEM_TRACKING_DISABLE)
	--g_allocationCounter;
#if (MEM_TRACKING > MEM_TRACKING_COUNTING)
	__Tracker::GetLock().lock();
	auto found = __Tracker::GetMap().find(p);
	if (found != __Tracker::GetMap().end())
	{
		g_allocatedSize -= __Tracker::GetMap()[p].m_sizeByte;
		__Tracker::GetMap().erase(p);
	}
	__Tracker::GetLock().unlock();
#endif
#endif
}

////////////////////////////////
void* operator new(size_t size)
{
	return TrackedAlloc(size);
}

////////////////////////////////
void operator delete(void* p)
{
	TrackedFree(p);
}