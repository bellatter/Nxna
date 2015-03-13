#include <cassert>
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "SpriteEffect.h"
#include "Texture2D.h"
#include "Effect.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexDeclaration.h"
#include "../MathHelper.h"
#include "../Utils.h"
#include "../MemoryAllocator.h"

namespace Nxna
{
namespace Graphics
{
	SpriteEffect* SpriteBatch::m_effect = nullptr;
	DynamicVertexBuffer* SpriteBatch::m_vertexBuffer = nullptr;
	IndexBuffer* SpriteBatch::m_indexBuffer = nullptr;
	VertexDeclaration* SpriteBatch::m_declaration = nullptr;


	SpriteBatch::SpriteBatch(GraphicsDevice* device)
	{
		m_device = device;

		if (m_declaration == nullptr)
		{
			VertexElement elements[] = {
				{ 0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0 },
				{ sizeof(float) * 3, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0},
				{ sizeof(float) * 5, VertexElementFormat::Color, VertexElementUsage::Color, 0}
			};

			m_declaration = new VertexDeclaration(elements, 3);
		}

		if (m_indexBuffer == nullptr)
		{
			createIndexBuffer();
		}

		if (m_effect == nullptr)
		{
			m_effect = new SpriteEffect(device);
		}
	}

	void SpriteBatch::Begin()
	{
		Begin(SpriteSortMode::Texture, nullptr, nullptr, 
			nullptr, nullptr, Nxna::Matrix::Identity);
	}

	void SpriteBatch::Begin(SpriteSortMode sortMode, const BlendState* blendState)
	{
		Begin(sortMode, blendState, nullptr, 
			nullptr, nullptr, Nxna::Matrix::Identity);
	}

	void SpriteBatch::Begin(SpriteSortMode sortMode, const BlendState* blendState, 
		const DepthStencilState* depthStencilState, const RasterizerState* rasterizerState)
	{
		Begin(sortMode, blendState, depthStencilState, rasterizerState, nullptr, Matrix::Identity);
	}

	void SpriteBatch::Begin(SpriteSortMode sortMode, const BlendState* blendState, 
		const DepthStencilState* depthStencilState, const RasterizerState* rasterizerState, Effect* effect)
	{
		Begin(sortMode, blendState, depthStencilState, rasterizerState, effect, Matrix::Identity);
	}

	void SpriteBatch::Begin(SpriteSortMode sortMode, const BlendState* blendState, 
		const DepthStencilState* depthStencilState, const RasterizerState* rasterizerState, Effect* effect, const Matrix& transform)
	{
		m_sortMode = sortMode;
		if (blendState == nullptr)
			m_blend = BlendState::GetAlphaBlend();
		else
			m_blend = blendState;

		m_customTransform = transform;
		m_customEffect = effect;

		if (depthStencilState == nullptr)
			m_depthStencilState = DepthStencilState::GetNone();
		else
			m_depthStencilState = depthStencilState;

		if (rasterizerState == nullptr)
			m_rasterizerState = RasterizerState::GetCullCounterClockwise();
		else
			m_rasterizerState = rasterizerState;

		if (sortMode == SpriteSortMode::Immediate)
		{
			setRenderStates();
		}
	}

	void SpriteBatch::End()
	{
		if (m_sortMode != SpriteSortMode::Immediate)
		{
			setRenderStates();
		}

        flush();
	}

	void SpriteBatch::Draw(Texture2D* texture, const Vector2& position, const Color& color)
	{
		assert(texture != nullptr);

		Rectangle r;
		r.X = (int)position.X;
		r.Y = (int)position.Y;
		r.Width = texture->GetWidth();
		r.Height = texture->GetHeight();

		Draw(texture, r, nullptr, color);
	}

	void SpriteBatch::Draw(Texture2D* texture, const Vector2& position, const Rectangle* sourceRectangle, const Color& color)
	{
		assert(texture != nullptr);

		Rectangle r;
		r.X = (int)position.X;
		r.Y = (int)position.Y;
		r.Width = texture->GetWidth();
		r.Height = texture->GetHeight();

		Draw(texture, r, sourceRectangle, color);
	}
	
