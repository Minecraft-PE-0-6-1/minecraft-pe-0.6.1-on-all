#pragma once

//package net.minecraft.client.renderer;

class RenderChunk;

class RenderList
{
	static const int MAX_NUM_OBJECTS = 1024 * 3;

public:
	RenderList();
	~RenderList();

    void init(float xOff, float yOff, float zOff);

	void add(int list);
	void addR(const RenderChunk& chunk);

	inline void next() { ++listIndex; }

    void render();
	void renderChunks();

    void clear();


	float xOff, yOff, zOff;
	int* lists;
	RenderChunk* rlists;

	int listIndex;
	bool inited;
	bool rendered;

private:
	int bufferLimit;
};

