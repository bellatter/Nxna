#include "SpriteBatch.h"
#include "GraphicsDevice.h"
#include "PipelineState.h"

#ifdef NXNA_ENABLE_MATH
#include "../Matrix.h"
#endif

#ifdef NXNA_NO_CRT
extern "C" { void *  __cdecl memset(void * _Dst, _In_ int _Val, _In_ size_t _Size); }
#pragma intrinsic(memset)
#else
#include <cstring>
#endif


namespace Nxna
{
namespace Graphics
{
#ifdef NXNA_ENABLE_DIRECT3D11
	unsigned char SpriteEffect_VS[] = {
		0x44, 0x58, 0x42, 0x43, 0x35, 0x0f, 0x6c, 0x44, 0x0a, 0x1e, 0x4f, 0xe7, 0x99, 0x09, 0x3d, 0x0a,
		0xc6, 0x9a, 0x08, 0x7d, 0x01, 0x00, 0x00, 0x00, 0x3c, 0x03, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x30, 0x00, 0x00, 0x00, 0x1c, 0x01, 0x00, 0x00, 0x54, 0x02, 0x00, 0x00, 0xc8, 0x02, 0x00, 0x00,
		0x41, 0x6f, 0x6e, 0x39, 0xe4, 0x00, 0x00, 0x00, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x02, 0xfe, 0xff,
		0xb0, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24, 0x00, 0x00, 0x00, 0x30, 0x00,
		0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x24, 0x00, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xfe, 0xff,
		0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02,
		0x05, 0x00, 0x01, 0x80, 0x01, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x02, 0x80,
		0x02, 0x00, 0x0f, 0x90, 0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, 0x02, 0x00, 0x55, 0x90,
		0x02, 0x00, 0xe4, 0xa0, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80, 0x02, 0x00, 0x00, 0x90,
		0x01, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80,
		0x02, 0x00, 0xaa, 0x90, 0x03, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xe4, 0x80, 0x04, 0x00, 0x00, 0x04,
		0x00, 0x00, 0x0f, 0x80, 0x02, 0x00, 0xff, 0x90, 0x04, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xe4, 0x80,
		0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0xe4, 0xa0,
		0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0c, 0xc0, 0x00, 0x00, 0xe4, 0x80,
		0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0xe4, 0x90, 0x01, 0x00, 0x00, 0x02,
		0x01, 0x00, 0x03, 0xe0, 0x01, 0x00, 0xe4, 0x90, 0xff, 0xff, 0x00, 0x00, 0x53, 0x48, 0x44, 0x52,
		0x30, 0x01, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x4c, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x04,
		0x46, 0x8e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x03,
		0xf2, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00,
		0x65, 0x00, 0x00, 0x03, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03,
		0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x04, 0xf2, 0x20, 0x10, 0x00,
		0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00,
		0x36, 0x00, 0x00, 0x05, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, 0x32, 0x20, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x46, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x08, 0xf2, 0x00, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x56, 0x15, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x46, 0x8e, 0x20, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x0a, 0xf2, 0x00, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x06, 0x10, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x46, 0x8e, 0x20, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x32, 0x00, 0x00, 0x0a, 0xf2, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0x1a, 0x10, 0x00,
		0x02, 0x00, 0x00, 0x00, 0x46, 0x8e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
		0x46, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x0a, 0xf2, 0x20, 0x10, 0x00,
		0x02, 0x00, 0x00, 0x00, 0xf6, 0x1f, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x46, 0x8e, 0x20, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x3e, 0x00, 0x00, 0x01, 0x49, 0x53, 0x47, 0x4e, 0x6c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x56, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x03, 0x03, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x43, 0x4f, 0x4c, 0x4f,
		0x52, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x6f,
		0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x6c, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
		0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x03, 0x0c, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
		0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0x53,
		0x56, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0xab,
	};

