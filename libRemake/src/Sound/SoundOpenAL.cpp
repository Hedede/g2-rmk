#include "SoundOpenAL.h"

#include <aw/fileformat/wav/reader.h>
#include <stdexcept>

//------------------------------------------------------------------------------
namespace g2 {
SoundOpenAL::SoundOpenAL()
{
	Log("OpenAL", "Creating audio device");
	dev = alcOpenDevice(nullptr);
	if (!dev)
		Fatal( "OpenAL", "could not open default device" );
	Log("OpenAL", "Creating context");
	ctx = alcCreateContext(dev, nullptr);
	if (!ctx)
		Fatal( "could not create context" );
	if (!MakeContextCurrent())
		Fatal(  "failed to make context current" );

	alcGetIntegerv( dev, ALC_MONO_SOURCES, 1, &attrib.max_sources );
	Log( "OpenAL", "ALC_MONO_SOURCES = " + std::to_string(attrib.max_sources) );


	Log("OpenAL", "OpenAL is initialized");
}

bool SoundOpenAL::MakeContextCurrent()
{
	if (!alcMakeContextCurrent(ctx))
		return false;
	if (alcGetError(dev) != ALC_NO_ERROR)
		return false;
	return true;
}

SoundOpenAL::~SoundOpenAL()
{
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
}

int SoundOpenAL::max_sources() const
{
	return attrib.max_sources;
}

void SoundOpenAL::play( Source& src )
{
	alSourcePlay(src.handle);
}

//------------------------------------------------------------------------------
Source::Source()
{
	alGenSources(1, &handle);
	if (alGetError() != AL_NO_ERROR) {
		Warning( "SFX", "could not create sound handle" );
		return;
	}
	
	alSourcei(handle,  AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcef(handle,  AL_PITCH, 1);
	alSourcef(handle,  AL_GAIN, 1);
	alSource3f(handle, AL_POSITION, 0, 2, 0);
	alSource3f(handle, AL_VELOCITY, 0, 0, 0);
	alSourcei(handle,  AL_LOOPING, AL_FALSE);
}

void Source::set_buffer(Buffer& buf)
{
	alSourcei(handle, AL_BUFFER, buf.get_handle());
	if (alGetError() != AL_NO_ERROR) {
		g2::Warning( "SFX", "could not bind buffer to source" );
		return;
	}
}

//------------------------------------------------------------------------------
Buffer::Buffer()
{
	alGenBuffers(1, &handle);
	if (alGetError() != AL_NO_ERROR) {
		g2::Warning( "SFX", "could not create buffer" );
		return;
	}
}

ALenum to_al_format_pcm(short channels, short samples)
{
        bool stereo = (channels > 1);

        switch (samples) {
        case 16:
                if (stereo)
                        return AL_FORMAT_STEREO16;
                else
                        return AL_FORMAT_MONO16;
        case 8:
                if (stereo)
                        return AL_FORMAT_STEREO8;
                else
                        return AL_FORMAT_MONO8;
        default:
                return -1;
        }
}

using wave_fmt = aw::wav::format;

ALenum to_al_format( aw::wav::wave_data& wave)
{

	switch ( wave.format ) {
	case wave_fmt::pcm:
		return to_al_format_pcm( wave.channels, wave.bits_per_sample() );
	case wave_fmt::ima_adpcm:
		if (wave.channels > 1)
			return AL_FORMAT_STEREO_IMA4;
		else
			return AL_FORMAT_MONO_IMA4;
	default:
		return -1;
	}
}


bool Buffer::set_data(aw::wav::wave_data& wave)
{
	auto fmt = to_al_format(wave);
	if (fmt == -1) {
		g2::Warning( "SFX", "unsupported format" );
		return false;
	}

	if (wave.format == wave_fmt::ima_adpcm)
		alBufferi( handle, AL_UNPACK_BLOCK_ALIGNMENT_SOFT, wave.samples_per_block() );

	alBufferData(handle, fmt, wave.data.data(), wave.data.size(), wave.sample_rate);
	if (alGetError() != AL_NO_ERROR) {
		//cleanup();
		g2::Warning( "SFX", "could not fill buffer with wave data" );
		return false;
	}
	return true;
}
} // namespace g2
