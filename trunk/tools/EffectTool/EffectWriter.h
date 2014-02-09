#ifndef EFFECTWRITER_H
#define EFFECTWRITER_H

#include <fstream>

class EffectXml;
class ResizableBuffer;

class EffectWriter
{
public:
	static void Write(const EffectXml& effect, ResizableBuffer* stream);

private:
	static void writeInt(int value);
};

#endif // EFFECTWRITER_H