	void SpriteBatch::Draw(Texture2D* texture, const Rectangle& destinationRectangle, const Rectangle* sourceRectangle, const Color& color)
	{
		assert(texture != nullptr);

		Sprite s;
		s.Texture = texture;
		s.SpriteColor = color;
		s.Destination.X = (float)destinationRectangle.X;
		s.Destination.Y = (float)destinationRectangle.Y;
		s.Destination.Z = (float)destinationRectangle.Width;
		s.Destination.W = (float)destinationRectangle.Height;
		s.Origin = Vector2(0, 0);
		s.Rotation = 0;
		s.Depth = 0;
		s.Effects = SpriteEffects::None;
		
		if (sourceRectangle != nullptr)
		{
			s.Source.X = (float)sourceRectangle->X;
			s.Source.Y = (float)sourceRectangle->Y;
			s.Source.Z = (float)sourceRectangle->Width;
			s.Source.W = (float)sourceRectangle->Height;
		}
		else
		{
			s.Source.X = 0;
			s.Source.Y = 0;
			s.Source.Z = (float)texture->GetWidth();
			s.Source.W = (float)texture->GetHeight();
		}

		m_sprites.push_back(s);
	}

	void SpriteBatch::Draw(Texture2D* texture, const Rectangle& destinationRectangle, const Color& color)
	{
		assert(texture != nullptr);

		Sprite s;
		s.Texture = texture;
		s.SpriteColor = color;
		s.Destination.X = (float)destinationRectangle.X;
		s.Destination.Y = (float)destinationRectangle.Y;
		s.Destination.Z = (float)destinationRectangle.Width;
		s.Destination.W = (float)destinationRectangle.Height;
		s.Origin = Vector2(0, 0);
		s.Rotation = 0;
		s.Depth = 0;
		s.Effects = SpriteEffects::None;
		
		s.Source.X = 0;
		s.Source.Y = 0;
		s.Source.Z = (float)texture->GetWidth();
		s.Source.W = (float)texture->GetHeight();

		m_sprites.push_back(s);
	}

	void SpriteBatch::Draw(Texture2D* texture, const Vector2& position, const Rectangle* sourceRectangle, const Color& color,
		float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth)
	{
		Draw(texture, position, sourceRectangle, color, rotation, origin, Vector2(scale, scale), effects, layerDepth);
	}

	void SpriteBatch::Draw(Texture2D* texture, const Vector2& position, const Rectangle* sourceRectangle, const Color& color,
		float rotation, const Vector2& origin, const Vector2& scale, SpriteEffects effects, float layerDepth)
	{
		assert(texture != nullptr);

		Vector4 destination;
		destination.X = position.X;
		destination.Y = position.Y;

		if (sourceRectangle != nullptr)
		{
			destination.Z = sourceRectangle->Width * scale.X;
			destination.W = sourceRectangle->Height * scale.Y;
		}
		else
		{
			destination.Z = texture->GetWidth() * scale.X;
			destination.W = texture->GetHeight() * scale.Y;
		}

		Vector4 source;
		if (sourceRectangle != nullptr)
		{
			source.X = (float)sourceRectangle->X;
			source.Y = (float)sourceRectangle->Y;
			source.Z = (float)sourceRectangle->Width;
			source.W = (float)sourceRectangle->Height;
		}
		else
		{
			source.X = 0;
			source.Y = 0;
			source.Z = (float)texture->GetWidth();
			source.W = (float)texture->GetHeight();
		}

		addSprite(texture, destination, source, color, rotation, origin, effects, layerDepth);
	}