	unsigned char SpriteEffect_PS[] = {
		0x44, 0x58, 0x42, 0x43, 0x3e, 0xe7, 0x9c, 0x58, 0x91, 0x9f, 0xac, 0x33, 0xa4, 0xcf, 0xcf, 0xfe,
		0x7c, 0xe6, 0x9b, 0x6a, 0x01, 0x00, 0x00, 0x00, 0xe8, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x30, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x00, 0x00, 0x64, 0x01, 0x00, 0x00, 0xb4, 0x01, 0x00, 0x00,
		0x41, 0x6f, 0x6e, 0x39, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xff,
		0x58, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x28, 0x00,
		0x00, 0x00, 0x28, 0x00, 0x01, 0x00, 0x24, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x02, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0xb0,
		0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x03, 0xb0, 0x1f, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x90, 0x00, 0x08, 0x0f, 0xa0, 0x42, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80,
		0x01, 0x00, 0xe4, 0xb0, 0x00, 0x08, 0xe4, 0xa0, 0x05, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80,
		0x00, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0xb0, 0x01, 0x00, 0x00, 0x02, 0x00, 0x08, 0x0f, 0x80,
		0x00, 0x00, 0xe4, 0x80, 0xff, 0xff, 0x00, 0x00, 0x53, 0x48, 0x44, 0x52, 0xa4, 0x00, 0x00, 0x00,
		0x40, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x04, 0x46, 0x8e, 0x20, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03,
		0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00,
		0x45, 0x00, 0x00, 0x09, 0xf2, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x46, 0x7e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x07, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x46, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x3e, 0x00, 0x00, 0x01, 0x49, 0x53, 0x47, 0x4e, 0x48, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x03, 0x03, 0x00, 0x00, 0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f,
		0x52, 0x44, 0x00, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x54,
		0x61, 0x72, 0x67, 0x65, 0x74, 0x00, 0xab, 0xab,
	};
#endif

	void SpriteBatch::WriteSprite(SpriteBatchSprite* sprite, Texture2D* texture, unsigned int textureWidth, unsigned int textureHeight, float x, float y, float width, float height, Color color)
	{
		sprite->Texture = *texture;
		sprite->TextureWidth = textureWidth;
		sprite->TextureHeight = textureHeight;
		sprite->Source[0] = 0;
		sprite->Source[1] = 0;
		sprite->Source[2] = (float)textureWidth;
		sprite->Source[3] = (float)textureHeight;
		sprite->Destination[0] = x;
		sprite->Destination[1] = y;
		sprite->Destination[2] = width;
		sprite->Destination[3] = height;
		sprite->Origin[0] = 0;
		sprite->Origin[1] = 0;
		sprite->Depth = 0;
		sprite->Rotation = 0;
		sprite->SpriteColor = NXNA_GET_PACKED_COLOR(color);
		sprite->Effects = SpriteEffects::None;
	}

	void SpriteBatch::SortSprites(SpriteBatchSprite* sprites, unsigned int* indices, unsigned int numSprites, SpriteSortMode mode)
	{
		if (indices == nullptr)
			return; // nowhere to put the results, so don't do anything

		// tODO: finish this and the other methods

		for (unsigned int i = 0; i < numSprites; i++)
		{
			indices[i] = i;
		}
	}

	void SpriteBatch::FillIndexBuffer(unsigned short* indices, unsigned int numIndices)
	{
		for (unsigned short i = 0; i < numIndices / 6; i++)
		{
			indices[i * 6 + 0] = i * 4;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4;
			indices[i * 6 + 4] = i * 4 + 2;
			indices[i * 6 + 5] = i * 4 + 3;
		}
	}

