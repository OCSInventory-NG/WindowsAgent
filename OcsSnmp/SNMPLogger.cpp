#include "SNMPLogger.h"

#include <sstream>
#include <iostream>

int SNMPLogger::_level = 0;

void SNMPLogger::Log(int type, std::string msg, unsigned long number, const char * file, const char * date, const char * time) {
	std::stringstream ss;
	switch(type) {
		case SNMP_LOG_INFO_TYPE:
			ss << "INFO SNMP";
			break;
		case SNMP_LOG_WARNING_TYPE:
			ss << "WARNING SNMP";
			break;
		// case SNMP_LOG_ERROR_TYPE:
		default:
			ss << "ERROR SNMP";
			break;
	}
	if(type != SNMP_LOG_INFO_TYPE) {
		ss << number << ": " << msg;
	} else {
		ss << ": " << msg;
	}
	if(file) {
		ss << " at \"file://" << file << "\"";
	}
	if(date) {
		ss << " compiled on " << date << " ";
	}
	if(date) {
		ss << " " << time;
	}
	if(_level >= type) {
		std::cout << "\n" << ss.str() << "\n";
	}
}