	void SpriteBatch::Draw(Texture2D* texture, const Rectangle& destinationRectangle, const Rectangle* sourceRectangle, const Color& color,
		float rotation, const Vector2& origin, SpriteEffects effects, float layerDepth)
	{
		assert(texture != nullptr);

		Sprite s;
		s.Texture = texture;
		s.SpriteColor = color;
		s.Origin = origin;
		s.Destination.X = (float)destinationRectangle.X;
		s.Destination.Y = (float)destinationRectangle.Y;
		s.Destination.Z = (float)destinationRectangle.Width;
		s.Destination.W = (float)destinationRectangle.Height;
		s.Rotation = rotation;
		s.Depth = layerDepth;
		s.Effects = effects;
		
		if (sourceRectangle != nullptr)
		{
			s.Source.X = (float)sourceRectangle->X;
			s.Source.Y = (float)sourceRectangle->Y;
			s.Source.Z = (float)sourceRectangle->Width;
			s.Source.W = (float)sourceRectangle->Height;
		}
		else
		{
			s.Source.X = 0;
			s.Source.Y = 0;
			s.Source.Z = (float)texture->GetWidth();
			s.Source.W = (float)texture->GetHeight();
		}

		m_sprites.push_back(s);
	}

	void SpriteBatch::DrawString(SpriteFont* spriteFont, const char* text, const Vector2& position, const Color& color)
	{
		DrawString(spriteFont, text, position, color, 0, Nxna::Vector2(0, 0), 1.0f, SpriteEffects::None, 0);
	}

	void SpriteBatch::DrawString(SpriteFont* spriteFont, const wchar_t* text, const Vector2& position, const Color& color)
	{
		DrawString(spriteFont, text, position, color, 0, Nxna::Vector2(0, 0), 1.0f, SpriteEffects::None, 0);
	}

	void SpriteBatch::DrawStringUTF8(SpriteFont* spriteFont, const char* text, const Vector2& position, const Color& color)
	{
		DrawStringUTF8(spriteFont, text, position, color, 0, Nxna::Vector2(0, 0), 1.0f, SpriteEffects::None, 0);
	}

	void SpriteBatch::DrawString(SpriteFont* spriteFont, const char* text, const Vector2& position, const Color& color,
		float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth)
	{
		Matrix rotationMatrix;
		Matrix::CreateRotationZ(rotation, rotationMatrix);
		Matrix translationMatrix;
		Matrix::CreateTranslation(-origin.X * scale, -origin.Y * scale, 0, translationMatrix);
		Matrix transform;
		Matrix::Multiply(translationMatrix, rotationMatrix, transform);

		// TODO: do the sprite effects

		Vector2 cursor(position.X, position.Y);

		size_t len = strlen(text);
		for (size_t i = 0; i < len; i++)
		{
			char c = text[i];
			Rectangle glyph, cropping;
			Vector3 kerning;

			spriteFont->GetCharacterInfo(c, &glyph, &cropping, &kerning);

			cursor.X += kerning.X * scale;

			Vector2 position = cursor;
			position.X += cropping.X * scale;
			position.Y += cropping.Y * scale;

			Draw(spriteFont->m_texture, position, &glyph, color, rotation, Vector2(0,0), scale, effects, layerDepth); 

			cursor.X += (kerning.Y + kerning.Z) * scale;
		}
	}

	void SpriteBatch::DrawString(SpriteFont* spriteFont, const wchar_t* text, const Vector2& position, const Color& color,
		float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth)
	{
		Matrix rotationMatrix;
		Matrix::CreateRotationZ(rotation, rotationMatrix);
		Matrix translationMatrix;
		Matrix::CreateTranslation(-origin.X * scale, -origin.Y * scale, 0, translationMatrix);
		Matrix transform;
		Matrix::Multiply(translationMatrix, rotationMatrix, transform);

		// TODO: do the sprite effects

		Vector2 cursor(position.X, position.Y);

		size_t len = wcslen(text);
		bool ignoreSpacing = true; 
		for (size_t i = 0; i < len; i++)
		{
			wchar_t c = text[i];
			Rectangle glyph, cropping;
			Vector3 kerning;

			spriteFont->GetCharacterInfo(c, &glyph, &cropping, &kerning);

			if (!ignoreSpacing)
				cursor.X += spriteFont->GetSpacing() * scale;

			cursor.X += kerning.X * scale;

			Vector2 position = cursor;
			position.X += cropping.X * scale;
			position.Y += cropping.Y * scale;

			Draw(spriteFont->m_texture, position, &glyph, color, rotation, Vector2(0,0), scale, effects, layerDepth); 

			cursor.X += (kerning.Y + kerning.Z) * scale;

			ignoreSpacing = false;
		}
	}

