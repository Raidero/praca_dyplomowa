#pragma once
#include <vector>

using namespace std;

class WaveletTransform
{
	public:
		WaveletTransform();
		virtual ~WaveletTransform();
	protected:
		void forwardWalvetTransform97(vector<double>& values);
		void inverseWalvetTranform97(vector<double>& values);
		void predictionStep(vector<double>& values, const double coefficient);
		void updateStep(vector<double>& values, const double coefficient);
		void normalizeStep(vector<double>& values);
		void orderValues(vector<double>& values);
		void disorderValues(vector<double>& values);
		void denormalizeStep(vector<double>& values);
};

