
#include "TimeUtils.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <ctime>
#endif

// uint64_t
// nanoseconds(){

// 	#if _MSC_VER
	
// 		LARGE_INTEGER ticks;
// 		LARGE_INTEGER ticksPerSecond;
	
// 		QueryPerformanceCounter(&ticks);
// 		QueryPerformanceFrequency(&ticksPerSecond);
// 		if(ticks.QuadPart != 0 && ticksPerSecond.QuadPart != 0){
		
// 			long double seconds = ticks.QuadPart / (double)ticksPerSecond.QuadPart;
// 			return static_cast<uint64_t>(1000000000 * seconds);
// 		}
// 		return 1000000 * ::GetTickCount();
	
	
// 	#else

// 		timespec t;
// 		if(/*(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t) == 0) || */(clock_gettime(CLOCK_REALTIME, &t) == 0)){
// 			return (((double)t.tv_sec * (double)1000000000) + t.tv_nsec);
// 		}
// 		else{
// 			return ((double)std::clock() / (double)CLOCKS_PER_SEC) * 1000000000.0;
// 		}
	
// 	#endif
	
// }
uint32_t
microseconds(){

	#if _MSC_VER
	
		LARGE_INTEGER ticks;
		LARGE_INTEGER ticksPerSecond;
	
		QueryPerformanceCounter(&ticks);
		QueryPerformanceFrequency(&ticksPerSecond);
		if(ticks.QuadPart != 0 && ticksPerSecond.QuadPart != 0){
		
			long double seconds = ticks.QuadPart / (double)ticksPerSecond.QuadPart;
			return static_cast<uint64_t>(1000000000 * seconds);
		}
		return 1000000 * ::GetTickCount();
	
	
	#else

		// timespec t;
		// if(/*(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t) == 0) || */(clock_gettime(CLOCK_REALTIME, &t) == 0)){
		// 	return (((double)t.tv_sec * (double)1000000000) + t.tv_nsec);
		// }
		// else{
			return ((double)std::clock() / (double)CLOCKS_PER_SEC) * 1000000.0;
		// }
	
	#endif
	
}

uint32_t
milliseconds()
{
#if _MSC_VER
	LARGE_INTEGER ticks;
	LARGE_INTEGER ticksPerSecond;
	
	QueryPerformanceCounter(&ticks);
	QueryPerformanceFrequency(&ticksPerSecond);
	if(ticks.QuadPart != 0 && ticksPerSecond.QuadPart){
		
		return static_cast<uint32_t>(1000 * ticks.QuadPart / ticksPerSecond.QuadPart);
	}
	return ::GetTickCount();
	
#else
	return (static_cast<uint32_t>((double)microseconds() * (1.0 / 1000.0) + 0.5));
#endif
}