	void SpriteBatch::DrawStringUTF8(SpriteFont* spriteFont, const char* text, const Vector2& position, const Color& color,
		float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth)
	{
		Matrix rotationMatrix;
		Matrix::CreateRotationZ(rotation, rotationMatrix);
		Matrix translationMatrix;
		Matrix::CreateTranslation(-origin.X * scale, -origin.Y * scale, 0, translationMatrix);
		Matrix transform;
		Matrix::Multiply(translationMatrix, rotationMatrix, transform);

		// TODO: do the sprite effects

		Vector2 cursor(position.X, position.Y);

		int pos = 0;
		int charsRead;
		unsigned int c;
		while ((charsRead = Utils::GetUTF8Character(text, pos, &c)) != 0)
		{
			pos += charsRead;

			Rectangle glyph, cropping;
			Vector3 kerning;

			spriteFont->GetCharacterInfo(c, &glyph, &cropping, &kerning);

			cursor.X += kerning.X * scale;

			Vector2 position = cursor;
			position.X += cropping.X * scale;
			position.Y += cropping.Y * scale;

			Draw(spriteFont->m_texture, position, &glyph, color, rotation, Vector2(0,0), scale, effects, layerDepth); 

			cursor.X += (kerning.Y + kerning.Z) * scale;
		}
	}

	void SpriteBatch::addSprite(Texture2D* texture, const Vector4& position, const Vector4& source, const Color& color, 
		float rotation, const Vector2& origin, SpriteEffects effects, float layerDepth)
	{
		Sprite s;
		s.Texture = texture;
		s.SpriteColor = color;
		s.Origin = origin;
		s.Destination = position;
		s.Source = source;
		s.Rotation = rotation;
		s.Depth = layerDepth;
		s.Origin = origin;
		s.Effects = effects;

		m_sprites.push_back(s);
	}

