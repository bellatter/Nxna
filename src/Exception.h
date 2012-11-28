#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

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

	Exception(const std::string& msg, const char* file, int line)
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

#endif // EXCEPTION_H