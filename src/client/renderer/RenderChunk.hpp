#pragma once

//package net.minecraft.client.renderer;

#include "gles.hpp"
#include "world/phys/Vec3.hpp"

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

