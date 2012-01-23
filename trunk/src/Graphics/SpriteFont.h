#ifndef GRAPHICS_SPRITEFONT_H
#define GRAPHICS_SPRITEFONT_H

#include <map>
#include "../NxnaConfig.h"
#include "../Vector2.h"
#include "../Content/ContentManager.h"

namespace Nxna
{
	struct Rectangle;
	struct Vector3;

namespace Content
{
	class FileStream;
	class XnbReader;
}

namespace Graphics
{
	class Texture2D;

	class SpriteFont
	{
		friend class SpriteBatch;

		Texture2D* m_texture;

		int m_numGlyphs;
		Rectangle* m_glyphs;

		int m_numCropping;
		Rectangle* m_cropping;

		int m_numKerning;
		float* m_kerning;

		std::map<unsigned short, int> m_characters;

		int m_lineHeight;
		float m_spacing;

	public:

		~SpriteFont();

		Nxna::Vector2 MeasureString(const char* text);
		Nxna::Vector2 MeasureString(const wchar_t* text);

		int GetLineSpacing() { return m_lineHeight; }

		static SpriteFont* LoadFrom(Content::XnbReader* stream);

	protected:

		bool GetCharacterInfo(unsigned short c, Rectangle* glyph, Rectangle* cropping, Vector3* kerning);
		
	private:
		static int convertUTF8Character(const char* string, unsigned short* result);

	};

	class SpriteFontLoader : public Content::IContentResourceLoader
	{
	public:
		virtual const char* GetTypeName() override { return typeid(SpriteFont).name(); }
		virtual void* Load(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}


#endif // GRAPHICS_SPRITEFONT_H
