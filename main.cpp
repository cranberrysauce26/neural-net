#include <cstdio>
#include <vector>
#include <ctime>
#include <cstdlib>

#include "inc/Network2.h"
#include "inc/Layer.h"
#include "inc/types.h"
#include "inc/FullyConnectedLayer.h"
#include "inc/ActivationFunction.h"

#include <Eigen/Dense>

typedef long long ll;

using namespace std;

Vec binary(ll i, int bits)
{
	Vec v(bits);
	for (int j = 0; j < bits; ++j) {
		if (i & (1LL << j))
			v[j] = 1.0;
		else
			v[j] = 0.0;
	}
	return v;
}

Vec convertToBase(ll i, int base, int digits) {
	Vec v(digits);
	for (int j = 0; j < digits; j++) {
		v[j] = i % base;
		i /= base;
	}
}

Vec mod(long long i, int m) {
	Vec v = Vec::Zero(m);
	v[i % m] = 1.0;
	return v;
}

pair<int, double> check(const Mat& tocheck, const Mat& correct)
{
	if (tocheck.rows() != correct.rows() || tocheck.cols() != correct.cols()) {
		printf("ERROR in check: Vectors are different sizes\n");
		return make_pair(0, 0);
	}
	int count = 0;
	double cost = 0.0;
	for (int col = 0; col < tocheck.cols(); col++)
	{
		bool works = true;
		for (int i = 0; i < tocheck.rows(); ++i) {
			double error = abs(tocheck(i, col) - correct(i, col));
			if (error >= 0.5)
				works = false;
			cost += error * error;
		}
		if (works)
			++count;
	}
	return make_pair(count, cost / tocheck.cols());
}

pair<int, double> checkMax(const Mat& tocheck, const Mat& correct)
{
	if (tocheck.rows() != correct.rows() || tocheck.cols() != correct.cols()) {
		printf("ERROR in check: Vectors are different sizes\n");
		return make_pair(0, 0);
	}
	int count = 0;
	double cost = 0.0;
	for (int col = 0; col < tocheck.cols(); col++)
	{
		int index = 0;
		for (int i = 0; i < tocheck.rows(); ++i) {
			if (tocheck(i, col) > tocheck(index, col))
				index = i;
			double error = abs(tocheck(i, col) - correct(i, col));
			cost += error * error;
		}
		if (correct(index, col))
			++count;
	}
	return make_pair(count, cost / tocheck.cols());
}

int main() {
	srand(time(NULL));
	int d = 6, m = 10; //d = digits, m = mod
	typedef FullyConnectedLayer<SigmoidActivationFunction> SigLayer;

	vector<Layer*> layers;
	layers.push_back(new SigLayer(2*d, 20*d));
	layers.push_back(new SigLayer(20*d, d+1));
	Network2 n(layers, checkMax, 2*d, d+1, 16, 0.5);
	trbatch training(100000), testing(100);

	ll limit = 1LL;
	for (int i = 0; i < d; i++) limit *= 10;

	for (trdata& data : testing) {
		ll i = rand() % limit;
		data.first = convertToBase(i, 10, d);
		data.second = binary(i + j, d + 1);
	}
	for (trdata& data : training) {
		ll i = rand() % limit;
		ll j = rand() % limit;
		data.first = base10(i * limit + j, 2 * d);
		data.second = base10(i + j, d + 1);
	}
	
	n.train(training, testing, 500);
}
//addition
/*
for (trdata& data : testing) {
ll i = rand() & ((1LL << bits) - 1);
ll j = rand() & ((1LL << bits) - 1);
data.first = binary((i << bits) + j, 2 * bits);
data.second = binary(i + j, bits + 1);
}
for (trdata& data : training) {
ll i = rand() & ((1LL << bits) - 1);
ll j = rand() & ((1LL << bits) - 1);
data.first = binary((i << bits) + j, 2 * bits);
data.second = binary(i + j, bits + 1);
}
*/

// mod

//for (trdata& data : testing) {
//	ll i = rand() % limit;
//	data.first = binary(i, bits);
//	data.second = mod(i, m);
//}
//for (trdata& data : training) {
//	ll i = rand() & ((1LL << bits) - 1);
//	data.first = binary(i, bits);
//	data.second = mod(i, m);
//}