/*
 * ANHFNet.h
 *
 *  Created on: 21.02.2011
 *      Author: dgrat
 */

#ifndef ANHFNET_H_
#define ANHFNET_H_

#include <vector>
#include <string>

#include <basic/ANAbsNet.h>

namespace ANN {


/**
 * \brief Implementation of a hopfield network.
 */
class HFNet : public AbsNet {
private:
	unsigned int m_iWidth;
	unsigned int m_iHeight;

	void CalculateMatrix();

public:
	HFNet();
	HFNet(const unsigned int &iW, const unsigned int &iH);
	HFNet(AbsNet *pNet);
	virtual ~HFNet();

	/**
	 * Creates a single layered network with iW * iH neurons.
	 * Each neuron is connected to all other neurons.
	 * @param iW Width of the net.
	 * @param iH Height of the net.
	 */
	void Resize(const unsigned int &iW, const unsigned int &iH);

	/**
	 * Propagates through all neurons of the net.
	 * \f$
	 * s_i =
	 * \begin{cases}
	 * 		1 & \mbox {if }\sum_{j}{w_{ij}s_j}>\theta_i,
	 * 	\\ -1 & \mbox {otherwise}
	 * \end{cases}
	 * \\
	 * \\ s_i \text{ is the current state of the neuron which will get updated and}
	 * \\ \theta_i \text{ is the bias}
	 * \f$
	 */
	virtual void PropagateFW();
	/**
	 * Calculates the weight matrix according to this function:
	 * \f$
	 * w_{j,i}=w_{i,j}=
	 * \begin{cases}
	 * \sum_{\mu=0}^L M_{\mu,i}*M_{\mu,j} & \mbox {if } i \neq j
	 * \\ 0 & \mbox {otherwise}
	 * \end{cases}
	 * \\
	 * \\ M \in \mathbb{R}^{L\times N}
	 * \\ L \hat = \text{ is the number of patterns}
	 * \\ N \hat = \text{ are the single values in the patterns}
	 * \f$
	 */
	virtual void PropagateBW();

	/**
	 * Set the value of neurons in the input layer to new values
	 * @param pInputArray Inherits the values of the input layer. Array has to have the same size like the net.
	 */
	void SetInput(float *pInputArray);
	/**
	 * Set the value of neurons in the input layer to new values
	 * @param vInputArray Inherits the values of the input layer.
	 */
	void SetInput(std::vector<float> vInputArray);
};

}

#endif /* ANHFNET_H_ */