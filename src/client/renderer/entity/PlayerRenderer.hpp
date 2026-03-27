#pragma once

#include "HumanoidMobRenderer.hpp"

class PlayerRenderer : public HumanoidMobRenderer
{
	typedef HumanoidMobRenderer super;
public:
	PlayerRenderer(HumanoidModel* humanoidModel, float shadow);
	~PlayerRenderer();

	virtual int prepareArmor(Mob* mob, int layer, float a);
	bool isModernPlayerSkin(Mob* mob);
	virtual void render(Entity* mob, float x, float y, float z, float rot, float a);

	virtual void setupPosition(Entity* mob, float x, float y, float z);
	virtual void setupRotations(Entity* mob, float bob, float bodyRot, float a);

	virtual void renderName(Mob* mob, float x, float y, float z);
	virtual void onGraphicsReset();
private:
	HumanoidModel* playerModel32;
	HumanoidModel* playerModel64;
	HumanoidModel* armorParts1;
	HumanoidModel* armorParts2;
};


