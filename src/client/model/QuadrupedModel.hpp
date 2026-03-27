#pragma once

//package net.minecraft.client.model;

#include "Model.hpp"
#include "geom/ModelPart.hpp"

class Entity;

class QuadrupedModel: public Model
{
public:
    QuadrupedModel(int legSize, float g);

    //@Override
	void render(QuadrupedModel* model, float scale);
    void render(Entity* entity, float time, float r, float bob, float yRot, float xRot, float scale);

	void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale);

	ModelPart head, hair, body, leg0, leg1, leg2, leg3;
protected:
	float yHeadOffs;
	float zHeadOffs;
};

