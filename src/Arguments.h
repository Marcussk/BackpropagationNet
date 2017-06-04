#ifndef BPNETWORK_ARGUMENTS_H
#define BPNETWORK_ARGUMENTS_H

#include <string>
#include <fstream>

#include "Logger.h"

/**
 * @brief Quick and dirty Argument parser from CLI or File
 */
class Arguments
{
public:
	Arguments();
	~Arguments();
	void parseArgs(int argc, char* argv[]);

	int hiddenLayerNumber;	//1
	int nodesPerLayer;		//2
	double learningRate;	//3
	int trainingEpochs;		//4
	int verbosity;			//5
	int featureVectorLength; //6
	std::string dataFilePath; //7
	std::string testFilePath; //8

private:
	void fromCLI(int argc, char* argv[]);
	void fromFile(char* argv[]);
	Logger* lg;
};

#endif
