#ifndef NXNA_GRAPHICS_GRAPHICSDEVICE_H
#define NXNA_GRAPHICS_GRAPHICSDEVICE_H

#ifdef NXNA_ENABLE_DIRECT3D11
#include <d3d11.h>
#endif

#ifdef NXNA_ENABLE_MATH
#include "../Vector3.h"
#include "../Matrix.h"
#endif

#include "PipelineState.h"


/// Nxna2
namespace Nxna
{
	/// Graphics
	namespace Graphics
	{
		/// The type of graphics device
		enum class GraphicsDeviceType
		{
			/// Represents no renderer
			None, 

			/// OpenGL 4.1
			OpenGl41,

			/// OpenGL ES 3.0
			OpenGLES3,
			
			/// Direct3D 11
			Direct3D11,

#ifndef NDEBUG
			LAST
#endif
		};
		
		union Capabilities
		{
			struct
			{
				bool SupportsS3tcTextureCompression;
			} OpenGLCaps;
		};

		enum class Usage
		{
			/// Default write-only usage
			Default,

			/// Write-once
			Immutable,

			/// Write lots of times
			Dynamic
		};

		enum class PrimitiveType
		{
			/// A list of triangles
			TriangleList,

			/// A triangle strip
			TriangleStrip
		};

		enum class IndexElementSize
		{
			/// 16 bit indices
			SixteenBits = 2,

			/// 32 bit indices
			ThirtyTwoBits = 4
		};

		struct IndexBuffer
		{
			union
			{
				struct
				{
					unsigned int ByteLength;
					unsigned int Buffer;
				} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
				struct
				{
					ID3D11Buffer* Buffer;
				} Direct3D11;
#endif
			};

			IndexElementSize ElementSize;

#ifndef NXNA_DISABLE_VALIDATION
			unsigned int ByteLength;
			Usage BufferUsage;
#endif
		};

		struct VertexBuffer
		{
			union
			{
				struct
				{
					unsigned int ByteLength;
					unsigned int Buffer;
					unsigned int VAO;
				} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
				struct
				{
					ID3D11Buffer* Buffer;
				} Direct3D11;
#endif
			};

#ifndef NXNA_DISABLE_VALIDATION
			unsigned int ByteLength;
			Usage BufferUsage;
#endif
		};

		struct ConstantBuffer
		{
			union
			{
				struct
				{
					unsigned int ByteLength;
					unsigned int UniformBuffer;
				} OpenGL;
				struct
				{
					ID3D11Buffer* Buffer;
				} Direct3D11;
			};

#ifndef NXNA_DISABLE_VALIDATION
			Usage BufferUsage;
			unsigned int ByteLength;
#endif
		};

		enum class BufferType
		{
			/// An index buffer
			Index,

			/// A vertex buffer
			Vertex,

			/// A constant (or uniform) buffer
			Constant
		};

		struct Buffer
		{
			union
			{
				struct
				{
					unsigned int BufferHandle;
				} OpenGL;
				struct
				{
					ID3D11Buffer* BufferPtr;
				} Direct3D11;
			};

#ifndef NXNA_DISABLE_VALIDATION
			BufferType Type;
			Usage BufferUsage;
			unsigned int ByteLength;
#endif
		};
		

		struct Viewport
		{
			Viewport()
			{
				X = 0;
				Y = 0;
				Width = 0;
				Height = 0;
			}

			Viewport(int x, int y, int width, int height)
			{
				X = x;
				Y = y;
				Width = width;
				Height = height;
			}

			float GetAspectRatio() const { return Width / (float)Height; }

#ifdef NXNA_ENABLE_MATH
			Vector3 Project(const Vector3& source,
				const Matrix& project,
				const Matrix& view,
				const Matrix& world);
#endif
			void Project(const float* source3f, 
				const float* projectMatrix16f, 
				const float* viewMatrix16f,
				const float* worldMatrix16f,
				float* result3f);

#ifdef NXNA_ENABLE_MATH
			Vector3 Unproject(const Vector3& source,
				const Matrix& projection,
				const Matrix& view,
				const Matrix& world);
#endif

#ifdef NXNA_ENABLE_MATH
			Rectangle GetBounds()
			{
				return Rectangle(X, Y, Width, Height);
			}
#endif

