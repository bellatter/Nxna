#ifndef GRAPHICS_SPRITESHEET_H
#define GRAPHICS_SPRITESHEET_H

#include <vector>
#include "../Content/ContentManager.h"
#include "../Rectangle.h"

namespace Nxna
{
namespace Graphics
{
	class Texture2D;

	class SpriteSheet : public Content::IContentResourceLoader
	{
		Texture2D* m_texture;
		std::vector<Rectangle> m_spriteRectangles;

	public:
		Texture2D* GetTexture() { return m_texture; }

		Rectangle GetSourceRectangle(int spriteIndex)
		{
			return m_spriteRectangles[spriteIndex];
		}

		int GetNumSprites() { return m_spriteRectangles.size(); }

		virtual const char* GetTypeName() override { return typeid(SpriteSheet).name(); }
		virtual void* Load(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

#endif // GRAPHICS_SPRITESHEET_H