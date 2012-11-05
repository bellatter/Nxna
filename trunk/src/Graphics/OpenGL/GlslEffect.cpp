#include <cstring>
#include <cassert>
#include <sstream>
#include "OpenGL.h"
#include "GlslEffect.h"
#include "GlTexture2D.h"
#include "OpenGLDevice.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	char GlslEffect::m_attribNameBuffer[];
	int GlslEffect::m_boundProgramIndex = -1;

	GlslEffect::GlslEffect(OpenGlDevice* device, const char* source)
	{
		m_device = device;

		std::string vertexResult, fragResult;
		ProcessSource(source, vertexResult, fragResult);
		CreateProgram(vertexResult, fragResult, nullptr);
	}

	GlslEffect::GlslEffect(OpenGlDevice* device)
	{
		assert(device != nullptr);
		m_device = device;
	}

	GlslEffect::~GlslEffect()
	{
		// delete all the parameters
		for (int i = 0; i < m_parameterList.size(); i++)
		{
			EffectParameter* p = m_parameterList[i];
			delete p;
		}

		// delete all the programs
		for (int i = 0; i < m_programs.size(); i++)
		{
			glDeleteProgram(m_programs[i].Program);
		}
	}

	void GlslEffect::ProcessSource(const char* source, std::string& vertexResult, std::string& fragResult)
	{
		const char* vertex = strstr(source, "#vertex");
		const char* frag = strstr(source, "#fragment");

		assert(vertex != nullptr);
		assert(frag != nullptr);

		vertexResult = vertex + 7;
		fragResult = frag + 9;
		vertexResult.erase(vertexResult.length() - fragResult.length() - 9);

		processSource(vertexResult, true);
		processSource(fragResult, false);
	}

	void GlslEffect::CreateProgram(const std::string& vertexSource, const std::string& fragSource, const char* defines)
	{
		glGetError();

		int vertexShader = compile(vertexSource, defines, true);
		int fragShader = compile(fragSource, defines, false);

		GlslProgram program;
		program.Program = glCreateProgram();

		glAttachShader(program.Program, vertexShader);
		if (fragShader != 0) glAttachShader(program.Program, fragShader);

		glLinkProgram(program.Program);

		int result;
		glGetProgramiv(program.Program, GL_LINK_STATUS, &result);

		char buffer[256];
		int len;
		glGetProgramInfoLog(program.Program, 255, &len, buffer);
		buffer[255] = 0;
		if (result != GL_TRUE)
		{
			throw GraphicsException(std::string("Error while linking GLSL program: ") + buffer);
		}

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			throw GraphicsException("Error while loading GLSL effect");

		loadUniformInfo(program);
		loadAttributeInfo(program);

		m_programs.push_back(program);
	}

	EffectParameter* GlslEffect::AddParameter(EffectParameterType type, int numElements, void* handle, const char* name)
	{
		EffectParameter* param = new EffectParameter(this, type, numElements, nullptr, name);

		m_parameters.insert(ParamMap::value_type(param->Name.c_str(), param));

		m_parameterList.push_back(param);

		return param;
	}

	/*void GlslEffect::SetParameter(EffectParameter* param, Texture2D* texture)
	{
		GlTexture2D* glTexture = static_cast<GlTexture2D*>(texture);

		// look for the index of the texture
		int texindex = 0;
		for (int i = 0; i < m_textureParams.size(); i++)
		{
			if (m_textureParams[i] == param)
			{
				texindex = i;
				break;
			}
		}

		glActiveTexture(GL_TEXTURE0 + texindex);
		glBindTexture(GL_TEXTURE_2D, glTexture->GetGlTexture());

		int index = (int)reinterpret_cast<intptr_t>(param->GetHandle());
		ParamCache& cache = m_cache[index];
		cache.CachedValueType = CacheType_Int;
		cache.CachedValueSize = 1;
		cache.CachedValue[0] = texindex;
	}

	void GlslEffect::SetParameter(EffectParameter* param, const Vector4& value)
	{
		int index = (int)reinterpret_cast<intptr_t>(param->GetHandle());

		ParamCache& cache = m_cache[index];
		cache.CachedValueType = CacheType_Float;
		cache.CachedValueSize = 4;
		memcpy(&cache.CachedValue, &value, sizeof(float) * 4);
	}

	void GlslEffect::SetParameter(EffectParameter* param, float matrix4x4[])
	{
		int index = (int)reinterpret_cast<intptr_t>(param->GetHandle());

		ParamCache& cache = m_cache[index];
		cache.CachedValueType = CacheType_Float;
		cache.CachedValueSize = 16;
		memcpy(&cache.CachedValue, matrix4x4, sizeof(float) * 16);
	}

	void GlslEffect::SetParameter(EffectParameter* param, const Matrix& matrix)
	{
		int index = (int)reinterpret_cast<intptr_t>(param->GetHandle());

		ParamCache& cache = m_cache[index];
		cache.CachedValueType = CacheType_Float;
		cache.CachedValueSize = 16;
		memcpy(&cache.CachedValue, matrix.C, sizeof(float) * 16);
	}*/

	void GlslEffect::Apply()
	{
		// which program do we use?
		assert(m_programs.empty() == false);
		assert(m_programs.size() == 1);

		ApplyProgram(0);
	}

	void GlslEffect::ApplyProgram(int programIndex)
	{
		int program = m_programs[programIndex].Program;

		// bind the effect
		glUseProgram(m_programs[programIndex].Program);
		static_cast<OpenGlDevice*>(GetGraphicsDevice())->SetCurrentEffect(this);
		m_boundProgramIndex = programIndex;

		// go through the cached values and send them to OpenGL
		for (std::vector<GlslUniform>::iterator itr = m_programs[programIndex].Uniforms.begin();
			itr != m_programs[programIndex].Uniforms.end(); itr++)
		{
			EffectParameterType type = (*itr).Param->GetType();
			int numElements = (*itr).Param->GetNumElements();

			if (type == EffectParameterType_Int32)
			{
				glUniform1iv((*itr).Uniform, numElements, GetRawValue((*itr).Param));
			}
			else if (type == EffectParameterType_Single)
			{
				if (numElements == 1)
					glUniform1fv((*itr).Uniform, 1, (float*)GetRawValue((*itr).Param));
				else if (numElements == 2)
					glUniform2fv((*itr).Uniform, 1, (float*)GetRawValue((*itr).Param));
				else if (numElements == 3)
					glUniform3fv((*itr).Uniform, 1, (float*)GetRawValue((*itr).Param));
				else if (numElements == 4)
					glUniform4fv((*itr).Uniform, 1, (float*)GetRawValue((*itr).Param));
				else if (numElements == 16)
					glUniformMatrix4fv((*itr).Uniform, 1, GL_FALSE, (float*)GetRawValue((*itr).Param));
			}
			else if (type == EffectParameterType_Texture2D)
			{
				if ((*itr).Param->GetValueTexture2D() != nullptr)
				{
					// look for the index of the texture
					int texindex = 0;
					for (int i = 0; i < m_textureParams.size(); i++)
					{
						if (m_textureParams[i] == (*itr).Param)
						{
							texindex = i;
							break;
						}
					}

					glActiveTexture(GL_TEXTURE0 + texindex);
					glBindTexture(GL_TEXTURE_2D, static_cast<GlTexture2D*>((*itr).Param->GetValueTexture2D())->GetGlTexture());
					glUniform1iv((*itr).Uniform, 1, &texindex);
				}
			}

			GlException::ThrowIfError(__FILE__, __LINE__);
		}
	}

	const GlslAttribute* GlslEffect::GetAttribute(VertexElementUsage usage, int index)
	{
		for (std::vector<GlslAttribute>::iterator itr = m_programs[m_boundProgramIndex].Attributes.begin();
			itr != m_programs[m_boundProgramIndex].Attributes.end(); itr++)
		{
			if ((*itr).Index == index &&
				(*itr).Usage == usage)
				return &(*itr);
		}

		return nullptr;
	}

	int GlslEffect::compile(const std::string& source, const char* defines, bool vertex)
	{
		int shader;
		if (vertex)
		{
			shader = glCreateShader(GL_VERTEX_SHADER);
		}
		else
		{
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		}
        if (shader == 0)
            throw GraphicsException("Unable to create shader");

		if (defines != nullptr)
		{
			const char* sources[] = {defines, source.c_str()};
			glShaderSource(shader, 2, sources, nullptr);
		}
		else
		{
			const char* s = source.c_str();
			glShaderSource(shader, 1, &s, nullptr);
		}

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

	void GlslEffect::processSource(std::string& source, bool vertex)
	{
		// extract all the attrib info
		if (vertex)
			source = extractAttribInfo(source.c_str());

		// replace the {VERSION} macro
		replaceVersionMacros(source);

		// replace the {highp} stuff
		replacePrecisionMacros(source);

		// convert "in" and "out" params, if necessary
		if (static_cast<OpenGlDevice*>(GetGraphicsDevice())->GetGlslVersion() < 130)
			fixParameters(source, vertex);
	}

	void GlslEffect::loadUniformInfo(GlslProgram& program)
	{
		int numUniforms;
		glGetProgramiv(program.Program, GL_ACTIVE_UNIFORMS, &numUniforms);

		for (unsigned int i = 0; i < numUniforms; i++)
		{
			GLenum type;
			int size;
			char nameBuffer[256];
			glGetActiveUniform(program.Program, i, 255, nullptr, &size, &type, nameBuffer);
			nameBuffer[255] = 0;

			int location = glGetUniformLocation(program.Program, nameBuffer);

			EffectParameter* param = GetParameter(nameBuffer);

			if (param == nullptr)
			{
				EffectParameterType pType;
				if (type == GL_BOOL || type == GL_BOOL_VEC2 || type == GL_BOOL_VEC3 || type == GL_BOOL_VEC4)
					pType = EffectParameterType_Bool;
				else if (type == GL_INT || type == GL_INT_VEC2 || type == GL_INT_VEC3 || type == GL_INT_VEC4)
					pType = EffectParameterType_Int32;
				else if (type == GL_FLOAT || type == GL_FLOAT_VEC2 || type == GL_FLOAT_VEC3 || type == GL_FLOAT_VEC4 || type == GL_FLOAT_MAT4)
					pType = EffectParameterType_Single;
#ifndef USING_OPENGLES
				else if (type == GL_SAMPLER_1D)
					pType = EffectParameterType_Texture1D;
#endif
				else if (type == GL_SAMPLER_2D)
					pType = EffectParameterType_Texture2D;
#ifndef USING_OPENGLES
				else if (type == GL_SAMPLER_3D)
					pType = EffectParameterType_Texture3D;
#endif
				else if (type == GL_SAMPLER_CUBE)
					pType = EffectParameterType_TextureCube;
				else
					pType = EffectParameterType_Void;
				// what about EffectParameterType_String? does GLSL even support that?

				int numElements = 1;
				if (type == GL_BOOL ||
					type == GL_INT ||
					type == GL_FLOAT ||
					type == GL_SAMPLER_1D || type == GL_SAMPLER_2D || type == GL_SAMPLER_3D || type == GL_SAMPLER_CUBE)
					numElements = 1;
				else if (type == GL_BOOL_VEC2 ||
					type == GL_INT_VEC2 ||
					type == GL_FLOAT_VEC2)
					numElements = 2;
				else if (type == GL_BOOL_VEC3 ||
					type == GL_INT_VEC3 ||
					type == GL_FLOAT_VEC3)
					numElements = 3;
				else if (type == GL_BOOL_VEC4 ||
					type == GL_INT_VEC4 ||
					type == GL_FLOAT_VEC4)
					numElements = 4;
				else if (type == GL_FLOAT_MAT4)
					numElements = 16;

				// add the parameter, with the handle being the index of this parameter
				param = this->AddParameter(pType, numElements, nullptr, nameBuffer);

				// if this is a texture param then add it to the list of
				// texture parameters
				if (pType == EffectParameterType_Texture ||
					pType == EffectParameterType_Texture1D ||
					pType == EffectParameterType_Texture2D ||
					pType == EffectParameterType_Texture3D ||
					pType == EffectParameterType_TextureCube)
				{
					m_textureParams.push_back(param);
				}
			}

			GlslUniform uniform;
			uniform.Param = param;
			uniform.Program = program.Program;
			uniform.Uniform = location;

			program.Uniforms.push_back(uniform);
		}
	}

	void GlslEffect::loadAttributeInfo(GlslProgram& program)
	{
		int numAttributes;
		glGetProgramiv(program.Program, GL_ACTIVE_ATTRIBUTES, &numAttributes);

		for (int i = 0; i < numAttributes; i++)
		{
			int length, size;
			GLenum type;
			glGetActiveAttrib(program.Program, i, MAX_ATTRIB_SIZE, &length, &size, &type, m_attribNameBuffer);

			for (int j = 0; j < m_attributes.size(); j++)
			{
				if (strcmp(m_attributes[j].Name.c_str(), m_attribNameBuffer) == 0)
				{
					GlslAttribute attr = m_attributes[j];
					attr.GlHandle = glGetAttribLocation(program.Program, m_attribNameBuffer);

					program.Attributes.push_back(attr);

					break;
				}
			}
		}
	}

	std::string GlslEffect::extractAttribInfo(const char* vertexShaderSource)
	{
		std::string result;

		char nameBuffer[256];
		char usageBuffer[256];
		char numbers[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

		const char* cursor = vertexShaderSource;
		while(true)
		{
			const char* colon = strchr(cursor, ':');
			if (colon == nullptr)
			{
				result += cursor;
				break;
			}

			const char* nameEnd = lastIndexNotSpace(cursor, colon - cursor - 1);
			const char* nameStart = lastIndexOf(cursor, ' ', nameEnd - cursor - 1) + 1;
			strncpy(nameBuffer, nameStart, nameEnd - nameStart + 1);
			nameBuffer[nameEnd - nameStart + 1] = 0;

			GlslAttribute attrib;
			attrib.Name = nameBuffer;
			attrib.Usage = VEU_Position;
			attrib.Index = 0;
			attrib.GlHandle = 0;

			const char* semicolon = strchr(colon, ';');
			const char* usageStart = firstIndexNotSpace(colon + 1);
			strncpy(usageBuffer, usageStart, semicolon - usageStart);
			usageBuffer[semicolon - usageStart] = 0;

			const char* firstNumber = strchrAny(usageBuffer, numbers, 10);
			if (firstNumber != nullptr)
			{
				attrib.Index = firstNumber[0] - '0';
			}

			if (strncmp(usageBuffer, "POSITION", 8) == 0)
				attrib.Usage = VEU_Position;
			else if (strncmp(usageBuffer, "TEXCOORD", 8) == 0)
				attrib.Usage = VEU_TextureCoordinate;
			else if (strncmp(usageBuffer, "COLOR", 5) == 0)
				attrib.Usage = VEU_Color;
			else if (strncmp(usageBuffer, "NORMAL", 6) == 0)
				attrib.Usage = VEU_Normal;

			result.append(cursor, colon - cursor);

			cursor = semicolon;

			m_attributes.push_back(attrib);
		}

		return result;
	}

	void GlslEffect::replaceVersionMacros(std::string& code)
	{
		int version = static_cast<OpenGlDevice*>(GetGraphicsDevice())->GetGlslVersion();
		int major = version / 100;
		int minor = (version - major * 100) / 10;

		std::string versionString = "000";
		versionString[0] = (char)(major + '0');
		versionString[1] = (char)(minor + '0');

		std::string versionStringNewline = versionString + "\n";

		// there are 2 different versions of the {VERSION} macro,
		// 1 that adds a newline immediately after (because some GLSL compilers don't like trailing
		// spaces after the preprocessor directive) and 1 that doesn't.
		// {VERSION} adds a newline, {VERSION } doesn't.
		// Unfortunately we can't rely on __VERSION__ because ATI drivers have a bug
		// that makes __VERSION__ always = 100.
		replaceAll(code, "{VERSION}", versionStringNewline);
		replaceAll(code, "{VERSION }", versionString);
	}

	void GlslEffect::replacePrecisionMacros(std::string& code)
	{
		int version = static_cast<OpenGlDevice*>(GetGraphicsDevice())->GetGlslVersion();

		if (version > 100)
			replaceAll(code, "{highp}", "");
		else
			replaceAll(code, "{highp}", "highp");
	}

	void GlslEffect::fixParameters(std::string& code, bool isVertexShader)
	{
		if (isVertexShader)
		{
			// replace "in" with "attribute"
			replaceAll(code, "in", "attribute");

			// replace "out" with "varying"
			replaceAll(code, "out", "varying");
		}
		else
		{
			// replace "in" with "varying"
			replaceAll(code, "in", "varying");
		}
	}

	void GlslEffect::replaceAll(std::string& original, const std::string& toRemove, const std::string& toPut)
	{
		std::string::size_type offset = 0;
		while(offset < original.length())
		{
			std::string::size_type pos = original.find(toRemove, offset);
			if (pos == std::string::npos) break;
			if (pos == 0 || (pos > 0 && (original[pos - 1] == ' ' || original[pos - 1] == '\t' || original[pos - 1] == '\n' 
				|| original[pos - 1] == '(')))
			{
				original.erase(pos, toRemove.size());
				original.insert(pos, toPut);
			}
			
			offset += toPut.size();
		}
	}
}
}
}
