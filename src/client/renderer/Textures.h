#ifndef NET_MINECRAFT_CLIENT_RENDERER__Textures_H__
#define NET_MINECRAFT_CLIENT_RENDERER__Textures_H__

//package net.minecraft.client.renderer;

#include <string>
#include <map>
#include <utility>
#include <IPlatform.h>
#include "gles.h"
#include "TextureData.h"

#define TEXTURES_INVALID_ID -1
#define TEXTURES_MIPMAP 0

class DynamicTexture;
class Options;
class AppPlatform;

typedef GLuint TextureId;
typedef std::map<std::string, TextureId> TextureMap;
typedef std::map<TextureId, TextureData> TextureImageMap;

//@todo: Should probably delete the data buffers with image data
//       after we've created an OpenGL-texture, and rewrite the
//       getTemporaryTextureData() to actually load from file IF
//       it's only read ~once anyway.
class Textures
{
public:
    Textures(Options& options, IPlatform& platform) : m_options(options), m_platform(platform) {}
	~Textures();

	void addDynamicTexture(DynamicTexture* dynamicTexture);

	inline void bind(TextureId id) {
		if (id != TEXTURES_INVALID_ID && lastBoundTexture != id) {
			glBindTexture2(GL_TEXTURE_2D, id);
			lastBoundTexture = id;
			++textureChanges;
		} else if (id == TEXTURES_INVALID_ID){
            LOGI("invalidId!\n");
        }
	}

	TextureId loadTexture(const std::string& resourceName, bool inTextureFolder = true);
	TextureId loadAndBindTexture(const std::string& resourceName);

    TextureId assignTexture(const std::string& resourceName, const TextureData& img);
	const TextureData* getTemporaryTextureData(TextureId id);

	void tick(bool uploadToGraphicsCard);

	void clear();
	void reloadAll();

	inline static bool isTextureIdValid(TextureId t) { return t != TEXTURES_INVALID_ID; }

private:
	int smoothBlend(int c0, int c1);
	int crispBlend(int c0, int c1);

public:
	static int textureChanges;

private:
	TextureMap idMap;
	TextureImageMap loadedImages;

	Options& m_options;
	IPlatform& m_platform;

	bool clamp = false;
	bool blur = false;

	int lastBoundTexture = TEXTURES_INVALID_ID;
	std::vector<DynamicTexture*> dynamicTextures;
};

#endif /*NET_MINECRAFT_CLIENT_RENDERER__Textures_H__*/
