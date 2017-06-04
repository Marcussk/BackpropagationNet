#include "Arguments.h"

Arguments::Arguments() :
	lg(Logger::getInstance())
{
}

Arguments::~Arguments() {
}

void Arguments::parseArgs(int argc, char* argv[])
{
	if (argc == 2)
	{
		fromFile(argv);
	}
	else{
		fromCLI(argc, argv);
	}


}


void Arguments::fromCLI(int argc, char* argv[])
{
	if (argc < 9) {
		lg->error("Wrong number of arguments");
	}
	hiddenLayerNumber = std::stoi(argv[1]);
	nodesPerLayer = std::stoi(argv[2]);
	learningRate = std::stod(argv[3]);
	trainingEpochs = std::stoi(argv[4]);
	verbosity = std::stoi(argv[5]);
	featureVectorLength = std::stoi(argv[6]);
	dataFilePath = argv[7];
	testFilePath = argv[8];
	lg->out("hiddenLayerNumber: " + std::to_string(hiddenLayerNumber));
	lg->out("nodesPerLayer: " + std::to_string(nodesPerLayer));
	lg->out("learningRate: " + std::to_string(learningRate));
	lg->out("trainingEpochs: " + std::to_string(trainingEpochs));
	lg->out("verbosity: " + std::to_string(verbosity));
	lg->out("featureVectorLength: " + std::to_string(featureVectorLength));
	lg->out("dataFilePath: \"" + dataFilePath + "\"");
	lg->out("testFilePath: \"" + testFilePath + "\"");
}

std::string strip(std::string line) {
	size_t pos;
	std::string token;
	while ((pos = line.find(':')) != std::string::npos) {
		token = line.substr(0, pos);
		line.erase(0, pos + 1);
	}
	return line;
}

void Arguments::fromFile(char* argv[])
{
	std::string filePath = argv[1];
	lg->out("From config file: " + filePath);
	std::ifstream configFile(filePath, std::ifstream::in);
	std::string line;

	if(!configFile)
	{
		lg->error("Couldn't open config file " + filePath);
		exit(1);
	}

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	hiddenLayerNumber =  std::stoi(strip(line));

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	nodesPerLayer =  std::stoi(strip(line));

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	learningRate =  std::stod(strip(line));

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	trainingEpochs =  std::stoi(strip(line));

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	verbosity =  std::stoi(strip(line));

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	featureVectorLength =  std::stoi(strip(line));

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	dataFilePath = strip(line);

	if(!getline(configFile, line)) {
		configFile.close();
		std::cerr << "Malformed configFile";
		exit(7);
	}
	testFilePath = strip(line);

	lg->out("hiddenLayerNumber: " + std::to_string(hiddenLayerNumber));
	lg->out("nodesPerLayer: " + std::to_string(nodesPerLayer));
	lg->out("learningRate: " + std::to_string(learningRate));
	lg->out("trainingEpochs: " + std::to_string(trainingEpochs));
	lg->out("verbosity: " + std::to_string(verbosity));
	lg->out("featureVectorLength: " + std::to_string(featureVectorLength));
	lg->out("dataFilePath: \"" + dataFilePath + "\"");
	lg->out("testFilePath: \"" + testFilePath + "\"");
	configFile.close();
}
