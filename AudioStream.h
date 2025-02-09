#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

class AudioStream : public sf::SoundStream
{
public:
	std::vector<std::int16_t> m_samples;
	std::size_t               m_currentSample{};
	std::vector<std::int16_t> fftBuffer;
	int samplesToStream;

	AudioStream(const int samplesToStream);

	void load(const sf::SoundBuffer& buffer);

	bool onGetData(Chunk& data) override;

	void onSeek(sf::Time timeOffset) override;
};