#include <Logging/Log.h>
#include <Logging/LogFile.h>
#include <cstdio>

namespace g2 {
aw::log_provider logger;

LogFile::LogFile()
{
	logfile = (void*)std::fopen("log.txt", "wb");
}

LogFile::~LogFile()
{
	if (logfile)
		std::fclose((FILE*)logfile);
}

void LogFile::message(level lvl, std::string const& src, std::string const& msg)
{
	if (logfile) {
		constexpr size_t nsrc = 32;
		constexpr size_t extra = sizeof(" :") + sizeof(" xxxx : ") + sizeof("\n");
		std::string str;
		str.reserve(nsrc + extra + msg.size());
		str += src;
		str += " :";
		str.resize(nsrc + sizeof(" :"), ' ');
		switch (lvl) {
		case info:
			str += " Info : ";
			break;
		case warning:
			str += " Wrng : ";
			break;
		case error:
			str += " Errr : ";
			break;
		case critical:
			str += " Crit : ";
			break;
		default:
			str += " Unkn : ";
			break;
		};
		str += msg;
		str += "\n";
		std::fwrite(str.data(), str.size(), 1, (FILE*)logfile);
	}
}
} // namespace g2
