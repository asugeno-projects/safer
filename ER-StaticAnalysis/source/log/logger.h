/**
* @file       logger.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/03 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_LOG_LOGGER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_LOG_LOGGER_H_


#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include "consoleLog.h"

class logger
{
private:
	struct  log_level {
		enum level { error, warn, notice, info, debug };
	};
public:
	logger()
	{
	}
	static ConsoleLog& getConsoleLogInstance(void)
	{
		static class ConsoleLog consoleLog;
		return consoleLog;
	}
	static ConsoleLog& debug()
	{
		return logger::getConsoleLogInstance();
	}

	static ConsoleLog& info()
	{
		return logger::getConsoleLogInstance();
	}

	static ConsoleLog& notice()
	{
		return logger::getConsoleLogInstance();
	}

	static ConsoleLog& error()
	{
		return logger::getConsoleLogInstance();
	}

	static ConsoleLog& warn()
	{
		return logger::getConsoleLogInstance();
	}

	static std::string logPrefix(std::string logLevel)
	{
		std::string buf;
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		buf = "<";
		buf += boost::posix_time::to_simple_string(now);
		buf += "> [";
		buf += logLevel;
		buf += "] ## ";

		return buf;
	}
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_LOG_LOGGER_H_