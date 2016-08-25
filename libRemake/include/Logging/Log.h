#ifndef g2remake_Log_h
#define g2remake_Log_h
#include <aw/logger/LogProvider.h>
#include <aw/utility/string/as_string.h>

namespace g2r {
extern aw::LogProvider logger;

template<typename... Args>
void Log(std::string const& src, Args... args)
{
	std::string msg = ( aw::as_string(args) + ... + std::string{} );
	logger.log(aw::Log::Info, src, msg);
}

template<typename... Args>
void Warning(std::string const& src, Args... args)
{
	std::string msg = ( aw::as_string(args) + ... + std::string("\n") );
	logger.log(aw::Log::Warning, src, msg);
}

template<typename... Args>
void Error(std::string const& src, Args... args)
{
	std::string msg = ( aw::as_string(args) + ... + std::string{} );
	logger.log(aw::Log::Error, src, msg);
}

template<typename... Args>
void Fatal(std::string const& src, Args... args)
{
	std::string msg = ( aw::as_string(args) + ... + std::string("\n") );
	logger.log(aw::Log::Critical, src, msg);
}
} // namespace g2r
#endif//g2remake_Log_h
