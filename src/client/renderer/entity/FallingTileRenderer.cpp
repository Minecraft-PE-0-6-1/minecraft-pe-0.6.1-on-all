#include "FallingTileRenderer.hpp"
#include "client/renderer/TileRenderer.hpp"
#include "world/entity/item/FallingTile.hpp"
#include "world/level/tile/Tile.hpp"
#include "world/level/Level.hpp"

FallingTileRenderer::FallingTileRenderer() {
    this->shadowRadius = 0.5f;
    tileRenderer = new TileRenderer();
}

FallingTileRenderer::~FallingTileRenderer() {
    delete tileRenderer;
}

void FallingTileRenderer::render( Entity* e, float x, float y, float z, float rot, float a ) {
    FallingTile* tile = (FallingTile*) e;

    glPushMatrix();
    glTranslatef(x, y, z);

    bindTexture("terrain.png");
    Tile* tt = Tile::tiles[tile->tile];

    Level* level = tile->getLevel();

    if (tt != NULL) {
        tileRenderer->renderBlock(tt, level, Mth::floor(tile->x), Mth::floor(tile->y), Mth::floor(tile->z));

    }
    glPopMatrix();
}
