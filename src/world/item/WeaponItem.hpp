#pragma once

//package net.minecraft.world.item;

#include "Item.hpp"
#include "world/entity/Entity.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/player/Player.hpp"
#include "world/level/Level.hpp"
#include "world/level/tile/Tile.hpp"

class WeaponItem: public Item
{
	typedef Item super;
public:
    WeaponItem(int id, const Item::Tier& tier)
	:	super(id),
		tier(tier)
	{
        maxStackSize = 1;
        setMaxDamage(tier.getUses());

        damage = 4 + tier.getAttackDamageBonus();
    }

    /*@Override*/
    float getDestroySpeed(ItemInstance* itemInstance, Tile* tile) {
        //@todo
        if (tile->id == Tile::web->id) {
            // swords can quickly cut web
            return 15;
        }
        return 1.5f;
    }

    void hurtEnemy(ItemInstance* itemInstance, Mob* mob/*, Mob* attacker*/) {
        itemInstance->hurt(1);// attacker);
        //return true;
    }

    bool mineBlock(ItemInstance* itemInstance, int tile, int x, int y, int z/*, Mob* owner*/) {
        itemInstance->hurt(2);//, owner);
        return true;
    }

    int getAttackDamage(Entity* entity) {
        return damage;
    }

    bool isHandEquipped() const {
        return true;
    }

    //UseAnim getUseAnimation(ItemInstance itemInstance) {
    //    return UseAnim.block;
    //}

    int getUseDuration(ItemInstance* itemInstance) {
        return 20 * 60 * 60; // Block for a maximum of one hour!
    }

    ItemInstance* use(ItemInstance* instance, Level* level, Player* player) {
        //player->startUsingItem(instance, getUseDuration(instance)); //@todo
        return instance;
    }

    // /*@Override*/
    bool canDestroySpecial(const Tile* tile) const {
        return tile->id == Tile::web->id;
    }

    ///*@Override*/
    //int getEnchantmentValue() {
    //    return tier.getEnchantmentValue();
    //}
private:
    int damage;
    const Tier& tier;
};

