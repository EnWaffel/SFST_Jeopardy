#include <iostream>
#include "Sound.h"
#include <stb_vorbis.c>
#include "Logger.h"
#include <AL/al.h>

static float volume;

Sound::Sound(const std::string& path) : buffer(0), source(0)
{
	FILE* f;
	fopen_s(&f, path.c_str(), "rb");

	int err = VORBIS__no_error;
	stb_vorbis* v = stb_vorbis_open_file(f, true, &err, NULL);
	if (err != VORBIS__no_error)
	{
		LogWarning("Could not load sound: " + path);
		return;
	}

	stb_vorbis_info i = stb_vorbis_get_info(v);

	int dataSize = stb_vorbis_stream_length_in_samples(v) * i.channels * 2;
	short* data = new short[dataSize];

	stb_vorbis_get_samples_short_interleaved(v, i.channels, data, dataSize);

	alGenBuffers(1, &buffer);
	alGenSources(1, &source);

	int format = AL_INVALID;
	if (i.channels == 1)
	{
		format = AL_FORMAT_MONO16;
	}
	else if (i.channels == 2)
	{
		format = AL_FORMAT_STEREO16;
	}
	else
	{
		LogWarning("Sound File [" + path + "] has more than 2 channels!");
		return;
	}

	alBufferData(buffer, format, data, dataSize, i.sample_rate);

	alSourcei(source, AL_BUFFER, buffer);
	alSourcef(source, AL_GAIN, volume);

	stb_vorbis_close(v);
	delete[] data;
}

Sound::~Sound()
{
	Stop();
	alDeleteBuffers(1, &buffer);
	alDeleteSources(1, &source);
}

void Sound::Play()
{
	alSourcePlay(source);
}

void Sound::Pause()
{
	alSourcePause(source);
}

void Sound::Resume()
{
	alSourcePlay(source);
}

void Sound::Stop()
{
	alSourceStop(source);
}

void Sound::SetLooping(bool looping)
{
	alSourcei(source, AL_LOOPING, looping);
}

void Sound::SetVolume(float v)
{
	volume = v;
}
