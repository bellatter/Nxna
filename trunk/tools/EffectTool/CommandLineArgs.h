#ifndef COMMANDLINEARGS_H
#define COMMANDLINEARGS_H

class CommandLineArgs
{
	int m_position;
	int m_count;
	char** m_args;

public:
	CommandLineArgs(int argc, char** argv)
	{
		m_position = 0;
		m_count = argc;
		m_args = argv;
	}

	const char* GetNext()
	{
		if (m_position >= m_count)
			return nullptr;

		return m_args[m_position++];
	}

	const char* PeekNext()
	{
		if (m_position >= m_count)
			return nullptr;

		return m_args[m_position];
	}
};

#endif // COMMANDLINEARGS_H
