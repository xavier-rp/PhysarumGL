#include "fftUtils.h"
#include <fftw3.h>
#include <iostream>

#include "utils.h"

float binWidth = (44100.0f / (static_cast<float>(samplesToStream) / 2.0f));

std::vector<float> computeFrequencyAmplitudes(std::vector<std::int16_t> fftBuffer) {
	double* in1;
	double* in2;
	fftw_complex* out1;
	fftw_complex* out2;
	fftw_plan p1;
	fftw_plan p2;

	in1 = (double*)fftw_malloc(sizeof(double) * fftBuffer.size()/2); //The size is devided by 2, because the buffer stores the samples of channel 1 and 2
	in2 = (double*)fftw_malloc(sizeof(double) * fftBuffer.size()/2);
	out1 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftBuffer.size()/2);
	out2 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftBuffer.size()/2);

	for (int i = 0; i < fftBuffer.size()/2; i++) {
		
		in1[i] = static_cast<double>(fftBuffer[2 * i])/32768.0f;
		in2[i] = static_cast<double>(fftBuffer[2 * i + 1])/ 32768.0f;
		

	}
	p1 = fftw_plan_dft_r2c_1d(fftBuffer.size() / 2, in1, out1, FFTW_ESTIMATE);

	fftw_execute(p1);

	p2 = fftw_plan_dft_r2c_1d(fftBuffer.size() / 2, in2, out2, FFTW_ESTIMATE);

	fftw_execute(p2);

	std::vector<float> amplitudes;

	for (int i = 0; i < fftBuffer.size() / 2; i++) {
		double amplitude1{ sqrt(out1[i][0] * out1[i][0] + out1[i][1] * out1[i][1]) };
		double amplitude2{ sqrt(out2[i][0] * out2[i][0] + out2[i][1] * out2[i][1]) };
		amplitudes.push_back(static_cast<float>((1.0 / (fftBuffer.size() / 2)) * (amplitude1 + amplitude2)));//amplitudes.push_back(static_cast<float>((1.0 / (fftBuffer.size() / 2)) * (amplitude1 + amplitude2)/2.0f));
	}

	fftw_destroy_plan(p1);
	fftw_free(in1);
	fftw_free(out1);
	fftw_destroy_plan(p2);
	fftw_free(in2);
	fftw_free(out2);

	return amplitudes;

}

float computeBassEnergy(std::vector<float>& amplitudes)
{
	int highestBassBin{ static_cast<int>(250.0f / binWidth) }; //Assumes a 44100 Hz sampling rate
	float energy{ 0.0 };

	for (int i = 0; i < highestBassBin; i++) {
		energy += amplitudes[i];
	}

	return energy;
}

float computeMidEnergy(std::vector<float>& amplitudes)
{
	int lowestMidBin{ static_cast<int>(250.0f / binWidth)};
	int highestMidBin{ static_cast<int>(2500.0f / binWidth) };
	float energy{ 0.0 };

	for (int i = lowestMidBin; i < highestMidBin; i++) {
		energy += amplitudes[i];
	}

	return energy;
}

float computeHighEnergy(std::vector<float>& amplitudes)
{
	float energy{ 0.0 };
	int lowestHighBin{ static_cast<int>(2500.0f / binWidth) };
	for (int i = lowestHighBin; i < amplitudes.size(); i++) {
		energy += amplitudes[i];
	}

	return energy;
}