	void SpriteBatch::flush()
	{
		//setRenderStates();

		int numSprites = m_sprites.size();
		if (numSprites == 0) return;

		const int stride = 6;
		const int vertsPerSprite = 4;

		if (m_vertexBuffer == nullptr)
		{
			m_vertexBuffer = new DynamicVertexBuffer(m_device, m_declaration, MAX_BATCH_SIZE * vertsPerSprite, BufferUsage::WriteOnly);
		}
		else if (m_vertexBuffer->GetVertexCount() < numSprites * vertsPerSprite)
		{
			delete m_vertexBuffer;
			m_vertexBuffer = new DynamicVertexBuffer(m_device, m_declaration, numSprites * 2 * vertsPerSprite, BufferUsage::WriteOnly);
		}

		int sizeOfVertices = sizeof(float) * numSprites * vertsPerSprite * stride;
		void* workingMemory = NxnaTempMemoryPool::GetMemory(sizeOfVertices);
		float* workingVerts = (float*)workingMemory;

		auto itr = m_sprites.begin();
		for (int i = 0; i < numSprites; i++, itr++)
		{
			float* verts = &workingVerts[i * stride * vertsPerSprite];

			copyIntoVerts((*itr), verts);
		}

		m_vertexBuffer->SetData(workingVerts, numSprites * vertsPerSprite);

		Effect* effect = nullptr;
		EffectParameter* diffuse = nullptr;
		Texture2D* lastTexture = nullptr;
		if (m_customEffect == nullptr)
		{
			Viewport vp = m_device->GetViewport();
			float n1 = vp.Width > 0 ? (1.0f / (float)vp.Width) : 0;
			float n2 = vp.Height > 0 ? (-1.0f / (float)vp.Height) : 0;

			Matrix transform;
			Matrix::GetIdentity(transform);
			transform.M11 = n1 * 2.0f;
			transform.M22 = n2 * 2.0f;
			transform.M41 = -1.0f;// - n1; // these are pixel offsets (I think)
			transform.M42 = 1.0f;// - n2;

			Matrix final;
			Matrix::Multiply(m_customTransform, transform, final);

			diffuse = m_effect->GetParameter("Diffuse");
			m_effect->GetParameter("ModelViewProjection")->SetValue(final);

			effect = m_effect;
		}
		else
		{
			// find the first texture parameter and use that
			for (int j = 0; j < m_customEffect->GetNumParameters(); j++)
			{
				EffectParameter* param = m_customEffect->GetParameter(j);
				EffectParameterType type = param->GetType();
				if (type == EffectParameterType::Texture ||
					type == EffectParameterType::Texture1D ||
					type == EffectParameterType::Texture2D ||
					type == EffectParameterType::Texture3D ||
					type == EffectParameterType::TextureCube)
				{
					diffuse = param;
					break;
				}
			}

			effect = m_customEffect;
		}
		
		m_device->SetVertexBuffer(m_vertexBuffer);
		m_device->SetIndices(m_indexBuffer);

		int batchSize = 0;
		int vertexBufferStartIndex = 0;
		int indexBufferStartIndex = 0;
		for (int i = 0; i < numSprites; i++)
		{
			if (batchSize >= MAX_BATCH_SIZE)
			{
				m_device->DrawIndexedPrimitives(PrimitiveType::TriangleList, vertexBufferStartIndex, 0, batchSize * vertsPerSprite, indexBufferStartIndex, batchSize * 2);
				vertexBufferStartIndex += batchSize * 4;
				indexBufferStartIndex = 0;
				batchSize = 0;
			}
			
			if (lastTexture != m_sprites[i].Texture)
			{
				if (batchSize > 0)
				{
					m_device->DrawIndexedPrimitives(PrimitiveType::TriangleList, vertexBufferStartIndex, 0, batchSize * vertsPerSprite, indexBufferStartIndex, batchSize * 2);
					vertexBufferStartIndex += batchSize * 4;
					indexBufferStartIndex = 0;
					batchSize = 0;
				}

				diffuse->SetValue(m_sprites[i].Texture);

				effect->GetCurrentTechnique()->Apply();

				lastTexture = m_sprites[i].Texture;
			}
			
			batchSize++;
		}

		// render any remaining sprites
		if (batchSize > 0)
		{
			m_device->DrawIndexedPrimitives(PrimitiveType::TriangleList, vertexBufferStartIndex, 0, batchSize * vertsPerSprite, indexBufferStartIndex, batchSize * 2);
		}

		m_sprites.clear();
	}

