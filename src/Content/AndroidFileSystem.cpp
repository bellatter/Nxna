#include "AndroidFileSystem.h"
#include "FileStream.h"
#include "../Platform/Android/JNI.h"

#include <jni.h>
#include <android/log.h>
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
		jlong m_offset;
		jlong m_length;

	public:

		AndroidFDStream(jobject asset)
		{
			// TODO: can getting pointers to all these methods be cached and done just once?

			JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());

			m_asset = env->NewGlobalRef(asset);
			jclass assetClass = env->GetObjectClass(m_asset);

			// get the "close" method for later
			m_close = env->GetMethodID(assetClass, "close", "()V");

			// get the start offset of the file
			jmethodID getStartOffset = env->GetMethodID(assetClass, "getStartOffset", "()J");
			m_offset = env->CallLongMethod(m_asset, getStartOffset);

			// get the length of the file
			jmethodID getLength = env->GetMethodID(assetClass, "getDeclaredLength", "()J");
			m_length = env->CallLongMethod(m_asset, getLength);

			// now get the actual file descriptor
			jmethodID getFileDescriptor = env->GetMethodID(assetClass, "getFileDescriptor", "()Ljava/io/FileDescriptor;");
			jobject fd = env->CallObjectMethod(m_asset, getFileDescriptor);
			jclass fdClass = env->GetObjectClass(fd);
			jfieldID fdDesc = env->GetFieldID(fdClass, "descriptor", "I");

			m_fd = env->GetIntField(fd, fdDesc);
			
			lseek(m_fd, m_offset, SEEK_SET);
			
			__android_log_print(ANDROID_LOG_DEBUG, "CNK", "FD = %d, offset = %ld, length = %ld", m_fd, m_offset, m_length);
		}

		virtual ~AndroidFDStream()
		{
			JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());

			env->CallVoidMethod(m_asset, m_close);
			env->DeleteGlobalRef(m_asset);
		}

		virtual int Read(byte* destination, int length) override
		{
			__android_log_print(ANDROID_LOG_DEBUG, "CNK", "Reading %d bytes from AndroidFDStream", length);
			
			if (m_length != -1)
			{
				uint64_t maxLength = m_length - m_bytesRead;
				if (length > maxLength)
					length = maxLength;
			}
			
			__android_log_print(ANDROID_LOG_DEBUG, "CNK", "Actually... reading %d bytes from AndroidFDStream", length);

			int bytesRead = read(m_fd, destination, length);
			if (bytesRead == -1)
			{
				__android_log_print(ANDROID_LOG_DEBUG, "CNK", "Unknown error while reading from AndroidFDStream");
				return 0; // some sort of error occured
			}

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
			else
			{
				__android_log_print(ANDROID_LOG_DEBUG, "CNK", "Unknown error while seeking in AndroidFDStream");
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
		
		virtual bool IsOpen() override
		{
			return true;
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

			JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());
			m_stream = env->NewGlobalRef(stream);
			m_buffer = env->NewGlobalRef(env->NewByteArray(m_bufferLength));

			jclass streamClass = env->GetObjectClass(m_stream);
			m_mark = env->GetMethodID(streamClass, "mark", "(I)V");
			m_available = env->GetMethodID(streamClass, "available", "()I");
			m_read = env->GetMethodID(streamClass, "read", "([BII)I");
			m_skip = env->GetMethodID(streamClass, "skip", "(J)J");
			m_reset = env->GetMethodID(streamClass, "reset", "()V");
			m_close = env->GetMethodID(streamClass, "close", "()V");

			env->CallVoidMethod(m_stream, m_mark, 100 * 1024 * 1024);
			m_length = env->CallIntMethod(m_stream, m_available);
		}

		virtual ~AndroidFileStream()
		{
			JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());
			env->CallVoidMethod(m_stream, m_close);
			
			if (env->ExceptionCheck())
				env->ExceptionClear();

			__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "deleting buffer with length of %d", m_bufferLength);
			
			env->DeleteGlobalRef(m_buffer);
			env->DeleteGlobalRef(m_stream);
		}

		virtual int Read(byte* destination, int length) override
		{
			JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());

			// expand the buffer if needed
			if (length > m_bufferLength)
			{
				__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "recreating buffer with old length of %d and new length of %d", m_bufferLength, length);
				
				m_bufferLength = length;

				env->DeleteGlobalRef(m_buffer);
				m_buffer = env->NewGlobalRef(env->NewByteArray(m_bufferLength));
			}

			int bytesRead = env->CallIntMethod(m_stream, m_read, m_buffer, 0, length);

			if (env->ExceptionCheck())
			{
				__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Exception while trying to read using AndroidFileStream");
				
				env->ExceptionClear();
				return 0;
			}
			else if (bytesRead == -1)
			{
				__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Unknown error while trying to read using AndroidFileStream");
				
				return 0;
			}
			else
			{
				env->GetByteArrayRegion(static_cast<jbyteArray>(m_buffer), 0, bytesRead, reinterpret_cast<jbyte*>(destination));
				m_bytesRead += bytesRead;

				__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Read %d bytes", bytesRead);
				
				return bytesRead;
			}
		}

		virtual void Seek(int offset, SeekOrigin origin) override
		{
			JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());
			
			__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Seek of %d bytes requested", offset);

			int newPosition = 0;
			if (origin == SeekOrigin::Begin)
				newPosition = offset;
			else if (origin == SeekOrigin::Current)
				newPosition = m_bytesRead + offset;
			else
				newPosition = m_length + offset;

			// oh, why can't the stream have a seek method... instead we have to do this nonsense.
			jlong bytesToSkip;
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
			
			__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "bytesToSkip = %ld", bytesToSkip);

			// now seek to the desired spot
			while(bytesToSkip > 0)
			{
				jlong r = env->CallLongMethod(m_stream, m_skip, bytesToSkip);

				if (env->ExceptionCheck())
				{
					__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Exception while seeking AndroidFileStream");
					
					env->ExceptionClear();
					return;
				}
				else if (r == 0)
				{
					// seeking failed for some reason...
					__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Unknown error while seeking AndroidFileStream");
					
					return;
				}

				__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Seeked %ld bytes", r);
				
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
		
		virtual bool IsOpen() override
		{
			return true;
		}
	};


	void* AndroidFileSystem::m_androidAssetManager = nullptr;
	void* AndroidFileSystem::m_openFd = nullptr;
	void* AndroidFileSystem::m_open = nullptr;

	void AndroidFileSystem::Initialize(void* assetManager)
	{
		JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());
		m_androidAssetManager = env->NewGlobalRef(static_cast<jobject>(assetManager));
		jclass cls = env->GetObjectClass(static_cast<jobject>(m_androidAssetManager));

		// get method info
		m_openFd = env->GetMethodID(cls, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
		m_open = env->GetMethodID(cls, "open", "(Ljava/lang/String;I)Ljava/io/InputStream;");
	}

	void AndroidFileSystem::Shutdown()
	{
		JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());
		env->DeleteGlobalRef(static_cast<jobject>(m_androidAssetManager));
	}

	FileStream* AndroidFileSystem::Open(const char* path)
	{
		JNIEnv* env = static_cast<JNIEnv*>(Platform::Android::JNI::GetEnv());
		jstring javaPath = env->NewStringUTF(path);

		// try openFd() first, since it's a LOT faster
		jobject fd = env->CallObjectMethod(static_cast<jobject>(m_androidAssetManager), static_cast<jmethodID>(m_openFd), javaPath);

		if (env->ExceptionCheck())
		{
			env->ExceptionClear();
		}
		else if (fd != 0)
		{
			// yay! it worked!
			__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Opened %s with AndroidFDStream", path);
			
			env->DeleteLocalRef(javaPath);
			return new AndroidFDStream(fd);
		}

		// well, looks like we're going to have to use the slow path...
		const int ACCESS_RANDOM = 1;
		jobject inputStream = env->CallObjectMethod(static_cast<jobject>(m_androidAssetManager), static_cast<jmethodID>(m_open), javaPath, ACCESS_RANDOM);

		if (env->ExceptionCheck())
		{
			// doh! we couldn't open the file that way either! We'll just have to return null.
			env->ExceptionClear();
			env->DeleteLocalRef(javaPath);

			return nullptr;
		}

		// it worked!
		__android_log_print(ANDROID_LOG_DEBUG, "NXNA", "Opened %s with AndroidFileStream", path);
		
		env->DeleteLocalRef(javaPath);
		return new AndroidFileStream(inputStream);
	}
}
}