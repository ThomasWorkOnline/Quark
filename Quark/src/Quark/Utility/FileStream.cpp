#include "qkpch.h"
#include "FileStream.h"

namespace Quark {

	FileStream::FileStream(std::string_view filepath)
		: m_Handle(std::fopen(filepath.data(), "rb"))
	{
		QK_RUNTIME_VERIFY(m_Handle, "Could not open file at: {0}", filepath);
	}

	FileStream::~FileStream()
	{
		std::fclose(m_Handle);
	}

	size_t FileStream::GetFileSize() const
	{
		std::fseek(m_Handle, 0, SEEK_END);

		size_t size = std::ftell(m_Handle);
		QK_RUNTIME_VERIFY(size > 0 && size != LONG_MAX, "Invalid file size");

		std::fseek(m_Handle, 0, SEEK_SET);
		return size;
	}
}
