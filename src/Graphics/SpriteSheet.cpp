#include <cassert>
#include "SpriteSheet.h"
#include "Texture2D.h"
#include "GraphicsDevice.h"
#include "../Content/XnbReader.h"
#include "../Content/FileStream.h"

namespace Nxna
{
namespace Graphics
{
	void* SpriteSheet::Read(Content::XnbReader* reader)
	{
		assert(reader != nullptr);
		Content::Stream* stream = reader->GetStream();

		SpriteSheet* newSheet = new SpriteSheet();

		int typeID = reader->ReadTypeID();

		// read the textures
		newSheet->m_texture = Texture2D::LoadFrom(stream);

		// read the rects
		int num = stream->ReadInt32();

		for (int i = 0; i < num; i++)
		{
			Rectangle r;
			r.X = stream->ReadInt32();
			r.Y = stream->ReadInt32();
			r.Width = stream->ReadInt32();
			r.Height = stream->ReadInt32();

			newSheet->m_spriteRectangles.push_back(r);
		}

		return newSheet;
	}

	void SpriteSheet::Destroy(void* resource)
	{
		SpriteSheet* ss = static_cast<SpriteSheet*>(resource);
		delete ss->m_texture;
		delete ss;
	}
}
}