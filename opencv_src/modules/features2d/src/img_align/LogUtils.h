
#ifndef IMGALIGN_IMGALIGN_LOGUTILSH
#define IMGALIGN_IMGALIGN_LOGUTILSH

#include <ostream>
#include <iostream>
#include <string>

#include "TimeUtils.h"
#include "CommonTypes.h"

namespace imgalign
{

class Matcher;

class LogUtils{
public:
	static void doLog(const Matcher& matcher, const std::string& inFileNameImage1 = "", const std::string& inFileNameImage2 = "");
	static void doLogMatches(TConstMatchInfos& matchInfos, int maxMatches);
	// static void logMatchesDistance(TConstMatchInfos& matchInfos);
	static std::ostream& getLog();
	static std::ostream& getLogUserInfo();

	static void logMat(std::string name, TConstMat &mat);

	static bool isDebug;
};


class FuncLogTime{
public:

	FuncLogTime(std::string inStr, long& outTime);
	FuncLogTime(std::string inStr);
	~FuncLogTime();

	std::string str;
	long& time;
	long sTime;
};

class FuncLog{
public:

	FuncLog(std::string inStr);
	~FuncLog();

	std::string str;
};

#define FUNCLOG(x) FuncLog myFuncLog(x)
#define FUNCLOGTIMEL(x) FuncLogTime myFuncLogTime(x)
#define FUNCLOGTIME(x, t) FuncLogTime myFuncLogTime(x, t)

} // imgalign

#endif