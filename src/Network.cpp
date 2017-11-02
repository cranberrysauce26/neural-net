#include "../inc/Network.h"
#include "../inc/Data.h"

#include <vector>
#include <random>
#include <algorithm>


using namespace std;


Network::Network(const vector<int>& sizes) {

	randGen = mt19937(randDev()); 

	// defaults to mean of 0.0, standard dev of 1.0
    randDistribution = normal_distribution<double>(); 

	layerSizes = sizes;
	bias = vector<vector<double> >(sizes.size());
	weights = vector<vector<vector<double> > >(sizes.size()-1);

	for (int i = 0; i < sizes.size(); ++i) {
		// don't set for input layer
		if (i) { 
			bias[i] = vector<double>(sizes[i]);
			for (int j = 0; j < bias[i].size(); ++j) {
				bias[i][j] = randDistribution(randGen); // is a random double
			}
		}
		
		weights[i] = vector<vector<double> >(sizes[i]);
		for (int j = 0; j < weights[i].size(); ++j) {
			weights[i][j] = vector<double>(sizes[i+1]);
			for (int k = 0; k < weights[i][j].size(); ++k) {
				weights[i][j][k] = randDistribution(randGen);
			}
		}
	}
}


void Network::SGD(vector<Data>& data, int numEpochs, int batchSize, double trainingRate) {
	for (int epoch = 1; epoch <= numEpochs; ++epoch) {
		shuffle(data.begin(), data.end(), randGen);
		vector< vector<Data> > batches;
		for (int i = 0; i < data.size(); ++i) {
			if (i%batchSize==0) batches.push_back(vector<Data>());
			batches.back().push_back(data[i]);
		}
		for (auto batch : batches) {
			updateBatch(batch, trainingRate);
			printf("Epoch %d complete\n", epoch);
		}
	}
}
