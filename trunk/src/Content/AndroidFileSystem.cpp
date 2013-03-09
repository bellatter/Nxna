#include "AndroidFileSystem.h"
#include "FileStream.h"
#include "../Platform/Android/JNI.h"

#include <jni.h>
#include <cstdint>
#include <sys/types.h>
#include <unistd.h>

namespace Nxna
{
namespace Content
{
	class AndroidFDStream : public FileStream
	{
		int m_fd;
		jobject m_asset;
		jmethodID m_close;
		uint64_t m_offset;
		uint64_t m_length;

	public:

		AndroidFDStream(jobject asset)
		{
			// TODO: can getting pointers to all these methods be cached and done just once?

			JNIEnv* env = Platform::Android::JNI::GetEnv();

			m_asset = env->NewGlobalRef(asset);
			jclass assetClass = env->GetObjectClass(m_asset);

			// get the "close" method for later
			m_close = env->GetMethodID(assetClass, "close", "()V");

			// get the start offset of the file
			jmethodID getStartOffset = env->GetMethodID(assetClass, "getStartOffset", "()J");
			m_offset = env->CallLongMethod(m_asset, getStartOffset);

			// get the length of the file
			jmethodID getLength = env->GetMethodID(assetClass, "getDeclaredLength", "()J");
			m_length = m_env->CallLongMethod(m_asset, getLength);

			// now get the actual file descriptor
			jmethodID getFileDescriptor = env->GetMethodID(assetClass, "getFileDescriptor", "()Ljava/io/FileDescriptor;");
			jobject fd = env->CallObjectMethod(m_asset, getFileDescriptor);
			jclass fdClass = env->GetObjectClass(fd);
			jfieldID fdDesc = env->GetFieldID(fdClass, "descriptor", "I");

			m_fd = env->GetIntField(fd, fdDesc);
		}

		virtual ~AndroidFDStream()
		{
			JNIEnv* env = Platform::Android::JNI::GetEnv();

			env->CallVoidMethod(m_asset, m_close);
			env->DeleteGlobalRef(m_asset);
		}

		virtual int Read(byte* destination, int length) override
		{
			if (m_length != -1)
			{
				uint64_t maxLength = m_length - m_bytesRead;
				if (maxLength > length)
					length = maxLength;
			}

			int bytesRead = read(m_fd, destination, length);
			if (bytesRead == -1)
				return 0; // some sort of error occured

			m_bytesRead += bytesRead;

			return bytesRead;
		}

		virtual void Seek(int offset, SeekOrigin origin) override
		{
			int whence;

			if (origin == SeekOrigin::Begin)
			{
				if (m_length != -1 && offset > m_length)
					offset = m_length;

				offset += m_offset;
				whence = SEEK_SET;
			}
			else if (origin == SeekOrigin::End)
			{
				offset = m_offset + m_length + offset;
				whence = SEEK_SET;
			}
			else if (origin == SeekOrigin::Current)
			{
				whence = SEEK_CUR;
			}

			int r = lseek(m_fd, offset, whence);

			if (r != -1)
			{
				m_bytesRead = r - m_offset;
			}
		}

		virtual int Position() override
		{
			int p = lseek(m_fd, 0, SEEK_CUR);

			return p - m_offset;
		}

		virtual int Length() override
		{
			return m_length;
		}
	};

	class AndroidFileStream : public FileStream
	{
		static const int m_defaultBufferLength = 1024 * 8;
		jobject m_stream;
		jobject m_buffer;
		int m_bufferLength;
		jmethodID m_mark;
		jmethodID m_available;
		jmethodID m_read;
		jmethodID m_skip;
		jmethodID m_reset;
		jmethodID m_close;
		int m_length;

	public:
		AndroidFileStream(jobject stream)
		{
			m_bufferLength = m_defaultBufferLength;

			JNIEnv* env = Platform::Android::JNI::GetEnv();
			m_stream = env->NewGlobalRef(stream);
			m_buffer = env->NewGlobalRef(env->NewByteArray(m_bufferLength));

			jclass streamClass = env->GetObjectClass(m_stream);
			m_mark = env->GetMethodID(streamClass, "mark", "(I)V");
			m_available = env->GetMethodID(cls, "available", "()I");
			m_read = env->GetMethodID(cls, "read", "([BII)I");
			m_skip = env->GetMethodID(cls, "skip", "(J)J");
			m_reset = env->GetMethodID(cls, "reset", "()V");
			m_close = env->GetMethodID(cls, "close", "()V");

			env->CallVoidMethod(m_stream, m_mark, 100 * 1024 * 1024);
			m_length = env->CallIntMethod(m_stream, m_available);
		}

