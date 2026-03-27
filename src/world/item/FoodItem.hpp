#pragma once

//package net.minecraft.world.item;

#include "Item.hpp"
#include "world/level/Level.hpp"
#include "world/entity/player/Player.hpp"
#include "SharedConstants.hpp"
#include "network/packet/SetHealthPacket.hpp"
//#include "world/effect/MobEffectInstance.hpp"
//#include "world/food/FoodConstants.hpp"

class FoodItem: public Item
{
	typedef Item super;
    static const int EAT_DURATION = (int) (20 * 1.6);
public:
    FoodItem(int id, int nutrition, bool isMeat, float saturationMod = 0.6f)
    :   super(id),
        nutrition(nutrition),
        _isMeat(isMeat),
        saturationModifier(saturationMod)
    {
	}

	bool isFood() const {
		return true;
	}

	ItemInstance useTimeDepleted(ItemInstance* instance, Level* level, Player* player) {
		instance->count--;
		player->foodData.eat(this);
		level->playSound(player, "random.burp", 0.5f, level->random.nextFloat() * 0.1f + 0.9f);
		return *instance;
	}

    int getUseDuration(ItemInstance* itemInstance) {
        return EAT_DURATION;
    }

    UseAnim::UseAnimation getUseAnimation() {
        return UseAnim::eat;
    }

	ItemInstance* use(ItemInstance* instance, Level* level, Player* player) {
		if (!player->abilities.invulnerable && player->isHurt()) {
			player->startUsingItem(*instance, getUseDuration(instance));
		}
		return instance;
	}

    int getNutrition() {
        return nutrition;
    }

	/*
    float getSaturationModifier() {
        return saturationModifier;
    }
	*/

    bool isMeat() {
        return _isMeat;
    }

	/*
    FoodItem* setEatEffect(int id, int durationInSecods, int amplifier, float effectProbability) {
        effectId = id;
        effectDurationSeconds = durationInSecods;
        effectAmplifier = amplifier;
        this->effectProbability = effectProbability;
        return this;
    }
	*/

    FoodItem* setCanAlwaysEat() {
        canAlwaysEat = true;
        return this;
    }
private:
    const int nutrition;
    const float saturationModifier;
    const bool _isMeat;
    bool canAlwaysEat;
	/*

    int effectId;
    int effectDurationSeconds;
    int effectAmplifier;
    float effectProbability;
	*/
};