	void SpriteBatch::copyIntoVerts(const Sprite& s, float* verts)
	{
		float cosine, sine;
		if (s.Rotation != 0)
		{
			cosine = cos(s.Rotation);
			sine = sin(s.Rotation);
		}
		else
		{
			cosine = 1.0f;
			sine = 0;
		}

		float adjustedOriginX = s.Origin.X / s.Source.Z;
		float adjustedOriginY = s.Origin.Y / s.Source.W;

		float inverseTextureWidth = 1.0f / (float)s.Texture->GetWidth();
		float inverseTextureHeight = 1.0f / (float)s.Texture->GetHeight();

        float x = s.Destination.X;
        float y = s.Destination.Y;
        float width = s.Destination.Z;
        float height = s.Destination.W;

		float o1 = (0 - adjustedOriginX) * width;
		float o2 = (0 - adjustedOriginY) * height;
		float o3 = (1 - adjustedOriginX) * width;
		float o4 = (0 - adjustedOriginY) * height;
		float o5 = (1 - adjustedOriginX) * width;
		float o6 = (1 - adjustedOriginY) * height;
		float o7 = (0 - adjustedOriginX) * width;
		float o8 = (1 - adjustedOriginY) * height;

		float texTLX = s.Source.X * inverseTextureWidth;
		float texTLY = s.Source.Y * inverseTextureHeight;
		float texBRX = (s.Source.X + s.Source.Z) * inverseTextureWidth;
		float texBRY = (s.Source.Y + s.Source.W) * inverseTextureHeight;

		if ((s.Effects & (int)SpriteEffects::FlipHorizontally) != 0)
		{
			float tmp = texTLX;
			texTLX = texBRX;
			texBRX = tmp;
		}

		if ((s.Effects & (int)SpriteEffects::FlipVertically) != 0)
		{
			float tmp = texTLY;
			texTLY = texBRY;
			texBRY = tmp;
		}

		unsigned int packedColor = s.SpriteColor.GetPackedValue();

        const int stride = 3 + 2 + 1;
        verts[0 * stride + 0] = x + o1 * cosine - o2 * sine;
        verts[0 * stride + 1] = y + o1 * sine + o2 * cosine;
		verts[0 * stride + 2] = s.Depth;
        verts[0 * stride + 3] = texTLX;
        verts[0 * stride + 4] = texTLY;
		memcpy(&verts[0 * stride + 5], &packedColor, sizeof(unsigned int));

        verts[1 * stride + 0] = x + o3 * cosine - o4 * sine;
        verts[1 * stride + 1] = y + o3 * sine + o4 * cosine;
		verts[1 * stride + 2] = s.Depth;
        verts[1 * stride + 3] = texBRX;
        verts[1 * stride + 4] = texTLY;
		memcpy(&verts[1 * stride + 5], &packedColor, sizeof(unsigned int));

        verts[2 * stride + 0] = x + o5 * cosine - o6 * sine;
        verts[2 * stride + 1] = y + o5 * sine + o6 * cosine;
		verts[2 * stride + 2] = s.Depth;
        verts[2 * stride + 3] = texBRX;
        verts[2 * stride + 4] = texBRY;
		memcpy(&verts[2 * stride + 5], &packedColor, sizeof(unsigned int));

        verts[3 * stride + 0] = x + o7 * cosine - o8 * sine;
        verts[3 * stride + 1] = y + o7 * sine + o8 * cosine;
		verts[3 * stride + 2] = s.Depth;
        verts[3 * stride + 3] = texTLX;
        verts[3 * stride + 4] = texBRY;
		memcpy(&verts[3 * stride + 5], &packedColor, sizeof(unsigned int));

	}

	void SpriteBatch::createIndexBuffer()
	{
		short indices[MAX_BATCH_SIZE * 6];
		for (short i = 0; i < MAX_BATCH_SIZE; i++)
		{
			indices[i * 6 + 0] = i * 4;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4;
			indices[i * 6 + 4] = i * 4 + 2;
			indices[i * 6 + 5] = i * 4 + 3;
		}

		m_indexBuffer = new Nxna::Graphics::IndexBuffer(m_device, IndexElementSize::SixteenBits);
		m_indexBuffer->SetData(indices, MAX_BATCH_SIZE * 6);
	}

	void SpriteBatch::setRenderStates()
	{
		m_device->SetBlendState(m_blend);
		m_device->SetRasterizerState(m_rasterizerState);
		m_device->SetDepthStencilState(m_depthStencilState);
		
		// TODO: this shouldn't be hard coded
		m_device->GetSamplerStates().Set(0, SamplerState::GetLinearClamp());
	}


	void SpriteBatch::Internal_Shutdown()
	{
		if (m_declaration != nullptr) delete m_declaration;
		if (m_indexBuffer != nullptr) delete m_indexBuffer;
		if (m_vertexBuffer != nullptr) delete m_vertexBuffer;
		if (m_effect != nullptr) delete m_effect;
	}
}
}

