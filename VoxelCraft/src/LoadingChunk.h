#pragma once

#include <Quark.h>

#include "Chunk.h"

class LoadingChunk : public Chunk
{
public:
	LoadingChunk(const glm::ivec2& coord, World* world)
		: Chunk(coord, world), m_Status(Status::Empty) {}

	enum class Status : int8_t
	{
		Empty = 0,
		Allocated,
		WorldGenerated,
		Loaded
	};

	Status GetStatus() const { return m_Status.load(); }
	void SetStatus(Status status) { m_Status.store(status); }

	operator Chunk* ()
	{
		return (Chunk*)this;
	}

private:
	std::atomic<Status> m_Status;
};

inline std::ostream& operator<< (std::ostream& os, LoadingChunk::Status status)
{
	switch (status)
	{
	case LoadingChunk::Status::Empty:
		os << "Empty";
		break;
	case LoadingChunk::Status::Allocated:
		os << "Allocated";
		break;
	case LoadingChunk::Status::WorldGenerated:
		os << "WorldGenerated";
		break;
	case LoadingChunk::Status::Loaded:
		os << "Loaded";
		break;
	default:
		os << "Undefined";
	}

	return os;
}
