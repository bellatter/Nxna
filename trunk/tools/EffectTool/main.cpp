#include <fstream>
#include <sstream>
#include <iostream>
//#include "HLSLCrossCompiler/include/hlslcc.hpp"
#include "EffectXml.h"
#include "EffectWriter.h"
#include "XnbWriter.h"
#include "EffectToolException.h"
#include "HeaderConverter.h"
#include "CommandLineArgs.h"


struct RunOptions
{
	bool BuildXnb;
	bool BuildHeader;
	bool VerboseOutput;
	std::string InputFile;
	std::string OutputFile;
	std::string FXCPath;
	std::string HeaderArrayName;
	std::vector<Profile::eProfile> IgnoredProfiles;

	std::string Message;

	RunOptions()
	{
		BuildXnb = false;
		BuildHeader = false;
		VerboseOutput = false;
	}
};

bool parseCommandLine(int argc, char** argv, RunOptions* result, char* errorMessage, int maxErrorMessageLength)
{
	RunOptions options;
	CommandLineArgs args(argc, argv);

	const int LOOKING_FOR_EXE = 0;
	const int LOOKING_FOR_FLAGS = 1;
	const int LOOKING_FOR_INPUT = 2;
	const int LOOKING_FOR_OUTPUT = 3;
	const int DONE = 4;

	int state = LOOKING_FOR_EXE;
	const char* nextArg = args.GetNext();

	while(nextArg != nullptr)
	{
		if (state == LOOKING_FOR_EXE)
		{
			// just skip it
			nextArg = args.GetNext();

			state = LOOKING_FOR_FLAGS;
			continue;
		}

		if (state == DONE)
		{
			// we shouldn't be here. Too many arguments.
			strncpy(errorMessage, "Too many arguments", maxErrorMessageLength);
			return false;
		}

		if (state == LOOKING_FOR_FLAGS)
		{
			if (nextArg[0] == '-')
			{
				if (strcmp(nextArg, "-x") == 0)
					options.BuildXnb = true;
				else if (strcmp(nextArg, "-h") == 0)
					options.BuildHeader = true;
				else if (strcmp(nextArg, "-hn") == 0)
				{
					if (args.PeekNext() != nullptr)
						options.HeaderArrayName = args.GetNext();
					else
					{
						strncpy(errorMessage, "Expected an array name", maxErrorMessageLength);
						return false;
					}
				}
				else if (strcmp(nextArg, "-fxc") == 0)
				{
					if (args.PeekNext() != nullptr)
						options.FXCPath = args.GetNext();
					else
					{
						strncpy(errorMessage, "Expected a path to FXC", maxErrorMessageLength);
						return false;
					}
				}
				else if (strcmp(nextArg, "-ip") == 0)
				{
					if (args.PeekNext() != nullptr)
					{
						auto ignoredProfile = Profile::Parse(args.GetNext());
						if (ignoredProfile == Profile::PROFILE_UNKNOWN)
						{
							strncpy(errorMessage, "Unknown profile", maxErrorMessageLength);
							return false;
						}

						options.IgnoredProfiles.push_back(ignoredProfile);
					}
					else
					{
						strncpy(errorMessage, "Expected a profile name", maxErrorMessageLength);
						return false;
					}
				}
				else if (strcmp(nextArg, "-v") == 0)
					options.VerboseOutput = true;
				else
				{
					strncpy(errorMessage, "Unknown option", maxErrorMessageLength);
					return false;
				}
			}
			else
			{
				state = LOOKING_FOR_INPUT;
			}
		}

		if (state == LOOKING_FOR_INPUT)
		{
			options.InputFile = nextArg;

			state = LOOKING_FOR_OUTPUT;
		}
		else if (state == LOOKING_FOR_OUTPUT)
		{
			options.OutputFile = nextArg;

			state = DONE;
		}

		nextArg = args.GetNext();
	}

	// we should have gotten to at least the LOOKING_FOR_OUTPUT state
	if (state == LOOKING_FOR_INPUT)
	{
		strncpy(errorMessage, "Input file name is required", maxErrorMessageLength);
		return false;
	}
	if (state == LOOKING_FOR_OUTPUT)
	{
		// we weren't given an output file, so we'll just have to figure it out for ourselves.
		if (options.BuildHeader)
			options.OutputFile = "effecttooloutput.h";
		else if (options.BuildXnb)
			options.OutputFile = "effecttooloutput.xnb";
		else
			options.OutputFile = "effecttooloutput.nxfxo";
	}

	if (result != nullptr)
		*result = options;

	return true;
}

int main(int argc, char** argv)
{
	// usage: EffectTool.exe effect.xml output.xnb
	RunOptions options;
	char errorBuffer[256];
	if (parseCommandLine(argc, argv, &options, errorBuffer, 255) == false)
	{
		errorBuffer[255] = 0;

		std::cout << "Error: " << errorBuffer << std::endl;
		std::cout << "Usage: " << argv[0] << " [-x] [-h] [-hn ARRAY_NAME] [-ip PROFILE_NAME] INPUTFILE OUTPUTFILE" << std::endl;
		std::cout << "\t-x  - Write the output as an XNB file" << std::endl;
		std::cout << "\t-h  - Write the output as a header file (can be combined with -x)" << std::endl;
		std::cout << "\t-hn - Specifies the name of the byte array inside the header (has no effect if -h isn't specified)" << std::endl;
		std::cout << "\t-ip - Ignores any shaders with the specified profile and doesn't include them in the output" << std::endl;
		std::cout << std::endl;

		return -1;
	}

	if (options.VerboseOutput)
	{
		std::cout << "Input file: " << options.InputFile.c_str() << std::endl;
		std::cout << "Output file: " << options.OutputFile.c_str() << std::endl;
	}

	try
	{
		std::ifstream input;
		input.open(options.InputFile.c_str());
		if (input.is_open() == false)
			throw EffectToolException("Unable to open input file", options.InputFile.c_str());

		std::string line;
		std::stringstream ss;
		while(std::getline(input, line))
		{
			ss << line << std::endl;
		}
		input.close();


		EffectXml effect(ss.str());

		// remove any excluded profiles
		for (auto itr = options.IgnoredProfiles.begin(); itr != options.IgnoredProfiles.end(); itr++)
			effect.RemoveProfile(*itr);

		// remove any shaders that aren't being used
		effect.RemoveUnusedShaders();


		std::string pathToBlah;
		auto lastSlash = options.InputFile.find_last_of("/\\");
		if (lastSlash == std::string::npos)
			pathToBlah = ".";
		else
			pathToBlah = options.InputFile.substr(0, lastSlash);

		effect.LoadExternalShaders(pathToBlah.c_str());

		ResizableBuffer buffer;

		if (options.BuildXnb)
		{
			XnbWriter xnb(&buffer);

			EffectWriter::Write(effect, &buffer);

			xnb.Finalize();
		}
		else
		{
			EffectWriter::Write(effect, &buffer);
		}

		if (options.BuildHeader)
		{
			const char* arrayName;
			if (options.HeaderArrayName.empty())
				arrayName = "EffectBytecode";
			else
				arrayName = options.HeaderArrayName.c_str();

			ResizableBuffer output;
			HeaderConverter::Convert(arrayName, (const unsigned char*)buffer.GetData(), buffer.GetSize(), &output);

			output.SaveToFile(options.OutputFile.c_str());
		}
		else
		{
			buffer.SaveToFile(options.OutputFile.c_str());
		}
	}
	catch(std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}

	return 0;
}