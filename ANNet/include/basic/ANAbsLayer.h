/*
 * ANBasicLayer.h
 *
 *  Created on: 21.02.2011
 *      Author: dgrat
 */

#ifndef ANBASICLAYER_H_
#define ANBASICLAYER_H_

#include <iostream>
#include <vector>
#include <stdint.h>
#include <containers/AN2DArray.h>

namespace ANN {

// own classes
class AbsNeuron;
class Function;


enum {
	ANLayerInput 	= 1 << 0,	// type of layer
	ANLayerHidden 	= 1 << 1,	// type of layer
	ANLayerOutput 	= 1 << 2,	// type of layer

	ANBiasNeuron 	= 1 << 3	// properties of layer
};
typedef uint32_t LayerTypeFlag;

/**
 * \brief Represents a container for neurons in the network.
 */
class AbsLayer {
protected:
	/*
	 * Array of pointers to all neurons in this layer.
	 */
	std::vector<AbsNeuron *> m_lNeurons;

	/*
	 * ID of the layer
	 */
	int m_iID;

	/*
	 * Flag describing the kind of layer.
	 * (i. e. input, hidden or output possible)
	 */
	LayerTypeFlag m_fTypeFlag;

public:
	AbsLayer();
//	AbsLayer(const unsigned int &iNumber, int iShiftID = 0);
	virtual ~AbsLayer();

	/**
	 * Sets the current ID in the Network inheriting the layer.
	 * Useful for administration purposes.
	 */
	virtual void SetID(const int &iID);
	/**
	 * Returns the current ID in the Network inheriting the layer.
	 * Useful for administration purposes.
	 */
	int GetID() const;

	/*
	 * TODO
	 */
	void EraseAllEdges();
	/**
	 * Deletes the complete layer (all connections and all values).
	 */
	virtual void EraseAll();

	/**
	 * Resizes the layer. Deletes old neurons and adds new ones (initialized with random values).
	 * @param iSize New number of neurons.
	 * @param iShiftID When called each neuron created gets an ID defined in this function plus the value of iShiftID. Used for example in ANHFLayer, when creating 2d matrix.
	 */
	virtual void Resize(const unsigned int &iSize) = 0;

	/**
	 * Pointer to the neuron at index.
	 * @return Returns the pointer of the neuron at index iID
	 * @param iID Index of the neuron in m_lNeurons
	 */
	AbsNeuron *GetNeuron(const unsigned int &iID) const;
	/**
	 * List of all neurons in this layer (not bias neuron).
	 * @return Returns an array with pointers of neurons in this layer.
	 */
	const std::vector<AbsNeuron *> &GetNeurons() const;

	/**
	 * Defines the type of "activation" function the net has to use for back-/propagation.
	 * @param pFunction New "activation" function
	 */
	virtual void SetNetFunction 	(const Function *pFunction);

	/**
	 * Sets the type of the layer (input, hidden or output layer)
	 * @param fType Flag describing the type of the layer.
	 * Flag: "ANBiasNeuron" will automatically add a bias neuron.
	 */
	virtual void SetFlag(const LayerTypeFlag &fType);
	/**
	 * Adds a flag if not already set.
	 * @param fType Flag describing the type of the layer.
	 * Flag: "ANBiasNeuron" will automatically add a bias neuron.
	 */
	virtual void AddFlag(const LayerTypeFlag &fType);
	/**
	 * Type of the layer
	 * @return Returns the flag describing the type of the layer.
	 */
	LayerTypeFlag GetFlag() const;

	// FRIEND
	friend void SetEdgesToValue(AbsLayer *pSrcLayer, AbsLayer *pDestLayer, const float &fVal, const bool &bAdaptState = false);

	/** \brief:
	 * NEURON1	 			: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * NEURON2 				: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * NEURON3	 			: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * NEURON[i < iHeight] 	: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * ..
	 * @return Returns a matrix with a row for each neuron and a column for each incoming weight from the previous layer
	 */
	virtual F2DArray ExpEdgesIn() const;
	/** \brief:
	 * NEURON1	 			: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * NEURON2 				: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * NEURON3	 			: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * NEURON[i < iHeight] 	: edge1, edge2, edge[n < iWidth] ==> directing to input neuron 1, 2, n
	 * ..
	 * @return Returns a matrix with a row for each neuron and a column for each outgoing weight to the next layer
	 */
	virtual F2DArray ExpEdgesOut() const;

	virtual void ImpEdgesIn(const F2DArray &);

	virtual void ImpEdgesOut(const F2DArray &);

	/**
	 * pPositions:
	 * NEURON1	 			: X, Y, POS[n < iWidth] ==> directing to input
	 * NEURON2 				: X, Y, POS[n < iWidth] ==> directing to input
	 * NEURON3	 			: X, Y, POS[n < iWidth] ==> directing to input
	 * NEURON[i < iHeight] 	: X, Y, POS[n < iWidth] ==> directing to input
	 */
	virtual F2DArray ExpPositions() const;
	virtual void ImpPositions(const F2DArray &f2dPos);
};

}
#endif /* ANBASICLAYER_H_ */