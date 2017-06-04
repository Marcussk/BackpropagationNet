#include "Network.h"

Network::Network(std::string inputFilePath, std::string testFilePath) :
	m_inputLayer(LayerType::input),
	m_outputLayer(LayerType::output),
	m_inputFile(inputFilePath, std::ifstream::in),
	m_testFile(testFilePath, std::ifstream::in),
	m_epoch(0),
	lg(Logger::getInstance()),
	m_minimumDifference(1000.0),
	m_maximumDifference(-1000.0),
	m_sampleNumber(0),
	m_meanAbsoluteDeviation(0)
{
	if(!m_inputFile)
	{
		lg->error("Couldn't open input file \"" + inputFilePath + "\"");
		exit(1);
	}

		if(!m_testFile)
	{
		lg->error("Couldn't open test file \"" + testFilePath + "\"");
		exit(1);
	}
}

Network::~Network() {
}

void Network::initialize(Arguments* programArgs) {
	lg->trace("Network::initialize");
	m_programArgs = programArgs;
	int windex = 0;

	// Fill input layer with Neurons
	for (int i = 0; i < m_programArgs->featureVectorLength; ++i)
	{
		m_inputLayer.add(NeuronPtr(new Neuron(i,-1)));
	}
	// Create hidden layers
	for (int j = 0; j < m_programArgs->hiddenLayerNumber; ++j)
	{
		m_hiddenLayers.push_back(LayerPtr(new Layer(LayerType::hidden)));
		// Fill hidden layer with Neurons
		for (int k = 0; k < m_programArgs->nodesPerLayer; ++k)
		{
			m_hiddenLayers[j]->add(NeuronPtr(new Neuron(k, j)));
		}
	}
	// Create output layer
	m_outputLayer.add(NeuronPtr(new Neuron(0, -1)));

	// Link input layer
	for (int l = 0; l < m_programArgs->featureVectorLength; ++l)
	{
		for (int m = 0; m < m_programArgs->nodesPerLayer; ++m)
		{
			LinkPtr linkPtr = std::make_shared<Link>(m, windex);
			windex++;
			m_inputLayer[l]->outgoingLinks.push_back(linkPtr);
		}
	}
	// Link hidden layer
	for (int n = 0; n < (m_programArgs->hiddenLayerNumber-1); ++n)
	{
		for (int o = 0; o < m_programArgs->nodesPerLayer; ++o)
		{
			for (int p = 0; p < m_programArgs->nodesPerLayer; ++p)
			{
				LinkPtr linkPtr = std::make_shared<Link>(p, windex);
				windex++;
				(*m_hiddenLayers[n])[o]->outgoingLinks.push_back(linkPtr);
			}
		}
	}
	// Link last hidden to output layer
	for (int r = 0; r < m_programArgs->nodesPerLayer; ++r)
	{
		LinkPtr linkPtr = std::make_shared<Link>(0, windex);
		windex++;
		(*m_hiddenLayers[m_programArgs->hiddenLayerNumber-1])[r]->outgoingLinks.push_back(linkPtr);
	}

	for (int i = 0; i <= 10; ++i)
	{
		m_correctVector.push_back(0);
		m_incorrectVector.push_back(1);
	}
}

void Network::train() {
	double Error = 1;
	lg->trace("Network::train");
	assignWeights();

	while(m_epoch < m_programArgs->trainingEpochs) {
		getInputData();

		// Forward pass
		double output = feedForward();
		double target = m_actualInput[m_actualInput.size()-1];
		Error = computeError(output, target);
		lg->debug("Epoch: " + std::to_string(m_epoch)
			+ " Output: " + std::to_string(output)
			+ " Target: " + std::to_string(target)
			+ " Error: " + std::to_string(Error)
			+ " Differerence: " + std::to_string(output-target)
			);

		backwardPass(output, target);
		debugWeights();
		printdeltaWeights();
		flipWeights();
	}
	m_inputFile.close();
	lg->out("Finished training after " + std::to_string(m_epoch) + "epochs");
}

void Network::assignWeights() {
	srand (time(NULL));
	double randomWeight;
	int weightVectorLength = (m_programArgs->featureVectorLength * m_programArgs->nodesPerLayer) +
		((m_programArgs->hiddenLayerNumber - 1) * m_programArgs->nodesPerLayer * m_programArgs->nodesPerLayer) +
		(m_programArgs->nodesPerLayer * 1);
	lg->verbose("weightVectorLength: " + std::to_string(weightVectorLength));

	for (int i = 0; i < weightVectorLength; i++) {
		randomWeight = ((double)rand() / ((double)RAND_MAX + 1))-0.5;
		lg->verbose("Initialized weight: " + std::to_string(randomWeight));
		m_weightVector.push_back(randomWeight);
	}
	for (int i = 0; i < weightVectorLength; i++) {
		m_tempVector.push_back(1.0);
	}
}

