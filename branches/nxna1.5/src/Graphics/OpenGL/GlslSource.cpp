#include <cstring>
#include "OpenGL.h"
#include "../GraphicsDevice.h"
#include "GlslSource.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	GlslSource::GlslSource(const char* vertexSource, const char* fragmentSource, int glslVersion)
	{
		int major = glslVersion / 100;
		int minor = (glslVersion - major * 100) / 10;

		strcpy(m_versionDefinition, "#version 000\n");
		strcpy(m_versionConstant, "#define VERSION 000\n");
		m_versionDefinition[9] = (char)(major + '0');
		m_versionDefinition[10] = (char)(minor + '0');
		m_versionConstant[16] = (char)(major + '0');
		m_versionConstant[17] = (char)(minor + '0');

		m_vertexSource = vertexSource;
		m_fragmentSource = fragmentSource;
		m_glslVersion = glslVersion;
	}

	unsigned int GlslSource::Build(const char* defines[], int numDefines)
	{
		int vertexShader = compile(m_vertexSource, defines, numDefines, true);
		int fragmentShader = compile(m_fragmentSource, defines, numDefines, false);

		unsigned int program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		int result;
		glGetProgramiv(program, GL_LINK_STATUS, &result);

		char buffer[256];
		int len;
		glGetProgramInfoLog(program, 255, &len, buffer);
		buffer[255] = 0;
		if (result != GL_TRUE)
		{
			throw GraphicsException(std::string("Error while linking GLSL program: ") + buffer);
		}

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			throw GraphicsException("Error while loading GLSL effect");

		return program;
	}

	int GlslSource::compile(const char* source, const char* defines[], int numDefines, bool isVertexShader)
	{
		int shader;
		if (isVertexShader)
		{
			shader = glCreateShader(GL_VERTEX_SHADER);
		}
		else
		{
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		}
        if (shader == 0)
            throw GraphicsException("Unable to create shader");

		// setup the source
		int totalDefines = 0;
		m_defines[totalDefines++] = m_versionDefinition;
		m_defines[totalDefines++] = m_versionConstant;
		if (m_glslVersion == 100)
			m_defines[totalDefines++] ="#define HIGHP highp\n";
		else
			m_defines[totalDefines++] = "#define HIGHP\n";
		if (m_glslVersion < 130)
		{
			if (isVertexShader)
			{
				m_defines[totalDefines++] = "#define in attribute\n";
				m_defines[totalDefines++] = "#define out varying\n";
			}
			else
			{
				m_defines[totalDefines++] = "#define in varying\n";
			}
		}
		for (int i = 0; i < numDefines; i++)
			m_defines[totalDefines++] = defines[i];
		m_defines[totalDefines++] = source;

		glShaderSource(shader, totalDefines, m_defines, nullptr);

		glCompileShader(shader);

		int logLength;
		char logBuffer[256];
		glGetShaderInfoLog(shader, 255, &logLength, logBuffer);
		logBuffer[255] = 0;

		int result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (result != GL_TRUE)
			throw GraphicsException(std::string("Unable to compile shader: ") + logBuffer);

		return shader;
	}
}
}
}
