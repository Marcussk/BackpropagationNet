#include <stdio.h>

#include "Arguments.h"
#include "Logger.h"
#include "Network.h"

Logger* Logger::m_loggerInstance = 0;

int main(int argc, char* argv[])
{
	Logger* lg = Logger::getInstance();
	Arguments Args;
	Args.parseArgs(argc, argv);
	lg->setVerbosity(Args.verbosity);
	Network bpNetwork(Args.dataFilePath, Args.testFilePath);
	bpNetwork.initialize(&Args);
	//bpNetwork.preprocess();
	//bpNetwork.debugLinks();
	bpNetwork.train();
	bpNetwork.test();
	return 0;
}
