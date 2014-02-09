#ifndef EFFECTTOOLEXCEPTION_H
#define EFFECTTOOLEXCEPTION_H

#include <exception>
#include <string>

class EffectToolException : public std::exception
{
	std::string m_message;

public:
	EffectToolException(const char* message)
	{
		m_message = message;
	}

	EffectToolException(const char* message, const char* extra)
	{
		m_message = std::string(message) + ": " + extra;
	}

	virtual const char* what() const throw() override
	{
		return m_message.c_str();
	}
};

#endif // EFFECTTOOLEXCEPTION_H
