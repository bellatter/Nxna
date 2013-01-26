#ifndef NXNA_GRAPHICS_OPENGL_GLSLSOURCE_H
#define NXNA_GRAPHICS_OPENGL_GLSLSOURCE_H

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlslSource
	{
		const static int MAX_SOURCE_OBJECTS = 20;
		const char* m_defines[MAX_SOURCE_OBJECTS];
		char m_versionDefinition[16];
		char m_versionConstant[24];
		const char* m_vertexSource;
		const char* m_fragmentSource;
		int m_glslVersion;

	public:
		GlslSource(const char* vertexSource, const char* fragmentSource, int glslVersion);
		unsigned int Build(const char* defines[], int numDefines);

	private:
		int compile(const char* source, const char* defines[], int numDefines, bool isVertexShader);
	};
}
}
}

#endif // NXNA_GRAPHICS_OPENGL_GLSLSOURCE_H