double Network::feedForward() {
	NeuronPtr currentNode;
	// For input layer
	for (int i = 0; i < m_inputLayer.size(); ++i)
	{
		//Assign current data to input of network
		m_inputLayer[i]->setActivation(m_actualInput[i]);
		lg->verbose("Input layer " + std::to_string(i) + " Net Input: " + std::to_string(m_inputLayer[i]->activation()));
	}

	// For each hidden layer
	for (int i = 0; i < m_programArgs->hiddenLayerNumber; ++i)
	{
		// For each node in layer
		for (int j = 0; j < m_programArgs->nodesPerLayer; ++j)
		{
			currentNode = (*m_hiddenLayers[i])[j];
			calculateNetInput(currentNode, LayerType::hidden);
			calculateActivation(currentNode);
		}
	}

	// For output layer
	for (int i = 0; i < m_outputLayer.size(); ++i)
	{
		calculateNetInput(m_outputLayer[i], LayerType::output);
		calculateActivation(m_outputLayer[i]);
	}
	return m_outputLayer[0]->activation();
}

void Network::calculateNetInput(NeuronPtr nodePtr, LayerType layer) {
	int layerIndex = nodePtr->layerIndex();
	int nodeIndex = nodePtr-> nodeIndex();
	double total;
	NeuronPtr currentNode;
	switch(layer) {
		case LayerType::input:
			return;
		case LayerType::output:
			// for all Nodes in last layer
			for (int i = 0; i < m_programArgs->nodesPerLayer; ++i)
			{
				currentNode = (*m_hiddenLayers[m_hiddenLayers.size()-1])[i];
				// For all links from last hidden layer
				for (unsigned int j = 0; j < currentNode->outgoingLinks.size(); ++j)
				{
					if (currentNode->outgoingLinks[j]->destinationIndex() == nodeIndex) {
						int linkWeightIndex = currentNode->outgoingLinks[j]->weightIndex();
						total += m_weightVector[linkWeightIndex]*currentNode->activation();
					}
				}
			}
			m_outputLayer[nodeIndex]->setNetInput(total);
			lg->verbose("Output layer " + std::to_string(nodeIndex) + " NetInput: " + std::to_string(m_outputLayer[nodeIndex]->netInput()));
			total = 0;
			return;
		case LayerType::hidden:
			// First hidden layer, calculate input from input vector
			if (layerIndex == 0) {
				// For all nodes in input layer
				for (int i = 0; i < m_inputLayer.size(); ++i)
				{
					currentNode = m_inputLayer[i];
					// For all links in input Layer
					for (unsigned int j = 0; j < currentNode->outgoingLinks.size(); ++j)
					{
						if (currentNode->outgoingLinks[j]->destinationIndex() == nodeIndex) {
							int linkWeightIndex = currentNode->outgoingLinks[j]->weightIndex();
							total += m_weightVector[linkWeightIndex]*currentNode->activation();
						}
					}
				}
				(*m_hiddenLayers[layerIndex])[nodeIndex]->setNetInput(total);
				lg->verbose("Hidden Layer " + std::to_string(layerIndex) +
					" Node " + std::to_string(nodeIndex) +
					" NetInput: " + std::to_string((*m_hiddenLayers[layerIndex])[nodeIndex]->netInput()));
				total = 0;
			}
			else {
				// For all nodes in input layer
				for (int i = 0; i < m_programArgs->nodesPerLayer; ++i)
				{
					currentNode = (*m_hiddenLayers[layerIndex-1])[i];
					// For all links in input Layer
					for (unsigned int j = 0; j < currentNode->outgoingLinks.size(); ++j)
					{
						if (currentNode->outgoingLinks[j]->destinationIndex() == nodeIndex) {
							int linkWeightIndex = currentNode->outgoingLinks[j]->weightIndex();
							total += m_weightVector[linkWeightIndex]*currentNode->activation();
						}
					}
				}
				//Update NetIput
				(*m_hiddenLayers[layerIndex])[nodeIndex]->setNetInput(total);
				lg->verbose("Hidden Layer " + std::to_string(layerIndex) +
					" Node " + std::to_string(nodeIndex) +
					" NetInput: " + std::to_string((*m_hiddenLayers[layerIndex])[nodeIndex]->netInput()));
				total = 0;
			}
			return;
		default:
			lg->error("Network::calculateNetInput Corrupt LayerType.");
			return;
	}
}

// activation
void Network::calculateActivation(NeuronPtr nodePtr) {
	nodePtr->setActivation(sigmoid(nodePtr->netInput()));
	lg->verbose("Net Input: " + std::to_string(nodePtr->netInput()) + " Node Output: " + std::to_string(nodePtr->activation()));
}

