#pragma once

//package net.minecraft.client.model;

#include "Model.hpp"
#include "geom/ModelPart.hpp"
class ItemInstance;
class HumanoidModel: public Model
{
public:
HumanoidModel(float g = 0, float yOffset = 0, int texW = 64, int texH = 32);

	void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale);

	void render(HumanoidModel* model, float scale);
    void render(Entity* e, float time, float r, float bob, float yRot, float xRot, float scale);
	void renderHorrible(float time, float r, float bob, float yRot, float xRot, float scale);
	void onGraphicsReset();

	ModelPart head, hair, body, arm0, arm1, leg0, leg1;//, ear;
	bool holdingLeftHand;
	bool holdingRightHand;
	bool sneaking;
	bool bowAndArrow;
};

