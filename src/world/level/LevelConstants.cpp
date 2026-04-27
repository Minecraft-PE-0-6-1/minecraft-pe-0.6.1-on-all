#include "LevelConstants.h"

int LevelConstants::LEVEL_HEIGHT = 128;
int LevelConstants::CHUNK_WIDTH = 16; // in blocks
int LevelConstants::CHUNK_DEPTH = 16;
int LevelConstants::CHUNK_COLUMNS = LevelConstants::CHUNK_WIDTH * LevelConstants::CHUNK_DEPTH;
int LevelConstants::CHUNK_BLOCK_COUNT = LevelConstants::CHUNK_COLUMNS * LevelConstants::LEVEL_HEIGHT;