	unsigned int SpriteBatch::FillVertexBuffer(SpriteBatchSprite* sprites, unsigned int* indices, unsigned int numSprites, void* buffer, size_t bufferByteLength, unsigned int* textureRunLengths, unsigned int* numTextureRunLengths)
	{
		if (numSprites == 0) return 0;

		// textureRunLengths and numTextureRunLengths must both be provided or neither provided
		if ((textureRunLengths == nullptr && numTextureRunLengths != nullptr) ||
			(textureRunLengths != nullptr && numTextureRunLengths == nullptr))
			return 0;

		// TODO: for now, ignore the sorting
		unsigned int texRunCount = 0;
		unsigned int currentTextureRunLength = 0;
		auto currentTextureID = sprites[0].Texture.UniqueID;

		float* verts = (float*)buffer;

		const size_t stride = 3 + 2 + 1;
		size_t spriteCapacity = bufferByteLength / (stride * 4 * sizeof(float));
		if (numSprites > spriteCapacity)
			numSprites = (int)spriteCapacity;
		
		for (unsigned int i = 0; i < numSprites; i++)
		{
			// look for texture changes
			if (sprites[i].Texture.UniqueID != currentTextureID)
			{
				if (textureRunLengths != nullptr)
				{
					textureRunLengths[texRunCount] = currentTextureRunLength;
					texRunCount++;

					if (texRunCount >= *numTextureRunLengths)
					{
						return i;
					}
				}

				currentTextureRunLength = 1;
				currentTextureID = sprites[i].Texture.UniqueID;
			}
			else
			{
				currentTextureRunLength++;
			}

			float cosine, sine;
			if (sprites[i].Rotation != 0)
			{
				cosine = cosf(sprites[i].Rotation);
				sine = sinf(sprites[i].Rotation);
			}
			else
			{
				cosine = 1.0f;
				sine = 0;
			}

			float adjustedOriginX = sprites[i].Origin[0] / sprites[i].Source[2];
			float adjustedOriginY = sprites[i].Origin[1] / sprites[i].Source[3];

			float inverseTextureWidth = 1.0f / (float)sprites[i].TextureWidth;
			float inverseTextureHeight = 1.0f / (float)sprites[i].TextureHeight;

			float x = sprites[i].Destination[0];
			float y = sprites[i].Destination[1];
			float width = sprites[i].Destination[2];
			float height = sprites[i].Destination[3];

			float o1 = (0 - adjustedOriginX) * width;
			float o2 = (0 - adjustedOriginY) * height;
			float o3 = (1 - adjustedOriginX) * width;
			float o4 = (0 - adjustedOriginY) * height;
			float o5 = (1 - adjustedOriginX) * width;
			float o6 = (1 - adjustedOriginY) * height;
			float o7 = (0 - adjustedOriginX) * width;
			float o8 = (1 - adjustedOriginY) * height;

			float texTLX = sprites[i].Source[0] * inverseTextureWidth;
			float texTLY = sprites[i].Source[1]* inverseTextureHeight;
			float texBRX = (sprites[i].Source[0] + sprites[i].Source[2]) * inverseTextureWidth;
			float texBRY = (sprites[i].Source[1] + sprites[i].Source[3]) * inverseTextureHeight;

			if ((sprites[i].Effects & (int)SpriteEffects::FlipHorizontally) != 0)
			{
				float tmp = texTLX;
				texTLX = texBRX;
				texBRX = tmp;
			}

			if ((sprites[i].Effects & (int)SpriteEffects::FlipVertically) != 0)
			{
				float tmp = texTLY;
				texTLY = texBRY;
				texBRY = tmp;
			}

			PackedColor packedColor = sprites[i].SpriteColor;
			float depth = sprites[i].Depth;

			verts[0 * stride + 0] = x + o1 * cosine - o2 * sine;
			verts[0 * stride + 1] = y + o1 * sine + o2 * cosine;
			verts[0 * stride + 2] = depth;
			verts[0 * stride + 3] = texTLX;
			verts[0 * stride + 4] = texTLY;
			verts[0 * stride + 5] = *(float*)&packedColor;

			verts[1 * stride + 0] = x + o3 * cosine - o4 * sine;
			verts[1 * stride + 1] = y + o3 * sine + o4 * cosine;
			verts[1 * stride + 2] = depth;
			verts[1 * stride + 3] = texBRX;
			verts[1 * stride + 4] = texTLY;
			verts[1 * stride + 5] = *(float*)&packedColor;

			verts[2 * stride + 0] = x + o5 * cosine - o6 * sine;
			verts[2 * stride + 1] = y + o5 * sine + o6 * cosine;
			verts[2 * stride + 2] = depth;
			verts[2 * stride + 3] = texBRX;
			verts[2 * stride + 4] = texBRY;
			verts[2 * stride + 5] = *(float*)&packedColor;

			verts[3 * stride + 0] = x + o7 * cosine - o8 * sine;
			verts[3 * stride + 1] = y + o7 * sine + o8 * cosine;
			verts[3 * stride + 2] = depth;
			verts[3 * stride + 3] = texTLX;
			verts[3 * stride + 4] = texBRY;
			verts[3 * stride + 5] = *(float*)&packedColor;

			verts += stride * 4;
		}

		if (textureRunLengths)
		{
			textureRunLengths[texRunCount] = currentTextureRunLength;
			*numTextureRunLengths = texRunCount + 1;
		}

		// return the # of sprites written to the buffer
		return numSprites;
	}

