#pragma once

//package net.minecraft.world.level.pathfinder;

#include "Node.hpp"
#include "world/phys/Vec3.hpp"
#include "world/entity/Entity.hpp"

class Path
{
public:
	Path();
	~Path();

	void  copyNodes(Node** nodes, int length);

	void  destroy();

    void  next();

	void  setSize(int size);
	int   getSize() const;

	bool  isEmpty() const;
    bool  isDone() const;

    Node* last() const;
    Node* get(int i) const;

    int   getIndex() const;
    void  setIndex(int index);

    Vec3  currentPos(Entity* e) const;
	Node* currentPos();

    Vec3  getPos(Entity* e, int index) const;

    bool  sameAs(const Path* path) const;

    bool  endsIn(const Vec3& pos) const;
    bool  endsInXZ(const Vec3& pos) const;

	int id;
private:
	Node** nodes;
	int length;
    int index;

	static int p;
};

