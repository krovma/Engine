#pragma once

class Semaphore
{
public:
	Semaphore(int initial = 1, int max = 1);
	~Semaphore();
	void BlockedAcquire();
	bool TryAcquire();
	void Release(int count = 1);
private:
	void* m_handle = nullptr;
};