#ifndef BPNETWORK_NEUTRON_H
#define BPNETWORK_NEUTRON_H

#include <vector>
#include <memory>

#include "Link.h"

/**
 * @brief Class representing one Neuron in Layer
 *
 * @param nodeIndex offset of this Neuron in layer
 * @param layerIndex owning layer offset in vector of hidden Layers

 */
class Neuron
{
public:
	typedef std::shared_ptr<Neuron> Ptr;
	typedef std::vector<Ptr> Vector;

	Neuron(int nodeIndex, int layerIndex);
	~Neuron();

	int nodeIndex() {
		return m_nodeIndex;
	}

	int layerIndex() {
		return m_layerIndex;
	}

	double netInput() {
		return m_netInput;
	}

	double activation() {
		return m_activation;
	}

	double deltaValue() {
		return m_deltaValue;
	}

	void setNetInput(double input) {
		m_netInput = input;
	}

	void setActivation(double activation) {
		m_activation = activation;
	}

	void setDeltaValue(double value) {
		m_deltaValue = value;
	}

	LinkVector outgoingLinks;
private:
	// total input t this Neuron
	double m_netInput;
	// total output (activation) of this Neuron
	double m_activation;
	// value of Delta rule in this Neuron
	double m_deltaValue;

	int m_nodeIndex;
	int m_layerIndex;
};

typedef Neuron::Ptr NeuronPtr;
typedef Neuron::Vector NeuronVector;

#endif
