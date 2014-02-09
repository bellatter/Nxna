#ifndef HEADERCONVERTER_H
#define HEADERCONVERTER_H

#include "ResizableBuffer.h"

class HeaderConverter
{
public:
	static void Convert(const char* arrayName, const unsigned char* data, int length, ResizableBuffer* output)
	{
		output->WriteString("const unsigned char ", false);
		output->WriteString(arrayName, false);
		output->WriteString("[] = {\n", false);
		
		const int columnsPerLine = 6;
		int currentColumn= 0;

		for (int i = 0; i < length; i++)
		{
			unsigned char byteValue = data[i];

			if (currentColumn == 0)
				output->WriteString("\t", false);
			else
				output->WriteString(" ", false);

			char buffer[4];
			getByteString(byteValue, buffer);

			output->WriteString(buffer, false);

			if (i < length - 1)
				output->WriteString(",", false);

			currentColumn++;
			if (currentColumn >= columnsPerLine)
			{
				output->WriteString("\n", false);
				currentColumn = 0;
			}
		}

		output->WriteString("};", false);
	}

private:
	static void getByteString(unsigned char byteValue, char buffer[4])
	{
		buffer[0] = ' ';
		buffer[1] = ' ';

		char* start = buffer;

		if (byteValue < 100 && byteValue > 9)
			start = &buffer[1];
		else if (byteValue <= 9)
			start = &buffer[2];

		sprintf(start, "%d", byteValue);
	}
};

#endif // HEADERCONVERTER_H
