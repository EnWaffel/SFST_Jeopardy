#pragma once
#include <string>

class Sound
{
private:
	unsigned int buffer;
	unsigned int source;
public:
	Sound(const std::string& path);
	~Sound();

	void Play();
	void Pause();
	void Resume();
	void Stop();

	void SetLooping(bool looping);
public:
	static void SetVolume(float v);
};