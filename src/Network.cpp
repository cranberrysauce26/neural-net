#include "../inc/Network.h"

using namespace std; // does not affect main.cpp

// initialize the network with random weights and biases
Network::Network(const vector<int>& sizes) {

	randGen = mt19937(randDev()); 

	// defaults to mean of 0.0, standard dev of 1.0
	randDistribution = normal_distribution<double>(); 

	layerSizes = sizes;

	biases = v2dbl(sizes.size());
	weights = v3dbl(sizes.size()-1);

	for (int i = 0; i < numLayers; ++i) {
		// don't set for input layer
		if (i) { 
			biases[i] = vdbl(sizes[i]);
			for (int j = 0; j < biases[i].size(); ++j) {
				biases[i][j] = randDistribution(randGen); // is a random double
			}
		}
		
		weights[i] = v2dbl(sizes[i]);
		for (int j = 0; j < weights[i].size(); ++j) {
			weights[i][j] = vdbl(sizes[i+1]);
			for (int k = 0; k < weights[i][j].size(); ++k) {
				weights[i][j][k] = randDistribution(randGen);
			}
		}
	}
}

void Network::feedForward(vdbl& a) {
	// a is initially the vector of inputs
	vdbl dot;
	for (int i = 0; i < numLayers - 1; ++i) {
		// update layer i + 1 from layer i
		dot = vdbl(0.0, layerSizes[i+1]);
		for (int j = 0; j < layerSizes[i]; ++j) {
			for (int k = 0; k < layerSizes[i+1]; ++k) {
				dot[k] += weights[i][j][k] * a[j];
			}
		}
		a.resize(dot.size());
		for (int k = 0; k < a.size(); ++k) {
			a[k] = sigmoid(dot[k]+biases[i+1][k]);
		}
	}
}

void Network::SGD(vector<trdata>& data, int numEpochs, int batchSize, double trainingRate) {
	for (int epoch = 1; epoch <= numEpochs; ++epoch) {
		shuffle(data.begin(), data.end(), randGen);
		vector< vector<trdata> > batches;
		for (int i = 0; i < data.size(); ++i) {
			if (i%batchSize == 0) batches.push_back(vector<trdata>());
			batches.back().push_back(data[i]);
		}
		for (auto batch : batches) {
			updateBatch(batch, trainingRate);
			printf("Epoch %d complete\n", epoch);
		}
	}
}

void Network::updateBatch(vector<trdata>& batch, double trainingRate) {

	// gradb[i][j] is the gradient for the bias of the jth node in layer i
	// gradw[i][j][k] is the gradient for the weight from the jth node in layer i to the kth node in layer i+1
	v2dbl gradb(numLayers), dgradb(numLayers);
	for (int i = 0; i < numLayers; ++i) {
		gradb[i] = dgradb[i] = vdbl(0.0, biases[i].size());
	}
	v3dbl gradw(numLayers - 1), dgradw(numLayers - 1);
	for (int i = 0; i < numLayers - 1; ++i)	{
		gradw[i] = dgradw[i] = v2dbl(weights[i].size());
		for (int j = 0; j < weights[i].size(); ++j) {
			gradw[i][j] = dgradw[i][j] = vdbl(0.0, weights[i][j].size());
		}
	}
	// Range based loops by constant reference!
	for (const trdata& data : batch) { 

		// dgradb[i][j] is the partial derivative of the cost function 
		// for the current data relative to b[i][j]
		// dgradw[i][j][k] is the partial derivative of the cost function
		// for the current data relative to w[i][j][k]

		// pass dgradb and dgradw by reference
		backprop(data, dgradb, dgradw); 


		for (int i = 0; i < gradb.size(); ++i) {
			for (int j = 0; j < gradb[i].size(); ++j) {
				gradb[i][j] += dgradb[i][j];
			}
		}
		for (int i = 0; i < gradw.size(); ++i) {
			for (int j = 0; j < gradw[i].size(); ++j) {
				for (int k = 0; k < gradw[i][j].size(); ++k) {
					gradw[i][j][k] += dgradw[i][j][k];
				}
			}
		}
	}

	for (int i = 0; i < biases.size(); ++i) {
		for (int j = 0; j < biases[i].size(); ++j) {
			biases[i][j] -= trainingRate/static_cast<double>(batch.size()) * gradb[i][j];
		}
	}

	for (int i = 0; i < weights.size(); ++i) {
		for (int j = 0; j < weights[i].size(); ++j) {
			for (int k = 0; k < weights[i][j].size(); ++k) {
				weights[i][j][k] -= trainingRate/static_cast<double>(batch.size()) * gradw[i][j][k];
			}
		}
	}

}

void Network::backprop(const trdata& data, v2dbl& dgradb, v3dbl& dgradw){
	return;
}


inline double Network::sigmoid(double x) {return 1.0 / (1.0 + exp(-x));}
inline double Network::sigmoidPrime(double x) {return sigmoid(x)*(1-sigmoid(x));}
