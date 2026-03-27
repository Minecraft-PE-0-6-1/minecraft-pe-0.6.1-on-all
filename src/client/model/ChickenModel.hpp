#pragma once

#include "Model.hpp"
#include "geom/ModelPart.hpp"

class Entity;

class ChickenModel: public Model
{
    typedef Model super;
public:
    ModelPart head, hair, body, leg0, leg1, wing0, wing1, beak, redThing;

    ChickenModel();

    /*@Override*/
    void render(Entity* entity, float time, float r, float bob, float yRot, float xRot, float scale);

    void setupAnim(float time, float r, float bob, float yRot, float xRot, float scale);
};