void Network::backwardPass(double prediction, double actual) {
	NeuronPtr currentNode;
	double delta;
	int weightIndex, nodeIndex;
	lg->trace("backwardPass");

	// Calculate delta for output layer
	m_outputLayer[0]->setDeltaValue(calculateOutputDelta(prediction, actual));

	// For all hidden layers
	for (int i = m_hiddenLayers.size()-1; i >= 0 ; --i)
	{
		lg->verbose("Hidden Layer " + std::to_string(i));
		// For all nodes in layer
		for (int j = 0; j < m_programArgs->nodesPerLayer; ++j)
		{
			delta = 0.0;
			currentNode = (*m_hiddenLayers[i])[j];
			// Calculate delta from all links
			for (unsigned int k = 0; k < currentNode->outgoingLinks.size(); ++k)
			{
				weightIndex = currentNode->outgoingLinks[k]->weightIndex();
				// Last hidden layer
				if (i == m_hiddenLayers.size()-1) {
					delta += (m_weightVector[weightIndex] * m_outputLayer[0]->deltaValue());
					//wi+1 = wi + alfa*deltao1*outi
					m_tempVector[weightIndex] = m_weightVector[weightIndex] + m_programArgs->learningRate
						* m_outputLayer[0]->deltaValue() * currentNode->activation();
				}
				else {
					nodeIndex = currentNode->outgoingLinks[k]->destinationIndex();
					delta += (m_weightVector[weightIndex] * (*m_hiddenLayers[i+1])[nodeIndex]->deltaValue());
					m_tempVector[weightIndex] = m_weightVector[weightIndex] + m_programArgs->learningRate *
						(*m_hiddenLayers[i+1])[nodeIndex]->deltaValue() * currentNode->activation();
				}
			}
			delta = delta * currentNode->activation() * (1 - currentNode->activation());
			currentNode->setDeltaValue(delta);
		}
	}

	//Update weights from input layer
	for (int i = 0; i < m_inputLayer.size(); ++i)
	{
		currentNode = m_inputLayer[i];
		for (unsigned int k = 0; k < currentNode->outgoingLinks.size(); ++k)
		{
			weightIndex = currentNode->outgoingLinks[k]->weightIndex();
			nodeIndex = currentNode->outgoingLinks[k]->destinationIndex();
			m_tempVector[weightIndex] = m_weightVector[weightIndex] + m_programArgs->learningRate *
			(*m_hiddenLayers[0])[nodeIndex]->deltaValue() * currentNode->activation();
		}
	}
}

void Network::test() {
	lg->out("Testing network");
	int i = 0;
	double diff;
	while(!getTestData()) {
		double output = feedForward();
		double target = m_actualInput[m_actualInput.size()-1];
		double Error = computeError(output, target);
		lg->debug("Sample: " + std::to_string(i)
			+ " Output: " + std::to_string(output)
			+ " Target: " + std::to_string(target)
			+ " Error: " + std::to_string(Error)
			+ " Differerence: " + std::to_string(output-target)
			);
		if ((diff = fabs(target-output)) > m_maximumDifference)
			m_maximumDifference = diff;
		if (diff < m_minimumDifference)
			m_minimumDifference = diff;
		m_meanAbsoluteDeviation += diff;
		m_totalError += ((diff *diff)/2);
		target *= 10;
		output *= 10;
		if(round(target) == round(output)) {
			//lg->out("Correct: " + std::to_string(output) + "Target: " + std::to_string(target));
			m_correctVector[round(target)] += 1;
		}
		else {
			m_incorrectVector[round(target)] += 1;
			//lg->out("InCorrect: " + std::to_string(output) + "Target: " + std::to_string(target));
		}
		m_sampleNumber++;
	}
	m_meanAbsoluteDeviation /= m_sampleNumber;
	lg->out("Finished testing");
	printStats();
}

void Network::printStats() {
	int total = 0;
	lg->out(std::to_string(m_sampleNumber) + " samples tested");
	lg->out("Range - minimum: " + std::to_string(m_minimumDifference));
	lg->out("Range - maximum: " + std::to_string(m_maximumDifference));
	lg->out("Mean Absolute Deviation: " + std::to_string(m_meanAbsoluteDeviation));
	lg->out("Total Error: " + std::to_string(m_totalError));
	for (int i = 1; i <= 10; ++i)
	{
		total = m_correctVector[i] + m_incorrectVector[i];
		lg->out("Class " + std::to_string(i)
			+ "\tCorrect: " + (total==0?"0":std::to_string(((double)m_correctVector[i]/(double)total)*100)) + "%" +
			+ "\tInCorrect : " + (total==0?"0":std::to_string(((double)m_incorrectVector[i]/(double)total)*100)) + "%" +
			+ "\tTotal: " + std::to_string(total));
	}
}

