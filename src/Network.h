#ifndef BPNETWORK_NETWORK_H
#define BPNETWORK_NETWORK_H

#include <vector>
#include <memory>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string>

#include "Arguments.h"
#include "Neuron.h"
#include "Link.h"
#include "Layer.h"
#include "Logger.h"

typedef double Weight;
typedef std::vector<Weight> weightVector;
typedef std::vector<double> DataVector;
typedef std::shared_ptr<DataVector> DataVectorPtr;

/**
 * @brief Class representing abstraction of network along with helper functions on Network scale
 *
 * @param inputFilePath path to input File
 * @param testFilePath path to test File
 */
class Network
{
public:
	Network(std::string inputFilePath, std::string testFilePath);
	~Network();

	void initialize(Arguments* programArgs);
	void assignWeights();
	void train();
	void test();
	double feedForward();
	double computeError(double prediction, double actual);
	void backwardPass(double prediction, double actual);
	double calculateOutputDelta(double prediction, double actual);
	double calculateHiddenDelta(NeuronPtr originNodePtr);
	double sigmoid(double x);
	void getInputData();
	int getTestData();
	void parseInput(std::string line);
	void calculateNetInput(NeuronPtr nodePtr, LayerType layer);
	void calculateActivation(NeuronPtr nodePtr);
	void flipWeights();
	void printStats();

	Arguments* m_programArgs;
	weightVector m_weightVector;
	weightVector m_tempVector;
	Layer m_inputLayer;
	Layer m_outputLayer;
	LayerVector m_hiddenLayers;
	DataVector m_actualInput;
	std::ifstream m_inputFile;
	std::ifstream m_testFile;
	int m_epoch;
	Logger* lg;

	double m_minimumDifference;
	double m_maximumDifference;
	std::vector<int> m_correctVector;
	std::vector<int> m_incorrectVector;
	int m_sampleNumber;
	double m_meanAbsoluteDeviation;
	double m_totalError;

	void preprocess();
	void debugWeights();
	void debugTemps();
	void debugLinks();
	void printdeltaWeights();
};

#endif
