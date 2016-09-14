#ifndef g2remake_LogFile_h
#define g2remake_LogFile_h
#include <string>
#include <aw/logger/Log.h>

namespace g2 {

struct LogFile : aw::Log {
	using aw::Log::Level;

	LogFile();
	~LogFile() override;
	void log(Level level, std::string const& src, std::string const& msg) override;

private:
	void* logfile;
};

} // namespace g2
#endif//g2remake_LogFile_h
