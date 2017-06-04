#ifndef BPNETWORK_LAYER_H
#define BPNETWORK_LAYER_H

#include "Neuron.h"

enum LayerType { hidden, input, output};

/**
 * @brief Class representing one Layer in Network
 * @details Consists from individual Neurons
 */
class Layer
{
public:
typedef std::shared_ptr<Layer> Ptr;

	Layer() {
	}
	Layer(LayerType type) :
		m_layerType(type)
	{
	}
	~Layer() {
	}

	NeuronPtr operator[](int index) {
		return m_neutrons[index];
	}

	LayerType layerType() {
		return m_layerType;
	}

	void add(NeuronPtr neutronPtr) {
		m_neutrons.push_back(neutronPtr);
	}

	int size() {
		return m_neutrons.size();
	}

private:
	LayerType m_layerType;
	NeuronVector m_neutrons;
};

typedef Layer::Ptr LayerPtr;
typedef std::vector<LayerPtr> LayerVector;
#endif
