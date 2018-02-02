#ifndef G2Remake_OpenALSource_H
#define G2Remake_OpenALSource_H
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
	Source( unsigned handle );
	void set_buffer(Buffer& buf);
	unsigned get_handle()
	{
		return handle;
	}

	unsigned handle;
};
} // namespace g2
#endif//G2Remake_OpenALSource_H
