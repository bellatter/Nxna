#include <cassert>
#include <fstream>
#include "EffectWriter.h"
#include "EffectXml.h"
#include "ResizableBuffer.h"

#define WRITE_INT(v) {int val = v; stream.write((char*)&val, 4); }
#define WRITE_BYTE(b) { unsigned char val = b; stream.write((char*)&val, 1); }
#define WRITE_SHORT(s) { unsigned short val = s; stream.write((char*)&val, 2); }

void EffectWriter::Write(const EffectXml& effect, ResizableBuffer* stream)
{
	const char header[] = {'N', 'X', 'F', 'X'}; // "not xna fx"
	stream->Write(header, 4);

	// write version
	stream->WriteByte(1);

	auto& techniques = effect.GetTechniques();
	auto& cbuffers = effect.GetCBuffers();
	auto& textures = effect.GetTextures();
	auto& shaders = effect.GetShaders();
	auto& techMaps = effect.GetTechniqueMaps();

	stream->WriteShort(techniques.size());
	stream->WriteShort(cbuffers.size());
	stream->WriteShort(textures.size());
	stream->WriteShort(shaders.size());
	stream->WriteShort(techMaps.size());

	// write the techniques
	for (auto itr = techniques.begin(); itr != techniques.end(); itr++)
	{
		stream->WriteByte((*itr).Name.length());
		stream->Write((*itr).Name.c_str(), (*itr).Name.length());
		stream->WriteByte((*itr).Hidden ? 1 : 0);

		stream->WriteByte((*itr).Attributes.size());
		for (auto itr2 = (*itr).Attributes.begin(); itr2 != (*itr).Attributes.end(); itr2++)
		{
			stream->WriteByte((*itr2).Name.length());
			stream->Write((*itr2).Name.c_str(), (*itr2).Name.length());

			stream->WriteByte((*itr2).Type);
			stream->WriteByte((*itr2).NumElements);
			stream->WriteByte((*itr2).Semantic);
			stream->WriteByte((*itr2).Index);
		}
	}

	// write the cbuffers
	for (auto itr = cbuffers.begin(); itr != cbuffers.end(); itr++)
	{
		stream->WriteByte((*itr).Constants.size());
		for (auto itr2 = (*itr).Constants.begin(); itr2 != (*itr).Constants.end(); itr2++)
		{
			stream->WriteByte((*itr2).Name.length());
			stream->Write((*itr2).Name.c_str(), (*itr2).Name.length());
			stream->WriteByte((*itr2).Type);
			stream->WriteByte((*itr2).NumElements);
		}
	}

	// write the textures
	for (auto itr = textures.begin(); itr != textures.end(); itr++)
	{
		stream->WriteByte((*itr).length());
		stream->Write((*itr).c_str(), (*itr).length());
	}

	// write the shaders
	for (auto itr = shaders.begin(); itr != shaders.end(); itr++)
	{
		stream->WriteInt((*itr).SourceCode.size());
		stream->Write((char*)(*itr).SourceCode.data(), (*itr).SourceCode.size());
	}

	// write the technique map
	for (auto itr = techMaps.begin(); itr != techMaps.end(); itr++)
	{
		// find the technique with the given name
		int techniqueIndex = effect.FindTechniqueByName((*itr).Name.c_str());
		assert(techniqueIndex >= 0);

		// find the shader with the given name
		int vertexShader = effect.FindShaderByName((*itr).VertexShader.c_str());
		int pixelShader = effect.FindShaderByName((*itr).PixelShader.c_str());
		assert(vertexShader >= 0);
		assert(pixelShader >= 0);

		stream->WriteShort((*itr).Profile);
		stream->WriteShort(techniqueIndex);
		stream->WriteShort(vertexShader);
		stream->WriteShort(pixelShader);
	}
}

void EffectWriter::writeInt(int value)
{
}