#include "pch.h"
#include "Compressor.h"


Compressor::Compressor()
{
	walvetSeries = 2;
	blockSize = 32;
	blockData = new vector<Block>();
}


Compressor::~Compressor()
{
	delete blockData;
}

void Compressor::transform(cv::Mat& src)
{
	for (int level = 0; level < walvetSeries; ++level)
	{
		horizontalTransformation(src, level);
		//verticalTransformation(src, level);
	}
}

void Compressor::inverseTransform(cv::Mat& src)
{
	for (int level = walvetSeries - 1; level >= 0; --level)
	{
		verticalTransformation(src, level, true);
		horizontalTransformation(src, level, true);
	}
}

void Compressor::horizontalTransformation(cv::Mat& src, int level, bool inverse)
{
	vector<double> data(src.cols >> level);
	for (int i = 0; i < (src.rows >> level); ++i)
	{

		for (int j = 0; j < (src.cols >> level); ++j)
		{
			data[j] = src.at<double>(i, j);
		}
		if (inverse)
		{
			inverseWalvetTranform97(data);
		}
		else
		{
			forwardWalvetTransform97(data);
		}
		for (int j = 0; j < (src.cols >> level); ++j)
		{
			src.at<double>(i, j) = data[j];
		}
	}
}

void Compressor::verticalTransformation(cv::Mat& src, int level, bool inverse)
{
	vector<double> data(src.rows >> level);
	for (int i = 0; i < (src.cols >> level); ++i)
	{
		for (int j = 0; j < (src.rows >> level); ++j)
		{
			data[j] = src.at<double>(j, i);
		}
		if (inverse)
		{
			inverseWalvetTranform97(data);
		}
		else
		{
			forwardWalvetTransform97(data);
		}
		for (int j = 0; j < (src.rows >> level); ++j)
		{
			src.at<double>(j, i) = data[j];
		}
	}
}

void Compressor::setWalvetSeries(int walvetSeries)
{
	if (walvetSeries > 0)
	{
		this->walvetSeries = walvetSeries;
	}
	else
	{
		throw std::out_of_range("Number of walvet series must be a positive integer number");
	}
}

void Compressor::setBlockSize(int blockSize)
{
	if (blockSize > 1)
	{
		this->blockSize = blockSize;
	}
	else
	{
		throw std::out_of_range("Block size must be a positive integer number");
	}
}

void Compressor::setCompressionLevel(int compressionLevel)
{
	if (compressionLevel >= 0 && compressionLevel < 100)
	{
		this->compressionLevel = compressionLevel;
	}
	else
	{
		throw std::out_of_range("Compression level must be an integer value between 0 and 99");
	}
}

void Compressor::forwardWalvetTransform97(vector<double>& values)
{
	predictionStep(values, -1.586134342);
	updateStep(values, -0.05298011854);

	predictionStep(values, 0.8829110762);
	updateStep(values, 0.4435068522);

	normalizeStep(values);
	orderValues(values);
}

void Compressor::inverseWalvetTranform97(vector<double>& values)
{
	disorderValues(values);
	denormalizeStep(values);

	updateStep(values, -0.4435068522);
	predictionStep(values, -0.8829110762);

	updateStep(values, 0.05298011854);
	predictionStep(values, 1.586134342);
}

void Compressor::predictionStep(vector<double>& values, const double coefficient)
{
	for (int i = 1; i < values.size() - 2; i += 2)
	{
		values[i] += coefficient * (values[i - 1] + values[i + 1]);
	}
	values[values.size() - 1] += 2 * coefficient * values[values.size() - 2];
}

void Compressor::updateStep(vector<double>& values, const double coefficient)
{
	for (int i = 2; i < values.size(); i += 2)
	{
		values[i] += coefficient * (values[i - 1] + values[i + 1]);
	}
	values[0] += 2 * coefficient * values[1];
}

void Compressor::normalizeStep(vector<double>& values)
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

void Compressor::orderValues(vector<double>& values)
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
			temp[(values.size() >> 1)  + (i >> 1)] = values[i];
		}
	}
	for (int i = 0; i < values.size(); ++i)
	{
		values[i] = temp[i];
	}
}

void Compressor::disorderValues(vector<double>& values)
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

void Compressor::denormalizeStep(vector<double>& values)
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

void Compressor::computePropertiesForAllBlocks(cv::Mat& src)
{
	for (int i = 0; i < (src.rows >> walvetSeries); i += blockSize)
	{
		for (int j = 0; j < (src.cols >> walvetSeries); j += blockSize)
		{
			Block block;
			block.computeProperties(src, i, j, (src.rows >> walvetSeries), (src.cols >> walvetSeries), blockSize, blockData->size());
			blockData->push_back(block);
			//std::cout << blockEnergy(src, i, j, (src.rows >> LEVEL), (src.cols >> LEVEL), blockSize) << '\n';
		}
	}
	for (int level = walvetSeries; level >= 1; --level)
	{
		const int zoneWidth = src.rows >> level;
		const int zoneHeight = src.cols >> level;
		for (int zone = 1; zone < 4; ++zone)
		{
			const int offsetX = (zone % 2) * zoneWidth;
			const int offsetY = (zone >> 1) * zoneHeight;
			for (int i = 0; i < zoneWidth; i += blockSize)
			{
				for (int j = 0; j < zoneHeight; j += blockSize)
				{
					Block block;
					block.computeProperties(src, i + offsetX, j + offsetY, offsetX + zoneWidth, offsetY + zoneHeight, blockSize, blockData->size());
					blockData->push_back(block);
					//std::cout << blockEnergy(src, i + offsetX, j + offsetY, offsetX + rows, offsetY + cols, blockSize) << '\n';
				}
			}
		}
	}
}

void Compressor::reduceNumberOfBlocksByLevelOfCompression()
{
	const double percentageOfRemainingBlocks = 1.0 - ((double)compressionLevel * 0.01);
	blockData->resize(blockData->size() * percentageOfRemainingBlocks);
}

void Compressor::sortBlocksByEnergy()
{
	sort(blockData->begin(), blockData->end(),
		[](const Block& b1, const Block& b2) -> bool { return b1.GetEnergy() > b2.GetEnergy(); });
}

void Compressor::sortBlocksByOrder()
{
	sort(blockData->begin(), blockData->end(),
		[](const Block& b1, const Block& b2) -> bool { return b1.GetOrder() > b2.GetOrder(); });
}
#include <iostream>
double Compressor::GetDataEnergy()
{
	double sum = 0;
	for (int i = 0; i < blockData->size(); ++i)
	{
		//std::cout << blockData->at(i).GetEnergy() << endl;
		sum += blockData->at(i).GetEnergy();
	}
	return sum;
}


