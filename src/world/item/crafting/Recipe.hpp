#pragma once

//package net.minecraft.world.item.crafting;

//#include "world/inventory/CraftingContainer.hpp"
#include "world/item/ItemInstance.hpp"
#include <map>
#include <vector>

class CraftingContainer;

class ItemPack {
public:
	typedef std::map<int, int> Map;
	//typedef std::vector<std::pair<int, int> > TypeCountList;

	void add(int id, int count = 1);
	int  getCount(int id) const;
	void print() const;

	int getMaxMultipliesOf(ItemPack& v) const;
	std::vector<ItemInstance> getItemInstances() const;

	static int getIdForItemInstance(const ItemInstance* ii);
	static int getIdForItemInstanceAnyAux( const ItemInstance* ii );
	static ItemInstance getItemInstanceForId(int id);
private:
	Map items;
};


class Recipe
{
public:
	static const int SIZE_2X2;// = 0;
	static const int SIZE_3X3;// = 1;
	static const int ANY_AUX_VALUE = -1;

	virtual ~Recipe() {}
    virtual bool matches(CraftingContainer* craftSlots) = 0;
	virtual const ItemPack& getItemPack() { return myItems; }
	virtual int getMaxCraftCount(ItemPack& fromItems) = 0;

    virtual int size() = 0;

    virtual ItemInstance assemble(CraftingContainer* craftSlots) = 0;
    virtual ItemInstance getResultItem() const = 0;

	static bool isAnyAuxValue(const ItemInstance* ii) {
		return isAnyAuxValue(ii->id);
	}

	virtual int getCraftingSize() = 0;
private:
	static bool isAnyAuxValue(int id);
protected:
	ItemPack myItems;
};

