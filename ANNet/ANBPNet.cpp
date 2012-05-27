/*
 * Net.cpp
 *
 *  Created on: 30.05.2009
 *      Author: Xerces
 */

#include <iostream>
#include <cassert>
#include <omp.h>
//own classes
#include <math/ANRandom.h>
#include <math/ANFunctions.h>
#include <containers/ANTrainingSet.h>
#include <containers/ANConTable.h>
#include <basic/ANEdge.h>
#include <ANBPNeuron.h>
#include <ANBPLayer.h>
#include <ANBPNet.h>

using namespace ANN;


BPNet::BPNet() {
	m_fTypeFlag 	= ANNetBP;
}

BPNet::BPNet(AbsNet *pNet) : AbsNet(pNet) {
	assert( pNet != NULL );
	*this = *GetSubNet( 0, pNet->GetLayers().size()-1 );

	m_fTypeFlag 	= ANNetBP;
}

BPNet::~BPNet() {
}

void BPNet::AddLayer(AbsLayer *pLayer) {
	AbsNet::AddLayer(pLayer);

	if( ( (BPLayer*)pLayer)->GetFlag() & ANLayerInput ) {
		m_pIPLayer = pLayer;
	}
	else if( ( (BPLayer*)pLayer)->GetFlag() & ANLayerOutput ) {
		m_pOPLayer = pLayer;
	}
	else {
	}
}

/*
 * TODO better use of copy constructors
 */
BPNet *BPNet::GetSubNet(const unsigned int &iStartID, const unsigned int &iStopID) {
	assert( iStopID < GetLayers().size() );
	assert( iStartID >= 0 );

	/*
	 * Return value
	 */
	BPNet *pNet = new BPNet;

	/*
	 * Create layers like in pNet
	 */
	for(unsigned int i = iStartID; i <= iStopID; i++) {
		BPLayer *pLayer = new BPLayer( ( (BPLayer*)GetLayer(i) ) );
		if( i == iStartID && !(( (BPLayer*)GetLayer(i) )->GetFlag() & ANLayerInput) )
			pLayer->AddFlag( ANLayerInput );
		if( i == iStopID && !(( (BPLayer*)GetLayer(i) )->GetFlag() & ANLayerOutput) )
			pLayer->AddFlag( ANLayerOutput );

		pNet->AddLayer( pLayer );
	}

	BPLayer 	*pCurLayer;
	AbsNeuron 	*pCurNeuron;
	Edge 		*pCurEdge;
	for(unsigned int i = iStartID; i <= iStopID; i++) { 	// layers ..
		// NORMAL NEURON
		pCurLayer = ( (BPLayer*)GetLayer(i) );
		for(unsigned int j = 0; j < pCurLayer->GetNeurons().size(); j++) { 		// neurons ..
			pCurNeuron = pCurLayer->GetNeurons().at(j);
			AbsNeuron *pSrcNeuron = ( (BPLayer*)pNet->GetLayer(i) )->GetNeuron(j);
			for(unsigned int k = 0; k < pCurNeuron->GetConsO().size(); k++) { 			// edges ..
				pCurEdge = pCurNeuron->GetConO(k);

				// get iID of the destination neuron of the (next) layer i+1 (j is iID of (the current) layer i)
				int iDestNeurID 	= pCurEdge->GetDestinationID(pCurNeuron);
				int iDestLayerID 	= pCurEdge->GetDestination(pCurNeuron)->GetParent()->GetID();

				// copy edge
				AbsNeuron *pDstNeuron = pNet->GetLayers().at(iDestLayerID)->GetNeuron(iDestNeurID);

				// create edge
				Connect( pSrcNeuron, pDstNeuron,
						pCurEdge->GetValue(),
						pCurEdge->GetMomentum(),
						pCurEdge->GetAdaptationState() );
			}
		}

		// BIAS NEURON
		if( ( (BPLayer*)GetLayer(i) )->GetBiasNeuron() != NULL) {	// importt requirement, else access violation
			pCurLayer 	= ( (BPLayer*)GetLayer(i) );
			pCurNeuron = pCurLayer->GetBiasNeuron();
			BPNeuron *pBiasNeuron 	= ( (BPLayer*)pNet->GetLayer(i) )->GetBiasNeuron();

			for(unsigned int k = 0; k < pCurNeuron->GetConsO().size(); k++) {
				pCurEdge = pCurNeuron->GetConO(k);

				int iDestNeurID 	= pCurEdge->GetDestinationID(pCurNeuron);
				int iDestLayerID 	= pCurEdge->GetDestination(pCurNeuron)->GetParent()->GetID();

				// copy edge
				AbsNeuron *pDstNeuron 	= pNet->GetLayers().at(iDestLayerID)->GetNeuron(iDestNeurID);

				// create edge
				Connect( pBiasNeuron, pDstNeuron,
						pCurEdge->GetValue(),
						pCurEdge->GetMomentum(),
						pCurEdge->GetAdaptationState() );
			}
		}
	}

	// Import further properties
	if( GetNetFunction() )
		pNet->SetNetFunction( pNet->GetNetFunction() );
	if( GetTrainingSet() )
		pNet->SetTrainingSet( this->GetTrainingSet() );
	pNet->SetLearningRate( this->GetLearningRate() );
	pNet->SetMomentum( this->GetMomentum() );

	return pNet;
}

void BPNet::PropagateFW() {
	for(unsigned int i = 1; i < m_lLayers.size(); i++) {
		BPLayer *curLayer = ( (BPLayer*)GetLayer(i) );
		#pragma omp parallel for
		for(int j = 0; j < static_cast<int>( curLayer->GetNeurons().size() ); j++) {
			curLayer->GetNeuron(j)->CalcValue();
		}
	}
}


void BPNet::PropagateBW() {
	/*
	 * Berechne  Hd der Abweichung vom Sollwert die Fehlerdeltas aller Neuronen
	 */
	for(int i = m_lLayers.size()-1; i >= 0; i--) {
		BPLayer *curLayer = ( (BPLayer*)GetLayer(i) );
		#pragma omp parallel for
		for(int j = 0; j < static_cast<int>( curLayer->GetNeurons().size() ); j++) {
			curLayer->GetNeuron(j)->AdaptEdges();
		}

		#pragma omp parallel
		if(curLayer->GetBiasNeuron() != NULL) {
			curLayer->GetBiasNeuron()->AdaptEdges();
		}
	}
}

void BPNet::SetLearningRate(const float &fVal)
{
	m_fLearningRate = fVal;
	#pragma omp parallel for
	for(int i = 0; i < static_cast<int>(m_lLayers.size() ); i++) {
		( (BPLayer*)GetLayer(i) )->SetLearningRate(fVal);
	}
}

float BPNet::GetLearningRate() const {
	return m_fLearningRate;
}

void BPNet::SetMomentum(const float &fVal) {
	m_fMomentum = fVal;
	#pragma omp parallel for
	for(int i = 0; i < static_cast<int>(m_lLayers.size() ); i++) {
		( (BPLayer*)GetLayer(i) )->SetMomentum(fVal);
	}
}

float BPNet::GetMomentum() const {
	return m_fMomentum;
}

void BPNet::SetWeightDecay(const float &fVal) {
	m_fWeightDecay = fVal;
	#pragma omp parallel for
	for(int i = 0; i < static_cast<int>(m_lLayers.size() ); i++) {
		( (BPLayer*)GetLayer(i) )->SetWeightDecay(fVal);
	}
}

float BPNet::GetWeightDecay() const {
	return m_fWeightDecay;
}