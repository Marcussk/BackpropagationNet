#include "Neuron.h"

Neuron::Neuron(int nodeIndex, int layerIndex) :
	m_netInput(-1),
	m_activation(-1),
	m_nodeIndex(nodeIndex),
	m_layerIndex(layerIndex)
{
}

Neuron::~Neuron() {
}
