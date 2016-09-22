#ifndef g2remake_LogFile_h
#define g2remake_LogFile_h
#include <string>
#include <aw/log/log.h>

namespace g2 {

struct LogFile : aw::log {
	using aw::log::level;

	LogFile();
	~LogFile() override;
	void message(level lvl, std::string const& src, std::string const& msg) override;

private:
	void* logfile;
};

} // namespace g2
#endif//g2remake_LogFile_h
