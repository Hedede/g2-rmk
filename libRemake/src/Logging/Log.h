#ifndef g2remake_Log_h
#define g2remake_Log_h
#include <aw/log/log_provider.h>
#include <aw/utility/to_string.h>

namespace g2 {
extern aw::log_provider logger;

template<typename... Args>
void Log(std::string const& src, Args&&... args)
{
	using aw::to_string;
	std::string msg = ( to_string(std::forward<Args>(args)) + ... + "" );
	logger.info(src, msg);
}

template<typename... Args>
void Warning(std::string const& src, Args&&... args)
{
	using aw::to_string;
	std::string msg = ( to_string(std::forward<Args>(args)) + ... + "" );
	logger.warning(src, msg);
}

template<typename... Args>
void Error(std::string const& src, Args&&... args)
{
	using aw::to_string;
	std::string msg = ( to_string(std::forward<Args>(args)) + ... + "" );
	logger.error(src, msg);
}

template<typename... Args>
void Fatal(std::string const& src, Args&&... args)
{
	using aw::to_string;
	std::string msg = ( to_string(std::forward<Args>(args)) + ... + "" );
	logger.fatal(src, msg);
}
} // namespace g2
#endif//g2remake_Log_h
