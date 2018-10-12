#include "pch.h"
#include "WaveletTransform.h"

WaveletTransform::WaveletTransform()
{
}

WaveletTransform::~WaveletTransform()
{
}

void WaveletTransform::forwardWalvetTransform97(vector<double>& values)
{
	predictionStep(values, -1.586134342);
	updateStep(values, -0.05298011854);

	predictionStep(values, 0.8829110762);
	updateStep(values, 0.4435068522);

	normalizeStep(values);
	orderValues(values);
}

void WaveletTransform::inverseWalvetTranform97(vector<double>& values)
{
	disorderValues(values);
	denormalizeStep(values);

	updateStep(values, -0.4435068522);
	predictionStep(values, -0.8829110762);

	updateStep(values, 0.05298011854);
	predictionStep(values, 1.586134342);
}

void WaveletTransform::predictionStep(vector<double>& values, const double coefficient)
{
	for (int i = 1; i < values.size() - 2; i += 2)
	{
		values[i] += coefficient * (values[i - 1] + values[i + 1]);
	}
	values[values.size() - 1] += 2 * coefficient * values[values.size() - 2];
}

void WaveletTransform::updateStep(vector<double>& values, const double coefficient)
{
	for (int i = 2; i < values.size(); i += 2)
	{
		values[i] += coefficient * (values[i - 1] + values[i + 1]);
	}
	values[0] += 2 * coefficient * values[1];
}

void WaveletTransform::normalizeStep(vector<double>& values)
{
	const double coefficient = 1 / 1.149604398;
	for (int i = 0; i < values.size(); ++i)
	{
		if (i % 2 == 1)
		{
			values[i] *= coefficient;
		}
		else
		{
			values[i] /= coefficient;
		}
	}
}

void WaveletTransform::orderValues(vector<double>& values)
{
	vector<double> temp(values.size());

	for (int i = 0; i < values.size(); ++i)
	{
		if (i % 2 == 0)
		{
			temp[i >> 1] = values[i];
		}
		else
		{
			temp[(values.size() >> 1) + (i >> 1)] = values[i];
		}
	}
	for (int i = 0; i < values.size(); ++i)
	{
		values[i] = temp[i];
	}
}

void WaveletTransform::disorderValues(vector<double>& values)
{
	vector<double> temp(values.size());
	for (int i = 0; i < (values.size() >> 1); ++i)
	{
		temp[i << 1] = values[i];
		temp[(i << 1) + 1] = values[i + (values.size() >> 1)];
	}
	for (int i = 0; i < values.size(); ++i)
	{
		values[i] = temp[i];
	}
}

void WaveletTransform::denormalizeStep(vector<double>& values)
{
	const double coefficient = 1.149604398;
	for (int i = 0; i < values.size(); ++i)
	{
		if (i % 2 == 1)
		{
			values[i] *= coefficient;
		}
		else
		{
			values[i] /= coefficient;
		}
	}
}