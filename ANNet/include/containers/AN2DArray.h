/*
 * NeuronArray.h
 *
 *  Created on: 28.01.2011
 *      Author: dgrat
 */

#ifndef NEURONARRAY_H_
#define NEURONARRAY_H_

#include <iostream>
#include <gpgpu/ANMatrix.h>

namespace ANN {

class F3DArray;


/**
 * \brief Pseudo 2D-array as a container for the neurons and error deltas of the network.
 *
 * @author Daniel "dgrat" Frenzel
 */
class F2DArray {
	friend class F3DArray;

private:
	//std::vector<float> 	m_pSubArray;
	bool 	m_bAllocated;

protected:
	void SetArray(float *pArray, const int &iX, const int &iY);
	float *GetArray() const;

public:
	// Public Access for CUDA
	int 	m_iX;		// nr. of neurons in layer m_iY
	int 	m_iY;		// nr. of layer in net
	float 	*m_pArray;	// value of neuron

	// Standard C++ "conventions"
	F2DArray();
	F2DArray(const Matrix &);
	F2DArray(float *pArray, const int &iSizeX, const int &iSizeY);
	virtual ~F2DArray();

	void Alloc(const int &iSize);
	void Alloc(const int &iX, const int &iY);

	const int &GetW() const;
	const int &GetH() const;
	int GetTotalSize() const; //X*Y

//	float *GetSubArrayX(const int &iY) const;
//	float *GetSubArrayY(const int &iX) const;

	std::vector<float> GetSubArrayX(const int &iY) const;
	std::vector<float> GetSubArrayY(const int &iX) const;

	/**
	 * Returns a sub array beginning at position [iX][iY]
	 * Running from line to line until iSize ints got copied
	 */
	F2DArray GetSubarray(const int &iX, const int &iY, const int &iSize);

	void SetValue(const float &fVal, const int &iX, const int &iY);
	float GetValue(const int &iX, const int &iY) const;

	void GetOutput();

//OPERATORS
	operator float*();
	float *operator[] (const int &iY) const;

	/**
	 * CUDA THRUST compatibility
	 * host_vector<float>: Contains one row of the matrix
	 * host_vector< host_vector<float> >: Contains all rows  of the matrix
	 */
	operator Matrix ();
};

}

#endif /* NEURONARRAY_H_ */