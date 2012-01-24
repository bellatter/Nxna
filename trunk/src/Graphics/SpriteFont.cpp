#include <cstring>
#include <cassert>
#include "SpriteFont.h"
#include "Texture2D.h"
#include "../Content/FileStream.h"
#include "../Content/XnbReader.h"
#include "../Rectangle.h"
#include "../Vector3.h"

namespace Nxna
{
namespace Graphics
{
	void* SpriteFontLoader::Load(Content::XnbReader* stream)
	{
		return SpriteFont::LoadFrom(stream);
	}

	void SpriteFontLoader::Destroy(void* resource)
	{
		delete static_cast<SpriteFont*>(resource);
	}

	SpriteFont::~SpriteFont()
	{
		delete m_texture;
		delete[] m_glyphs;
		delete[] m_cropping;
		delete[] m_kerning;
	}

	Nxna::Vector2 SpriteFont::MeasureString(const char* text)
	{
		Nxna::Vector2 size(0, 0);

		int len = strlen(text);
		for (int i = 0; i < len; i++)
		{
			Rectangle glyph, cropping;
			Vector3 kerning;

			GetCharacterInfo(text[i], &glyph, &cropping, &kerning);

			size.X += kerning.X;

			size.X += kerning.Y + kerning.Z;

			if (size.Y < glyph.Height)
			{
				size.Y = (float)glyph.Height;
			}
		}

		return size;
	}

	Nxna::Vector2 SpriteFont::MeasureString(const wchar_t* text)
	{
		Nxna::Vector2 size(0, 0);

		int len = wcsnlen(text, 10000);

		for (int i = 0; i < len; i++)
		{
			Rectangle glyph, cropping;
			Vector3 kerning;

			GetCharacterInfo(text[i], &glyph, &cropping, &kerning);

			size.X += kerning.X;
			size.X += kerning.Y + kerning.Z;

			if (size.Y < cropping.Height)
			{
				size.Y = (float)cropping.Height;
			}
		}

		return size;
	}

	SpriteFont* SpriteFont::LoadFrom(Content::XnbReader* stream)
	{
		int typeID = stream->ReadTypeID();

		SpriteFont* result = new SpriteFont();

		result->m_texture = Texture2D::LoadFrom(stream);

		typeID = stream->ReadTypeID();
		result->m_numGlyphs = stream->GetStream()->ReadInt32();
		result->m_glyphs = new Rectangle[result->m_numGlyphs];

		for (int i = 0; i < result->m_numGlyphs; i++)
		{
			result->m_glyphs[i].X = stream->GetStream()->ReadInt32();
			result->m_glyphs[i].Y = stream->GetStream()->ReadInt32();
			result->m_glyphs[i].Width = stream->GetStream()->ReadInt32();
			result->m_glyphs[i].Height = stream->GetStream()->ReadInt32();
		}

		typeID = stream->ReadTypeID();
		result->m_numCropping = stream->GetStream()->ReadInt32();
		result->m_cropping = new Rectangle[result->m_numCropping];

		for (int i = 0; i < result->m_numGlyphs; i++)
		{
			result->m_cropping[i].X = stream->GetStream()->ReadInt32();
			result->m_cropping[i].Y = stream->GetStream()->ReadInt32();
			result->m_cropping[i].Width = stream->GetStream()->ReadInt32();
			result->m_cropping[i].Height = stream->GetStream()->ReadInt32();
		}

		typeID = stream->ReadTypeID();
		int numCharacters = stream->GetStream()->ReadInt32();

		for (int i = 0; i < numCharacters; i++)
		{
			unsigned short c = stream->GetStream()->ReadByte();
			if ((c & 0xE0) == 0xC0)
			{
				// this is a 2-byte unicode character, so read the next byte
				byte b2 = stream->GetStream()->ReadByte();

				c = ((c & 0x1F) << 6) + (b2 & 0x3F);
			}
			result->m_characters.insert(std::map<unsigned short,int>::value_type(c, i));
		}

		result->m_lineHeight = stream->GetStream()->ReadInt32();
		result->m_spacing = stream->GetStream()->ReadFloat();

		typeID = stream->ReadTypeID();
		result->m_numKerning = stream->GetStream()->ReadInt32();
		result->m_kerning = new float[result->m_numKerning * 3];
		for (int i = 0; i < result->m_numKerning; i++)
		{
			result->m_kerning[i * 3 + 0] = stream->GetStream()->ReadFloat();
			result->m_kerning[i * 3 + 1] = stream->GetStream()->ReadFloat();
			result->m_kerning[i * 3 + 2] = stream->GetStream()->ReadFloat();
		}

		return result;
	}

	bool SpriteFont::GetCharacterInfo(unsigned short c, Rectangle* glyph, Rectangle* cropping, Vector3* kerning)
	{
		// find the character
		std::map<unsigned short,int>::iterator itr = m_characters.find(c);
		if (itr != m_characters.end())
		{
			*glyph = m_glyphs[(*itr).second];
			*cropping = m_cropping[(*itr).second];
			*kerning = Vector3(m_kerning[(*itr).second * 3 + 0], m_kerning[(*itr).second * 3 + 1], m_kerning[(*itr).second * 3 + 2]);

			return true;
		}

		return false;
	}
}
}