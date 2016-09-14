#ifndef g2remake_Log_h
#define g2remake_Log_h
#include <aw/logger/LogProvider.h>
#include <aw/utility/to_string.h>

namespace g2 {
extern aw::LogProvider logger;

template<typename... Args>
void Log(std::string const& src, Args... args)
{
	using namespace std::string_literals;
	std::string msg = ( aw::to_string(args) + ... + "\n"s );
	logger.log(aw::Log::Info, src, msg);
}

template<typename... Args>
void Warning(std::string const& src, Args... args)
{
	using namespace std::string_literals;
	std::string msg = ( aw::to_string(args) + ... + "\n"s );
	logger.log(aw::Log::Warning, src, msg);
}

template<typename... Args>
void Error(std::string const& src, Args... args)
{
	using namespace std::string_literals;
	std::string msg = ( aw::to_string(args) + ... + "\n"s );
	logger.log(aw::Log::Error, src, msg);
}

template<typename... Args>
void Fatal(std::string const& src, Args... args)
{
	using namespace std::string_literals;
	std::string msg = ( aw::to_string(args) + ... + "\n"s );
	logger.log(aw::Log::Critical, src, msg);
}
} // namespace g2
#endif//g2remake_Log_h
