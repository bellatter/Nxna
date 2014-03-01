#ifndef GRAPHICS_SPRITEFONT_H
#define GRAPHICS_SPRITEFONT_H

#include <unordered_map>
#include "../NxnaConfig.h"
#include "../Vector2.h"
#include "../Content/ContentManager.h"

NXNA_DISABLE_OVERRIDE_WARNING

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

		std::unordered_map<unsigned int, int> m_characters;

		int m_lineHeight;
		float m_spacing;

		SpriteFont() { }

	public:
		// In XNA the constructor is hidden, but here it isn't, for I am kind.
		// You are free to delete glyphs, cropping, charMap, and kerning after you call this.
		// The SpriteFont takes over ownership of texture, so make sure it isn't deleted or unloaded.
		SpriteFont(Texture2D* texture, int numCharacters, Rectangle* glyphs, Rectangle* cropping, unsigned short* charMap,
			int lineSpacing, float spacing, Vector3* kerning, unsigned short* defaultCharacter);
		~SpriteFont();

		Nxna::Vector2 MeasureString(const char* text);
		Nxna::Vector2 MeasureString(const wchar_t* text);
		Nxna::Vector2 MeasureStringUTF8(const char* text);

		float GetSpacing() { return m_spacing; }
		int GetLineSpacing() { return m_lineHeight; }

		static SpriteFont* LoadFrom(Content::XnbReader* stream);

	protected:

		bool GetCharacterInfo(unsigned int c, Rectangle* glyph, Rectangle* cropping, Vector3* kerning);
		
	private:
		static int convertUTF8Character(const char* string, unsigned short* result);

	};

	class SpriteFontLoader : public Content::IContentReader
	{
	public:
		virtual const char* GetTypeName() override { return typeid(SpriteFont).name(); }
		virtual void* Read(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_SPRITEFONT_H
