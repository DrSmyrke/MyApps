#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

struct Config{
	bool logging=false;
	bool verbose=false;
	unsigned int port=7000;
	QString fileAccess;
	QString logsPath="logs";
};

namespace data{
	extern std::vector<QString> access;
}
namespace cfg {
	extern Config conf;
}

#endif // GLOBAL_H
