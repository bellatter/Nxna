#ifndef GRAPHICS_SPRITESHEET_H
#define GRAPHICS_SPRITESHEET_H

#include <vector>
#include "../NxnaConfig.h"
#include "../Content/ContentManager.h"
#include "../Rectangle.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class Texture2D;

	class SpriteSheet : public Content::IContentReader
	{
		Texture2D* m_texture;
		std::vector<Rectangle> m_spriteRectangles;

	public:
		Texture2D* GetTexture() { return m_texture; }

		Rectangle GetSourceRectangle(int spriteIndex)
		{
			return m_spriteRectangles[spriteIndex];
		}

		int GetNumSprites() { return (int)m_spriteRectangles.size(); }

		virtual const char* GetTypeName() override { return typeid(SpriteSheet).name(); }
		virtual void* Read(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_SPRITESHEET_H