			int X, Y, Width, Height;
		};

		struct GraphicsDeviceDebugMessage
		{
			GraphicsDeviceType DeviceType;
			const char* Message;
		};

		enum class ShaderType
		{
			Vertex,
			Pixel
		};

		typedef void (*GraphicsDeviceMessageCallback)(GraphicsDeviceDebugMessage);

		/// A struct full of parameters used to create a new GraphicsDevice
		///
		/// In the case of OpenGL, you have the option of using *just* the features of the particular
		/// context version you requested, or you can enable extensions and allow the renderer to
		/// take advantage of newer features that may be present and provide better performance.
		/// This lets the renderer do things like target Mac OSX, which only supports OpenGL 4.1,
		/// but on platforms that support OpenGL 4.5 the renderer can do things like direct state access.
		struct GraphicsDeviceCreationParams
		{
			GraphicsDeviceType Type;
			int ScreenWidth;
			int ScreenHeight;

			union
			{
				struct
				{
					/// Whether to allow the renderer to use extensions beyond what is present
					/// in the core OpenGL version requested
					bool AllowExtensionUse;
				} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
				struct
				{
					ID3D11Device* Device;
					ID3D11DeviceContext* DeviceContext;
					ID3D11RenderTargetView* RenderTargetView;
					IDXGISwapChain* SwapChain;
				} Direct3D11;
#endif
			};
		};

#ifdef NXNA_ENABLE_DIRECT3D11
		struct D3D11DeviceState
		{
			ID3D11Device* Device;
			ID3D11DeviceContext* Context;
			ID3D11RenderTargetView* RenderTargetView;
			IDXGISwapChain* SwapChain;
		};
#endif

		enum class SurfaceFormat
		{
			Color,
			Bgr565,
			Bgra5551,
			Bgra4444,
			Dxt1,
			Dxt3,
			Dxt5,

			// the following are not supported by XNA. These are our own
			// extensions so that iOS devices can have compressed textures too.
			Pvrtc4
		};

		struct TextureCreationDesc
		{
			int Width;
			int Height;
			int MipLevels;
			Usage TextureUsage;
			SurfaceFormat Format;

			void* InitialData;
			size_t InitialDataByteCount;
		};

		struct Texture2D
		{
			union
			{
				struct
				{
					unsigned int Handle;
				} OpenGL;
#ifdef NXNA_ENABLE_DIRECT3D11
				struct
				{
					ID3D11ShaderResourceView* m_shaderResourceView;
					ID3D11Texture2D* m_texture;
				} Direct3D11;
#endif
			};
		};

		struct Texture2DExtra
		{
			Texture2D Texture;
			int Width;
			int Height;
		};

		struct IndexBufferDesc
		{
			IndexElementSize ElementSize;
			int NumElements;
			Usage BufferUsage;

			void* InitialData;
			unsigned int InitialDataByteCount;
		};

		struct VertexBufferDesc
		{
			int NumVertices;
			Usage BufferUsage;
			InputElement* InputElements;
			int NumInputElements;
			int StrideBytes;

			void* InitialData;
			size_t InitialDataByteCount;
		};

		struct ConstantBufferDesc
		{
			Usage BufferUsage;
			unsigned int ByteCount;

			void* InitialData;
		};

		enum class MapType
		{
			Write,
			WriteDiscard,
			WriteNoOverwrite
		};

		class GraphicsDevice
		{
			GraphicsDeviceType m_type;
			Capabilities m_caps;
			int m_screenWidth, m_screenHeight;

			IndexBuffer m_indices;
			VertexBuffer m_vertices;
			ShaderPipeline* m_shaderPipeline;
			BlendState* m_blendState;
			RasterizerState* m_rasterizerState;

			NxnaResultDetails m_errorDetails;

#ifdef NXNA_ENABLE_DIRECT3D11
			D3D11DeviceState m_d3d11State;
#endif

		public:
			GraphicsDevice(const GraphicsDeviceCreationParams* params);
			void SetMessageCallback(GraphicsDeviceMessageCallback callback);

