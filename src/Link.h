#ifndef BPNETWORK_LINK_H
#define BPNETWORK_LINK_H

#include <memory>
#include <vector>

/**
 * @brief Class representing link between Neurons in different Layers
 * @details Consists from target Neuron and associated weight
 *
 * @param destinationNeuronIndex integer, specifies offset in Layer of target Neuron
 * @param weightIndex integer, specifies offset of weight from this links in vector of weights
 */
class Link
{
public:
	typedef std::shared_ptr<Link> Ptr;
	typedef std::vector<Ptr> Vector;
	Link(int destinationNeuronIndex,
		int weightIndex) :
	m_destinationNeuronIndex(destinationNeuronIndex),
	m_weightIndex(weightIndex)
	{
	}
	~Link() {
	}

	void setweightIndex(int index) {
		m_weightIndex = index;
	}

	int weightIndex() {
		return m_weightIndex;
	}

	int destinationIndex() {
		return m_destinationNeuronIndex;
	}

private:
	int m_destinationNeuronIndex;
	int m_weightIndex;
};

typedef Link::Ptr LinkPtr;
typedef Link::Vector LinkVector;

#endif
