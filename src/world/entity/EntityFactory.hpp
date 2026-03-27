#pragma once

class Level;
class Entity;
class CompoundTag;

class EntityFactory
{
public:
	static Entity* CreateEntity(int typeId, Level* level);
	static Entity* loadEntity(CompoundTag* tag, Level* level);
};

