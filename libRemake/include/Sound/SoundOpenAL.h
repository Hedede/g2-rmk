#ifndef G2Remake_SoundOpenAL_H
#define G2Remake_SoundOpenAL_H
#include <string>
#include <memory>
#include <Logging/Log.h>
//#include <Gothic/Audio/zSound.h>
#include <Gothic/Audio/zSoundSystemDummy.h>

namespace aw::wav {
struct wave_data;
}

namespace g2 {
struct SoundOpenAL;

struct Buffer {
	Buffer();
	bool set_data(aw::wav::wave_data& wave);
	unsigned get_handle()
	{
		return handle;
	}

	unsigned handle;
};

struct Source {
	Source();
	void set_buffer(Buffer& buf);
	unsigned get_handle()
	{
		return handle;
	}

	unsigned handle;
};

struct SoundOpenAL {
	SoundOpenAL();
	~SoundOpenAL();

	void play( Source& src );

protected:
	struct Impl;
	static constexpr size_t impl_size  = 0x20;
	//static constexpr auto   impl_align = alignof(void*);

	Impl& impl()
	{
		return Value<Impl>(&data);
	}

private:
	using storage = std::aligned_storage< impl_size >::type;
	storage data;
};
} // namespace g2
#endif//G2Remake_SoundOpenAL_H
