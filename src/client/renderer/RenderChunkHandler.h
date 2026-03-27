#pragma once

#include <vector>
#include "RenderChunk.h"

typedef std::vector<RenderChunk> ChunkList;

class RenderChunkHandler
{
public:
    int vboCount;
    ChunkList chunks;

    RenderChunkHandler() {
        vboCount = Tesselator::getVboCount();
    }

    void render() {
    }
};

