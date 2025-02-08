#pragma once
#include <vector>
#include <SFML/Audio.hpp>

std::vector<float> computeFrequencyAmplitudes(std::vector<std::int16_t> fftBuffer);

float computeBassEnergy(std::vector<float>& amplitudes);

float computeMidEnergy(std::vector<float>& amplitudes);

float computeHighEnergy(std::vector<float>& amplitudes);