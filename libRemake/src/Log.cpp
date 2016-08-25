#include <Logging/Log.h>
#include <Logging/LogFile.h>
#include <cstdio>

namespace g2r {
aw::LogProvider logger;

LogFile::LogFile()
{
	logfile = (void*)std::fopen("log.txt", "wb");
}

LogFile::~LogFile()
{
	if (logfile)
		std::fclose((FILE*)logfile);
}

void LogFile::log(Level level, std::string const& src, std::string const& msg)
{
	if (logfile) {
		constexpr size_t nsrc = 16;
		constexpr size_t extra = sizeof("xxxx : ") + sizeof(" : ") + sizeof("\n");
		std::string str;
		str.reserve(nsrc + extra + msg.size());
		str += src;
		str.resize(nsrc, ' ');
		switch (level) {
		case Info:
			str += "Info : ";
			break;
		case Warning:
			str += "Wrng : ";
			break;
		case Error:
			str += "Errr : ";
			break;
		case Critical:
			str += "Crit : ";
			break;
		default:
			str += "Unkn : ";
			break;
		};
		str += src;
		str += " : ";
		str += msg;
		str += "\n";
		std::fwrite(str.data(), str.size(), 1, (FILE*)logfile);
	}
}
} // namespace g2r
