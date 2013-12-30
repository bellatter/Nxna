#ifndef GRAPHICS_SPRITEBATCH_H
#define GRAPHICS_SPRITEBATCH_H

#include <vector>
#include "../Color.h"
#include "../Rectangle.h"
#include "../Matrix.h"
#include "../Vector4.h"
#include "GraphicsDevice.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(SpriteSortMode)
		BackToFront,
		Deferred,
		FrontToBack,
		Immediate,
		Texture
	END_NXNA_ENUM(SpriteSortMode)

	NXNA_ENUM(SpriteEffects)
		None = 0,
		FlipHorizontally = 2,
		FlipVertically = 4
	END_NXNA_ENUM(SpriteEffects)

	class Texture2D;
	class Effect;
	class SpriteFont;
	class SpriteEffect;

	class SpriteBatch
	{
		static const int MAX_BATCH_SIZE = 2048;

		GraphicsDevice* m_device;
		Effect* m_customEffect;
		SpriteSortMode m_sortMode;
		const BlendState* m_blend;
		const DepthStencilState* m_depthStencilState;
		const RasterizerState* m_rasterizerState;
		Matrix m_customTransform;

		struct Sprite
		{
			Texture2D* Texture;
			Color SpriteColor;
			Vector4 Source;
			Vector4 Destination;
			Vector2 Origin;
			float Rotation;
			float Depth;
			SpriteEffects Effects;
		};

		std::vector<Sprite> m_sprites;

		static SpriteEffect* m_effect;
		static VertexDeclaration* m_declaration;
		static DynamicVertexBuffer* m_vertexBuffer;
		static IndexBuffer* m_indexBuffer;
		static float* m_workingVerts;
		static int m_workingVertsSize;

	public:
		SpriteBatch(GraphicsDevice* device);

		GraphicsDevice* GetGraphicsDevice() { return m_device; }

		void Begin();
		void Begin(SpriteSortMode sortMode, const BlendState* blendState, 
			const DepthStencilState* depthStencilState, const RasterizerState* rasterizerState, Effect* effect);
		void Begin(SpriteSortMode sortMode, const BlendState* blendState, 
			const DepthStencilState* depthStencilState, const RasterizerState* rasterizerState, Effect* effect, const Matrix& transform);

		void Draw(Texture2D* texture, const Vector2& position, const Color& color);
		void Draw(Texture2D* texture, const Rectangle& destinationRectangle, const Color& color);
		void Draw(Texture2D* texture, const Vector2& position, const Rectangle* sourceRectangle, const Color& color);
		void Draw(Texture2D* texture, const Rectangle& destinationRectangle, const Rectangle* sourceRectangle, const Color& color);

		void Draw(Texture2D* texture, const Rectangle& destinationRectangle, const Rectangle* sourceRectangle, const Color& color,
			float rotation, const Vector2& origin, SpriteEffects effects, float layerDepth);
		void Draw(Texture2D* texture, const Vector2& position, const Rectangle* sourceRectangle, const Color& color,
			float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth);
		void Draw(Texture2D* texture, const Vector2& position, const Rectangle* sourceRectangle, const Color& color,
			float rotation, const Vector2& origin, const Vector2& scale, SpriteEffects effects, float layerDepth);

		void DrawString(SpriteFont* spriteFont, const wchar_t* text, const Vector2& position, const Color& color);
		void DrawString(SpriteFont* spriteFont, const wchar_t* text, const Vector2& position, const Color& color,
			float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth);
		void DrawString(SpriteFont* spriteFont, const char* text, const Vector2& position, const Color& color);
		void DrawString(SpriteFont* spriteFont, const char* text, const Vector2& position, const Color& color,
			float rotation, const Vector2& origin, float scale, SpriteEffects effects, float layerDepth);

		void End();

		static void Internal_Shutdown();

	private:

		void addSprite(Texture2D* texture, const Vector4& position, const Vector4& source, const Color& color, 
			float rotation, const Vector2& origin, SpriteEffects effects, float layerDepth);

		void flush();
		void copyIntoVerts(const Sprite& s, float* verts);
		void createIndexBuffer();
		void setRenderStates();
	};
}
}

#endif // GRAPHICS_SPRITEBATC_H
