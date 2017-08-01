#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <memory>

#include "sdsingleton.h"
#include "Print.h"


enum LogLevel
{
	LOGLEVEL_ERROR,
	LOGLEVEL_WARNING,
	LOGLEVEL_NOTICE,
	LOGLEVEL_INFO,
	LOGLEVEL_LOG,
	LOGLEVEL_DEBUG
};

const enum LogLevel DefaultLevel = LOGLEVEL_DEBUG;

class Logger : public Crown::Singleton<Logger, Crown::WITH_DEFAULT_CONSTRUCTOR>
{
	//std::unique_ptr<std::ostream> m_OStream;

	enum LogLevel m_level;
public:
	static const char* STDOUT;

	Logger(const std::string& s = STDOUT, enum LogLevel = DefaultLevel);

	Logger& SetLevel(enum LogLevel level) { m_level = level; return *this; }

	static void ResetInstanceToStdout(enum LogLevel = DefaultLevel);
	static void ResetInstance(const std::string& logfilename, enum LogLevel = DefaultLevel);

	// FIXME: performance issue, the the input would be still formatted.
	static std::ofstream NullOStream;

	static std::ostream& D(); // Debug.
	static std::ostream& L(); // Log.
	static std::ostream& I(); // Info.
	static std::ostream& N(); // Notice.
	static std::ostream& W(); // Warning.
	static std::ostream& E(); // Error.

#ifndef WIN
/*
#define PRINT_METHOD(shortname) \
	template <typename ... Tail> \
	static void shortname(const char* s, const Tail&... args) \
	{ \
		std::ostream& out = shortname(); \
		Crown::Printf(out, s, args...); \
		out << std::endl; \
		out.flush(); \
	}

	PRINT_METHOD(D)
	PRINT_METHOD(L)
	PRINT_METHOD(I)
	PRINT_METHOD(N)
	PRINT_METHOD(W)
	PRINT_METHOD(E)

#undef PRINT_METHOD
*/
#endif //WIN

	static std::ostream& GetOutStream();
	static std::ostream& GetErrStream();

	static void PY(const char* pylog, bool isErr=false);
};

#define LOG_D Logger::D()
#define LOG_L Logger::L()
#define LOG_I Logger::I()
#define LOG_N Logger::N()
#define LOG_W Logger::W()
#define LOG_E Logger::E()

#endif // LOGGING_H