	void SpriteBatch::SetupVertexElements(InputElement* elements, unsigned int* stride)
	{
		elements[0].Offset = 0;
		elements[0].ElementFormat = InputElementFormat::Vector3;
		elements[0].ElementUsage = InputElementUsage::Position;
		elements[0].UsageIndex = 0;
		elements[0].InputSlot = 0;

		elements[1].Offset = sizeof(float) * 3;
		elements[1].ElementFormat = InputElementFormat::Vector2;
		elements[1].ElementUsage = InputElementUsage::TextureCoordinate;
		elements[1].UsageIndex = 0;
		elements[1].InputSlot = 0;

		elements[2].Offset = sizeof(float) * 5;
		elements[2].ElementFormat = InputElementFormat::Color;
		elements[2].ElementUsage = InputElementUsage::Color;
		elements[2].UsageIndex = 0;
		elements[2].InputSlot = 0;

		if (stride != nullptr)
			*stride = sizeof(float) * 5 + sizeof(int);
	}

	void SpriteBatch::GetShaderBytecode(GraphicsDevice* device, ShaderBytecode* vertexShader, ShaderBytecode* pixelShader)
	{
		switch (device->GetType())
		{
#ifdef NXNA_ENABLE_DIRECT3D11
		case GraphicsDeviceType::Direct3D11:
		{
			vertexShader->Bytecode = SpriteEffect_VS;
			vertexShader->BytecodeLength = sizeof(SpriteEffect_VS);

			pixelShader->Bytecode = SpriteEffect_PS;
			pixelShader->BytecodeLength = sizeof(SpriteEffect_PS);
		}
			break;
#endif
		case GraphicsDeviceType::OpenGl41:
		{
			const char* vertex =
				"uniform dataz { mat4 ModelViewProjection; };\n"
				"layout(location = 0) in vec3 position;\n"
				"layout(location = 1) in vec2 texCoords;\n"
				"layout(location = 2) in vec4 color;\n"
				"out VertexOutput\n"
				"{\n"
				"	vec2 o_diffuseCoords;\n"
				"	vec4 o_color;\n"
				"};\n"
				"out gl_PerVertex { vec4 gl_Position; };\n"
				"void main()\n"
				"{\n"
				"	gl_Position = ModelViewProjection * vec4(position, 1.0);\n"
				"	o_diffuseCoords = texCoords;\n"
				"	o_color = color;\n"
				"}\n";
			const char* pixel =
				"DECLARE_SAMPLER2D(0, Diffuse);"
				"in VertexOutput\n"
				"{\n"
				"	vec2 o_diffuseCoords;\n"
				"	vec4 o_color;\n"
				"};\n"
				"out vec4 outputColor;\n"
				"\n"
				"void main()\n"
				"{\n"
				" float v = o_diffuseCoords.x;"
				"	outputColor = texture(Diffuse, o_diffuseCoords) * o_color;\n"
				"}\n";
			
			vertexShader->Bytecode = vertex;
			vertexShader->BytecodeLength = 0;

			pixelShader->Bytecode = pixel;
			pixelShader->BytecodeLength = 0;
		}
			break;
		}
	}

	void SpriteBatch::SetupConstantBuffer(Viewport viewport, void* constantBuffer, size_t maxLength)
	{
		float n1 = viewport.Width > 0 ? (1.0f / (float)viewport.Width) : 0;
		float n2 = viewport.Height > 0 ? (-1.0f / (float)viewport.Height) : 0;

#ifdef NXNA_ENABLE_MATH
		Matrix transform;
		Matrix::GetIdentity(transform);
		transform.M11 = n1 * 2.0f;
		transform.M22 = n2 * 2.0f;
		transform.M41 = -1.0f;// - n1; // these are pixel offsets (I think)
		transform.M42 = 1.0f;// - n2;


		memcpy(constantBuffer, &transform, sizeof(Matrix));
#else
		if (maxLength < sizeof(float) * 16) return;

		float* cbufferData = (float*)constantBuffer;
		cbufferData[0] = n1 * 2.0f;
		cbufferData[1] = 0;
		cbufferData[2] = 0;
		cbufferData[3] = 0;

		cbufferData[4] = 0;
		cbufferData[5] = n2 * 2.0f;
		cbufferData[6] = 0;
		cbufferData[7] = 0;

		cbufferData[8] = 0;
		cbufferData[9] = 0;
		cbufferData[10] = 1.0f;
		cbufferData[11] = 0;

		cbufferData[12] = -1.0f;
		cbufferData[13] = 1.0f;
		cbufferData[14] = 0;
		cbufferData[15] = 1.0f;
#endif

	}
}
}
