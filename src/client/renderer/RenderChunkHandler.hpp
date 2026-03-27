#pragma once

#include <vector>
#include "RenderChunk.hpp"

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

