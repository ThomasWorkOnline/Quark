#pragma once

class ChunkRenderer
{
public:
	ChunkRenderer() = delete;
	ChunkRenderer operator= (const ChunkRenderer& other) = delete;

	static void Initialize();
	static void Shutdown();

private:

};
