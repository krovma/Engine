#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Develop/Callstack.hpp"
#include <atomic>

extern std::atomic<size_t> g_allocationCounter;

void* UntrackedAlloc(size_t size);
void  UntrackedFree(void* p);
void* TrackedAlloc(size_t size);
void  TrackedFree(void* p);
void TrackAllocation(void* p, size_t size);
void UntrackAllocation(void* p);

void* operator new(size_t size);
void operator delete(void* p);


std::string GetByteSizeString(size_t size);

size_t GetLiveAllocationCount();
size_t GetLiveAllocationSize();
void LogLiveAllocations();

struct MemoryAllocationInfo
{
	void* m_originalPointer;
	size_t m_sizeByte;
	Callstack m_callstack;
};

template <typename T>
inline T* Alloc()
{
#if (MEM_TRACKING > MEM_TRACKING_DISABLE)
	return static_cast<T*>(TrackedAlloc(sizeof(T)));
#else
	return static_cast<T*>(UntrackedAlloc(sizeof(T)));
#endif
}

inline void Free(void* p)
{
#if (MEM_TRACKING>MEM_TRACKING_DISABLE)
	TrackedFree(p);
#else
	UntrackedFree(p);
#endif
}

template <typename T>
struct EngineUntrackedAllocator
{
	EngineUntrackedAllocator() = default;

	template <typename U>
	EngineUntrackedAllocator(const EngineUntrackedAllocator<U>&) noexcept {}

	typedef T				value_type;
	typedef size_t			size_type;
	typedef T*				pointer;
	typedef const T*		const_pointer;
	typedef T&				reference;
	typedef const T&		const_reference;
	typedef std::ptrdiff_t	difference_type;

	typedef std::true_type	propagate_on_container_move_assignment;
	typedef std::true_type	is_always_equal;

	pointer allocate(size_type count)
	{
		return static_cast<pointer>(::malloc(count * sizeof(value_type)));
	}

	void deallocate(pointer p, size_type size)
	{
		UNUSED(size);
		::free(p);
	}
};

template<typename T1, typename T2>
constexpr bool operator==(const EngineUntrackedAllocator<T1>& a, const EngineUntrackedAllocator<T2>& b)
{
	return true;
}

template<typename T1, typename T2>
constexpr bool operator!=(const EngineUntrackedAllocator<T1>& a, const EngineUntrackedAllocator<T2>& b)
{
	return false;
}