		virtual ~AndroidFileStream()
		{
			JNIEnv* env = Platform::Android::JNI::GetEnv();
			env->CallVoidMethod(m_stream, m_close);

			env->DeleteGlobalRef(m_buffer);
			env->DeleteGlobalRef(m_stream);
		}

		virtual int Read(byte* destination, int length) override
		{
			JNIEnv* env = Platform::Android::JNI::GetEnv();

			// expand the buffer if needed
			if (length > m_bufferLength)
			{
				m_bufferLength = length;

				env->DeleteGlobalRef(m_buffer);
				m_buffer = env->NewGlobalRef(env->NewByteArray(m_bufferLength));
			}

			int bytesRead = env->CallIntMethod(m_stream, m_read, m_buffer, 0, length);

			if (env->ExceptionCheck())
			{
				env->ExceptionClear();
				return 0;
			}
			else if (bytesRead == -1)
			{
				return 0;
			}
			else
			{
				env->GetByteArrayRegion(m_buffer, 0, bytesRead, destination);
				m_bytesRead += bytesRead;

				return bytesRead;
			}
		}

		virtual void Seek(int offset, SeekOrigin origin) override
		{
			JNIEnv* env = Platform::Android::JNI::GetEnv();

			int newPosition = 0;
			if (origin == SeekOrigin::Begin)
				newPosition = offset;
			else if (origin == SeekOrigin::Current)
				newPosition = m_bytesRead + offset;
			else
				newPosition = m_length + offset;

			// oh, why can't the stream have a seek method... instead we have to do this nonsense.
			int bytesToSkip;
			if (newPosition > m_bytesRead)
			{
				bytesToSkip = newPosition - m_bytesRead;
			}
			else
			{
				// rewind to the beginning
				env->CallVoidMethod(m_stream, m_reset);

				if (env->ExceptionCheck())
				{
					// ignore the exception, I guess?
					env->ExceptionClear();
					return;
				}

				m_bytesRead = 0;
				bytesToSkip = newPosition;
			}

			// now seek to the desired spot
			while(bytesToSkip > 0)
			{
				int64_t r = env->CallLongMethod(m_stream, m_skip, bytesToSkip);

				if (env->ExceptionCheck())
				{
					env->ExceptionClear();
					return;
				}
				else if (r == 0)
				{
					// seeking failed for some reason...
					return;
				}

				m_bytesRead += r;
				bytesToSkip -= r;
			}
		}

		virtual int Position() override
		{
			return m_bytesRead;
		}

		virtual int Length() override
		{
			return m_length;
		}
	};


	void* AndroidFileSystem::m_androidAssetManager = nullptr;
	void* AndroidFileSystem::m_openFd = nullptr;
	void* AndroidFileSystem::m_open = nullptr;

	void AndroidFileSystem::Initialize(void* assetManager)
	{
		JNIEnv* env = Platform::Android::JNI::GetEnv();
		m_androidAssetManager = env->NewGlobalRef(assetManager);
		jclass cls = env->GetObjectClass(m_androidAssetManager);

		// get method info
		m_openFd = env->GetMethodID(cls, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
		m_open = env->GetMethodID(cls, "open", "(Ljava/lang/String;I)Ljava/io/InputStream;");
	}

	void AndroidFileSystem::Shutdown()
	{
		JNIEnv* env = Platform::Android::JNI::GetEnv();
		env->DeleteGlobalRef(m_androidAssetManager);
	}

	FileStream* AndroidFileSystem::Open(const char* path)
	{
		JNIEnv* env = Platform::Android::JNI::GetEnv();
		jstring javaPath = env->NewStringUTF(path);

		// try openFd() first, since it's a LOT faster
		jobject fd = env->CallObjectMethod(m_androidAssetManager, m_openFd, javaPath);

		if (env->ExceptionCheck())
		{
			env->ExceptionClear();
		}
		else if (fd != 0)
		{
			// yay! it worked!
			env->DeleteLocalRef(javaPath);
			return new AndroidFDStream(fd);
		}

		// well, looks like we're going to have to use the slow path...
		jobject inputStream = env->CallObjectMethod(m_androidAssetManager, m_open, jpath, ACCESS_RANDOM);

		if (env->ExceptionCheck())
		{
			// doh! we couldn't open the file that way either! We'll just have to return null.
			env->ExceptionClear();
			env->DeleteLocalRef(javaPath);

			return nullptr;
		}

		// it worked!
		return new AndroidFileStream(inputStream);
	}
}
}