double Network::calculateOutputDelta(double output, double target) {
	double delta = ((target - output) * output * (1 - output ));
	lg->verbose("Output Delta:" + std::to_string(delta));
	return delta;
}

void Network::flipWeights() {
	debugTemps();
	m_weightVector = m_tempVector;
}

double Network::computeError(double output, double target) {
	return (output - target)*(output - target);
}

double Network::sigmoid(double x) {
	return 1/(1+exp(-x));
}

void Network::parseInput(std::string line) {
	size_t pos;
	std::string token;
	m_actualInput.clear();
	// Split string by ; and push back tokens
	while ((pos = line.find(';')) != std::string::npos) {
		token = line.substr(0, pos);
		m_actualInput.push_back(std::stod(token));
		line.erase(0, pos + 1);
	}
	// Push back Output value
	m_actualInput.push_back(std::stod(line));
}

void Network::getInputData() {
	std::string line;
	// Get next input from file
	if (std::getline(m_inputFile, line)) {
		// Push back inputs
		parseInput(line);
	}
	else {
		// At the end f file go back to beginning
		m_inputFile.clear();
		m_inputFile.seekg (0, m_inputFile.beg);
		m_epoch++;
		lg->debug("Epoch: " + std::to_string(m_epoch) + "/" + std::to_string(m_programArgs->trainingEpochs));
		std::getline(m_inputFile, line);
		parseInput(line);
	}
}

int Network::getTestData() {
	std::string line;
	// Get next input from file
	if (std::getline(m_testFile, line)) {
		// Push back inputs
		parseInput(line);
		return 0;
	}
	else {
		m_testFile.close();
		return 1;
	}
}

void Network::debugWeights() {
	std::string out = "Weight";
	for (unsigned int i = 0; i < m_weightVector.size(); i++) {
		out += (" " + std::to_string(m_weightVector[i]));
	}
	lg->verbose(out);
}

void Network::debugTemps() {
	std::string out = "Temporary";
	for (unsigned int i = 0; i < m_tempVector.size(); i++) {
		out += (" " + std::to_string(m_tempVector[i]));
	}
	lg->verbose(out);
}

void Network::printdeltaWeights() {
	std::string out = "Weight";
	for (unsigned int i = 0; i < m_weightVector.size(); i++) {
		out += (" " + std::to_string(m_weightVector[i]-m_tempVector[i]));
	}
	lg->verbose(out);
}

void Network::debugLinks() {
	lg->verbose("Input Layer Links:");
	std::string out;
	for (int i = 0; i < m_inputLayer.size(); ++i)
	{
		for (unsigned int j = 0; j < m_inputLayer[i]->outgoingLinks.size(); ++j)
		{
			out = "Link " + std::to_string(i) + ": ";
			out += "Weight : " + std::to_string(m_inputLayer[i]->outgoingLinks[j]->weightIndex());
			out += " DestinationNode : " + std::to_string(m_inputLayer[i]->outgoingLinks[j]->destinationIndex());
			lg->verbose(out);
		}
	}

	for (int i = 0; i < m_programArgs->hiddenLayerNumber; ++i)
	{
		lg->verbose("Hidden Layer " + std::to_string(i) + ":");
		for (int j = 0; j < m_programArgs->nodesPerLayer; ++j)
		{
			for (unsigned int k = 0; k < (*m_hiddenLayers[i])[j]->outgoingLinks.size(); ++k)
			{
				out = "Link" + std::to_string(j) + ": ";
				out += "Weight: " + std::to_string((*m_hiddenLayers[i])[j]->outgoingLinks[k]->weightIndex());
				out += " DestinationNode : " + std::to_string((*m_hiddenLayers[i])[j]->outgoingLinks[k]->destinationIndex());
				lg->verbose(out);
			}
		}
	}
}

void Network::preprocess() {
	lg->debug("Preprocess");
	std::vector<int> v { 0,1,2,3,4,5,6,7,8,9,10,11 };
	size_t pos;
	std::string token;
	std::string out;
	int i = 0;
	std::string line;
	double target;

	while(std::getline(m_inputFile, line)) {
		while ((pos = line.find(';')) != std::string::npos) {
			token = line.substr(0, pos);
			line.erase(0, pos + 1);
			for(auto it = v.begin(); it != v.end(); ++it) {
				lg->debug(std::to_string(*it));
				if(*it == i) {
					out += (token + ";");
					//lg->out(token);
				}
			}
			i++;
		}
		//lg->out(line);
		//lg->out("");
		target = (std::stod(line)/10);
		out += std::to_string(target);
		lg->out(out);
		out = "";
		i = 0;
	}
}
