#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class Exception
{
protected:
	std::string m_message;

public:
	Exception(const std::string& msg)
	{
		m_message = msg;
	}

	const std::string& GetMessage() { return m_message; }
};

#endif // EXCEPTION_H