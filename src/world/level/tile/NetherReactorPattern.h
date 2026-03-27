#pragma once

class NetherReactorPattern {
public:
	NetherReactorPattern();
	unsigned int  getTileAt(int level, int x, int z);
private:
	void setTileAt(int level, int x, int z, int tile);
private:
	unsigned int pattern[3][3][3];
};

