#ifndef NXNA_EXCEPTION_H
#define NXNA_EXCEPTION_H

#include <string>

namespace Nxna
{
class Exception
{
protected:
	std::string m_message;
	std::string m_file;
	int m_line;

public:
	Exception(const std::string& msg)
	{
		m_message = msg;

		m_file = "Unknown";
		m_line = 0;
	}

	Exception(const char* msg)
	{
		m_message = msg;

		m_file = "Unknown";
		m_line = 0;
	}

	Exception(const std::string& msg, const char* file, int line)
	{
		m_message = msg;

		m_file = file;
		m_line = line;
	}

	Exception(const char* msg, const char* file, int line)
	{
		m_message = msg;

		m_file = file;
		m_line = line;
	}

	const std::string& GetMessage() { return m_message; }
	const std::string& GetFile() { return m_file; }
	const int GetLine() { return m_line; }
};

class ArgumentException : public Exception
{
public:
	ArgumentException(const std::string& parameter)
		: Exception("Invalid argument: " + parameter)
	{
	}

	ArgumentException(const std::string& message, const std::string& parameter)
		: Exception(message + ": " + parameter)
	{
	}

	ArgumentException(const std::string& message, const std::string& parameter, const char* file, int line)
		: Exception(message + ": " + parameter, file, line)
	{
	}
};

	class InvalidOperationException : public Exception
	{
	public:
		InvalidOperationException()
			: Exception("An invalid operation has occurred")
		{
		}

		InvalidOperationException(const char* message)
			: Exception(message)
		{
		}

		InvalidOperationException(const char* message, const char* file, int line)
			: Exception(message, file, line)
		{
		}
	};
}

#endif // NXNA_EXCEPTION_H