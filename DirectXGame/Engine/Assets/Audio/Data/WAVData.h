#pragma once
#include "AudioData.h"

class WAVData : public AudioData {
public:

	WAVData() = default;
	~WAVData() = default;

	void Load(const std::filesystem::path& filepath) override;
	int Play(IXAudio2* xAudio, bool isLoop) override;

private:

};

