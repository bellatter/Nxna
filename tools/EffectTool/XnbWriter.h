#ifndef XNBWRITER_H
#define XNBWRITER_H

#include <fstream>
#include "ResizableBuffer.h"

class XnbWriter
{
	ResizableBuffer* m_buffer;

public:
	XnbWriter(ResizableBuffer* buffer);
	void Finalize();
};

#endif // XNBWRITER_H
