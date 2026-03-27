#include "TileEntityRenderer.hpp"
#include "TileEntityRenderDispatcher.hpp"
#include "client/renderer/Textures.hpp"
#include "client/gui/Font.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/entity/TileEntity.hpp"

TileEntityRenderer::TileEntityRenderer()
:   tileEntityRenderDispatcher(NULL)
{
}

void TileEntityRenderer::bindTexture( const std::string& resourceName )
{
	Textures* t = tileEntityRenderDispatcher->textures;
	if (t != NULL) t->loadAndBindTexture(resourceName);
}

Level* TileEntityRenderer::getLevel()
{
	return tileEntityRenderDispatcher->level;
}

void TileEntityRenderer::init( TileEntityRenderDispatcher* tileEntityRenderDispatcher )
{
	this->tileEntityRenderDispatcher = tileEntityRenderDispatcher;
}

Font* TileEntityRenderer::getFont()
{
	return tileEntityRenderDispatcher->getFont();
}
