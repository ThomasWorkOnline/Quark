#pragma once

#include <Quark.h>

#include "Chunk.h"

class LoadingChunk : public Chunk
{
public:
	LoadingChunk(const glm::ivec2& coord, World* world)
		: Chunk(coord, world) {}

	enum class Status : int8_t
	{
		Allocated = 0,
		WorldGenerated,
		Loaded
	};

	Status GetStatus() const { return m_Status.load(); }
	void SetStatus(Status status) { m_Status.store(status); }

	operator Chunk* ()
	{
		return static_cast<Chunk*>(this);
	}

private:
	std::atomic<Status> m_Status = Status::Allocated;
};

inline std::ostream& operator<< (std::ostream& os, LoadingChunk::Status status)
{
	switch (status)
	{
	case LoadingChunk::Status::Allocated:
		os << "Allocated";
		break;
	case LoadingChunk::Status::WorldGenerated:
		os << "WorldGenerated";
		break;
	case LoadingChunk::Status::Loaded:
		os << "Loaded";
		break;
	}

	return os;
}
