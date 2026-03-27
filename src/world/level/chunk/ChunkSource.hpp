#pragma once

//package net.minecraft.world.level.chunk;

#include <string>
#include "world/level/biome/Biome.hpp"

class ProgressListener;
class LevelChunk;

class ChunkSource
{
public:
	bool isChunkCache;

	ChunkSource()
	:	isChunkCache(false)
	{}

	virtual ~ChunkSource(){}

	virtual bool hasChunk(int x, int y) = 0;

    virtual LevelChunk* getChunk(int x, int z) = 0;

    virtual LevelChunk* create(int x, int z) = 0;

    virtual void postProcess(ChunkSource* parent, int x, int z) = 0;

    //virtual bool save(bool force, ProgressListener* progressListener) = 0;

    virtual bool tick() = 0;

    virtual bool shouldSave() = 0;
	virtual void saveAll(bool onlyUnsaved) {}

	virtual Biome::MobList getMobsAt(const MobCategory& mobCategory, int x, int y, int z) = 0;

    /**
     * Returns some stats that are rendered when the user holds F3.
     */
    virtual std::string gatherStats() = 0;
};

