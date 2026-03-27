#include "EntityFactory.hpp"
#include "MobFactory.hpp"
#include "nbt/CompoundTag.hpp"

#include "item/PrimedTnt.hpp"
#include "projectile/Arrow.hpp"
#include "projectile/ThrownEgg.hpp"
#include "projectile/Snowball.hpp"
#include "Painting.hpp"
#include "item/FallingTile.hpp"

Entity* EntityFactory::CreateEntity( int typeId, Level* level )
{
	switch (typeId) {
		case EntityTypes::IdItemEntity: return new ItemEntity(level);
        case EntityTypes::IdFallingTile:return new FallingTile(level);
		case EntityTypes::IdPrimedTnt:  return new PrimedTnt(level);
		case EntityTypes::IdArrow:      return new Arrow(level);
        case EntityTypes::IdThrownEgg:  return new ThrownEgg(level);
        case EntityTypes::IdSnowball:   return new Snowball(level);
		case EntityTypes::IdPainting:   return new Painting(level);
	}
	return NULL;
}

Entity* EntityFactory::loadEntity( CompoundTag* tag, Level* level )
{
	if (!tag) return NULL;
	if (!tag->contains("id")) return NULL;
	int id = tag->getInt("id");

	Entity* e = NULL;

	if (id < 0) {
		LOGE("Negative ItemId: %d at MobFactory::loadEntity\n", id);
	} else if (id < 64) {
		e = MobFactory::CreateMob(id, level);
	} else {
		e = CreateEntity(id, level);
	}

	if (e) {
		e->load(tag);

		// Add "fixes" here :p
		if (e->isItemEntity()) {
			const ItemInstance& item = ((ItemEntity*)e)->item;
			// Remove items out of range, and now invalid
			if(item.isNull() || item.id < 0 || item.id >= Item::MAX_ITEMS || !Item::items[item.id]) {
				delete e;
				e = NULL;
			}
		}
	}

	return e;
}
