#pragma once 

#include <string>

#define SNMP_LOG_INFO_TYPE			3
#define SNMP_LOG_WARNING_TYPE		2
#define SNMP_LOG_ERROR_TYPE			1

#define SNMP_LOG_INFO(msg)			SNMPLogger::Log(SNMP_LOG_INFO_TYPE, msg, 0, __FILE__, __DATE__, __TIME__)
#define SNMP_LOG_WARNING(msg, n)	SNMPLogger::Log(SNMP_LOG_WARNING_TYPE, msg, n, __FILE__, __DATE__, __TIME__)
#define SNMP_LOG_ERROR(msg, n)		SNMPLogger::Log(SNMP_LOG_ERROR_TYPE, msg, n, __FILE__, __DATE__, __TIME__)

class SNMPLogger {
protected:
	static int _level;
public:	
	static void Log(int type, std::string msg, unsigned long number, const char * file = NULL, const char * date = NULL, const char * time = NULL); 
	static setLevel(int lev) { _level = lev; }
};


