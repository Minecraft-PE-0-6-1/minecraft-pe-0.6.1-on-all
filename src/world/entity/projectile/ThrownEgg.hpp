#pragma once

//package net.minecraft.world.entity->projectile;

#include "Throwable.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/animal/Chicken.hpp"
#include "world/level/Level.hpp"
#include "world/phys/HitResult.hpp"

class ThrownEgg: public Throwable
{
	typedef Throwable super;
public:
    ThrownEgg(Level* level)
	:	super(level)
	{
		entityRendererId = ER_THROWNEGG_RENDERER;
	}

    ThrownEgg(Level* level, Mob* mob)
	:	super(level, mob)
	{
		entityRendererId = ER_THROWNEGG_RENDERER;
	}

    ThrownEgg(Level* level, float x, float y, float z)
	:	super(level, x, y, z)
	{
		entityRendererId = ER_THROWNEGG_RENDERER;
	}

	virtual int getEntityTypeId() const {
		return EntityTypes::IdThrownEgg;
	}

    /*@Override*/
protected:
	void onHit(const HitResult& res) {
        if (res.type == ENTITY)
            res.entity->hurt(this, 0);

		if (!level->isClientSide && sharedRandom.nextInt(8) == 0) {
            int count = 1;
            if (sharedRandom.nextInt(32) == 0) count = 4;
            for (int i = 0; i < count; i++) {
                Chicken* chicken = new Chicken(level);
                //chicken.setAge(-20 * 60 * 20);
                chicken->moveTo(x, y, z, yRot, 0);
                level->addEntity(chicken);
            }
        }

        for (int i = 0; i < 6; i++)
            level->addParticle(PARTICLETYPE(snowballpoof), x, y, z, 0, 0, 0);

        if (!level->isClientSide)
            remove();
    }
};

