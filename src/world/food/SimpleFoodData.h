#pragma once

//package net.minecraft.world.food;

class FoodItem;
class Player;

class SimpleFoodData
{
public:
    SimpleFoodData();

    void eat(int food);
    void eat(FoodItem* item);

    void tick(Player* player);
private:
	int foodLevel;
};