			GraphicsDeviceType GetType() { return m_type; }
			const NxnaResultDetails* GetErrorDetails() { return &m_errorDetails; }

			Capabilities* GetCaps() { return &m_caps; }

			/// Sets the current viewport
			void SetViewport(int x, int y, int width, int height);
			/// Sets the current viewport
			void SetViewport(Viewport viewport);

			/// Creates a new shader object
			/// @param[in] type The type of shader to create
			/// @param[in] bytecode A pointer to the bytecode of the shader. This can be Direct3D bytecode, or GLSL source code.
			/// @param[in] bytecodeLength The length in bytes of the bytecode
			/// @param[out] result A pointer to a Shader object
			/// @return NxnaResult::Success if successful, or an error otherwise
			NxnaResult CreateShader(ShaderType type, const void* bytecode, int bytecodeLength, Shader* result);

			/// Destroys an existing shader object
			/// @param[in] shader A pointer to the shader object to delete
			///
			/// Destroying a shader that is being used (including as part of a ShaderPipeline) is allowed.
			/// The underlying API will delete the shader when appropriate.
			void DestroyShader(Shader* shader);

			/// Create a new texture object
			/// @param[in] desc A pointer to a TextureCreationDesc object describing the new texture
			/// @param[out] result A pointer to a Texture2D object
			/// @return NxnaResult::Success if successful, or an error otherwise
			NxnaResult CreateTexture2D(const TextureCreationDesc* desc, Texture2D* result);
			void BindTexture(Texture2D* texture, int textureUnit);

			/// Destroys an existing texture object
			/// @param[in] texture A pointer to the texture to delete
			void DestroyTexture2D(Texture2D* texture);

			/// Create a new ShaderPipeline object
			/// @param[in] desc A pointer to a ShaderPipelineDesc object describing the new shader pipeline
			/// @param[out] result A pointer to a Texture2D object
			/// @return NxnaResult::Success if successful, or an error otherwise
			NxnaResult CreateShaderPipeline(const ShaderPipelineDesc* desc, ShaderPipeline* result);
			void SetShaderPipeline(ShaderPipeline* pipeline);

			/// Destroys an existing shader pipeline object
			/// @param[in] pipeline A pointer to the shader pipeline object to delete
			void DestroyShaderPipeline(ShaderPipeline* pipeline);

			/// Create a new BlendState object
			/// @param[in] desc A pointer to a BlendStateDesc object describing the new blend state
			/// @param[out] result A pointer to a BlendState object
			/// @return NxnaResult::Success if successful, or an error otherwise
			NxnaResult CreateBlendState(const BlendStateDesc* desc, BlendState* result);
			void SetBlendState(BlendState* state);

			/// Destroys an existing shader object
			/// @param[in] state A pointer to the blend state to delete
			void DestroyBlendState(BlendState* state);

			NxnaResult CreateRasterizerState(const RasterizerStateDesc* desc, RasterizerState* result);
			void SetRasterizerState(RasterizerState* state);
			void DestroyRasterizerState(RasterizerState* state);

			/// Create a new IndexBuffer object
			/// @param[in] desc A pointer to a IndexBufferDesc object describing the new index buffer
			/// @param[out] result A pointer to a IndexBuffer object
			/// @return NxnaResult::Success if successful, or an error otherwise
			NxnaResult CreateIndexBuffer(const IndexBufferDesc* desc, IndexBuffer* result);
			/// Destroys an existing index buffer
			/// @param[in] buffer The index buffer to delete
			void DestroyIndexBuffer(IndexBuffer buffer);
			/// Updates the contents of an IndexBuffer
			///
			/// This is equivalent to glBufferSubData() on OpenGL and ID3D11DeviceContext::UpdateSubresource() on Direct3D 11
			void UpdateIndexBuffer(IndexBuffer buffer, unsigned int startOffset, void* data, unsigned int dataLengthInBytes);
			void SetIndices(IndexBuffer indices);

