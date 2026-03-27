#pragma once

//package net.minecraft.client.model;

#include "ZombieModel.hpp"
#include "geom/ModelPart.hpp"

class SkeletonModel: public ZombieModel
{
    typedef ZombieModel super;
public:
    SkeletonModel()
	{
        float g = 0;

        arm0 = ModelPart(24 + 16, 16);
		arm0.setModel(this);
        arm0.addBox(-1, -2, -1, 2, 12, 2, g); // Arm0
        arm0.setPos(-5, 2, 0);

        arm1 = ModelPart(24 + 16, 16);
		arm1.setModel(this);
        arm1.mirror = true;
        arm1.addBox(-1, -2, -1, 2, 12, 2, g); // Arm1
        arm1.setPos(5, 2, 0);

        leg0 = ModelPart(0, 16);
		leg0.setModel(this);
        leg0.addBox(-1, 0, -1, 2, 12, 2, g); // Leg0
        leg0.setPos(-2, 12, 0);

        leg1 = ModelPart(0, 16);
		leg1.setModel(this);
        leg1.mirror = true;
        leg1.addBox(-1, 0, -1, 2, 12, 2, g); // Leg1
        leg1.setPos(2, 12, 0);
    }

    void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale) {
        bowAndArrow = true;
        super::setupAnim(time, r, bob, yRot, xRot, scale);
    }
};

