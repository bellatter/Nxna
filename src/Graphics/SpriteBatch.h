#ifndef NXNA_GRAPHICS_SPRITEBATCH_H
#define NXNA_GRAPHICS_SPRITEBATCH_H

#include <cstddef>
#include "../Color.h"
#include "../Vector2.h"
#include "../Vector4.h"
#include "GraphicsDevice.h"

namespace Nxna
{
namespace Graphics
{
	enum class SpriteSortMode
	{
		BackToFront,
		Deferred,
		FrontToBack,
		Immediate,
		Texture
	};

	enum SpriteEffects
	{
		None = 0,
		FlipHorizontally = 2,
		FlipVertically = 4
	};

	struct SpriteBatchSprite
	{
		Texture2D Texture;
		int TextureWidth;
		int TextureHeight;
		PackedColor SpriteColor;
		float Source[4];
		float Destination[4];
		float Origin[2];
		float Rotation;
		float Depth;
		SpriteEffects Effects;
	};

	class SpriteBatch
	{

	public:

		// writes a new sprite
		static void WriteSprite(SpriteBatchSprite* sprite, Texture2D* texture, unsigned int textureWidth, unsigned int textureHeight, float x, float y, float width, float height, Color color);
		static void WriteSprite(SpriteBatchSprite* sprite, Texture2D* texture, unsigned int textureWidth, unsigned int textureHeight, float x, float y, float width, float height, unsigned int packedColor);

		// sorts the sprites based on the mode
		static void SortSprites(SpriteBatchSprite* sprites, unsigned int* indices, unsigned int numSprites, SpriteSortMode mode);

		// fills the index buffer with a bunch of indices
		static void FillIndexBuffer(unsigned short* indices, unsigned int numIndices);

		// writes as many sprites as it can to the buffer, and returns how many sprites it wrote.
		[[deprecated]]
		static unsigned int FillVertexBuffer(SpriteBatchSprite* sprites, unsigned int* indices, unsigned int numSprites, void* buffer, size_t bufferByteLength, unsigned int* spriteTextureChangeIndices, unsigned int* numSpriteTextureChangeIndices); 

		static unsigned int FillVertexBuffer(SpriteBatchSprite* sprites, unsigned int* indices, unsigned int numSprites, void* buffer, size_t bufferByteLength);


		static void SetupVertexElements(InputElement* elements, unsigned int* stride);
		static void GetShaderBytecode(GraphicsDevice* device, ShaderBytecode* vertexShader, ShaderBytecode* pixelShader);
		static void SetupConstantBuffer(Viewport viewport, void* constantBuffer, size_t maxLength);
	};
}
}

#endif // NXNA_GRAPHICS_SPRITEBATCH_H
