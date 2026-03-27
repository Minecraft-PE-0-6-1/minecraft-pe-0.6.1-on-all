#pragma once

//package net.minecraft.client.model;

#include "Model.hpp"
#include "geom/ModelPart.hpp"

class SignModel: public Model
{
public:
    SignModel()
	:	cube(0, 0),
		cube2(0, 14)
	{
        cube.addBox(-12, -14, -1, 24, 12, 2, 0);
        cube2.addBox(-1, -2, -1, 2, 14, 2, 0);

		cube.setModel(this);
		cube2.setModel(this);
    }

    void render() {
        cube.render(1 / 16.0f);
        cube2.render(1 / 16.0f);
    }

    ModelPart cube;
    ModelPart cube2;
};

