#pragma once

//package net.minecraft.client.renderer;

#include "gles.h"
#include "../../world/phys/Vec3.h"

class RenderChunk
{
public:
	RenderChunk();
	RenderChunk(GLuint vboId_, int vertexCount_);

	GLuint vboId;
	GLsizei vertexCount;
	int id;
	Vec3 pos;

private:
	static int runningId;
};