			/// Create a new VertexBuffer object
			/// @param[in] desc A pointer to a VertexBufferDesc object describing the new vertex buffer
			/// @param[out] result A pointer to a VertexBuffer object
			/// @return True if successful, false otherwise
			NxnaResult CreateVertexBuffer(const VertexBufferDesc* desc, VertexBuffer* result);
			/// Destroys an existing vertex buffer
			/// @param[in] buffer The vertex buffer to delete
			void DestroyVertexBuffer(VertexBuffer buffer);
			/// Updates the contents of a VertexBuffer
			///
			/// This is equivalent to glBufferSubData() on OpenGL and ID3D11DeviceContext::UpdateSubresource() on Direct3D 11
			void UpdateVertexBuffer(VertexBuffer buffer, unsigned int startOffset, void* data, unsigned int dataLengthInBytes);
			void SetVertexBuffer(VertexBuffer vertexBuffer, unsigned int offset, unsigned int stride);

			/// Create a new ConstantBuffer object
			/// @param[in] desc A pointer to a ConstantBufferDesc object describing the new constant buffer
			/// @param[out] result A pointer to a ConstantBuffer object
			/// @return NxnaResult::Success if successful, or an error otherwise
			NxnaResult CreateConstantBuffer(const ConstantBufferDesc* desc, ConstantBuffer* result);
			/// Updates the contents of a ConstantBuffer
			///
			/// This is equivalent to glBufferSubData() on OpenGL and ID3D11DeviceContext::UpdateSubresource() on Direct3D 11
			void UpdateConstantBuffer(ConstantBuffer buffer, void* data, unsigned int dataLengthInBytes);
			void SetConstantBuffer(ConstantBuffer buffer, int index);
			/// Destroys an existing vertex buffer
			/// @param[in] buffer A pointer to the constant buffer to delete
			void DestroyConstantBuffer(ConstantBuffer* buffer);

			/// Maps an IndexBuffer for writing or reading
			/// @param[in] buffer An index buffer to map
			/// @param[in] type The type of mapping to perform
			/// @return A pointer to memory representing the contents of the buffer, or nullptr if unable to map the buffer
			///
			/// This is equivalent to glMapBufferRange() on OpenGL or ID3D11DeviceContext::Map() on Direct3D 11
			void* MapBuffer(IndexBuffer buffer, MapType type);
			/// Maps a VertexBuffer for writing or reading
			/// @param[in] buffer A vertex buffer to map
			/// @param[in] type The type of mapping to perform
			/// @return A pointer to memory representing the contents of the buffer, or nullptr if unable to map the buffer
			///
			/// This is equivalent to glMapBufferRange() on OpenGL or ID3D11DeviceContext::Map() on Direct3D 11
			void* MapBuffer(VertexBuffer buffer, MapType type);
			/// Maps a ConstantBuffer for writing or reading
			/// @param[in] buffer A constant buffer to map
			/// @param[in] type The type of mapping to perform
			/// @return A pointer to memory representing the contents of the buffer, or nullptr if unable to map the buffer
			///
			/// This is equivalent to glMapBufferRange() on OpenGL or ID3D11DeviceContext::Map() on Direct3D 11
			void* MapBuffer(ConstantBuffer buffer, MapType type);

			/// Unmaps a previously mapped IndexBuffer
			/// @param[in] buffer An index buffer
			void UnmapBuffer(IndexBuffer buffer);
			/// Unmaps a previously mapped VertexBuffer
			/// @param[in] buffer A vertex buffer
			void UnmapBuffer(VertexBuffer buffer);
			/// Unmaps a previously mapped ConstantBuffer
			/// @param[in] buffer A constant buffer
			void UnmapBuffer(ConstantBuffer buffer);

			void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount);
			void DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount);

			void Clear(float r, float g, float b, float a);
			void Present();

#ifdef NXNA_ENABLE_DIRECT3D11
			/// Gets a struct containing all the Direct3D objects it has (such as the ID3D11Device, ID3D11DeviceContext, etc)
			///
			/// Don't call this method if the GraphicsDevice is not a Direct3D 11 device. Bad things may or may not happen.
			const D3D11DeviceState* GetD3D11DeviceState() { return &m_d3d11State; }
#endif

		private:
			void applyDirtyStates();
		};
	}
}

#endif // NXNA_GRAPHICS_GRAPHICSDEVICE_H
