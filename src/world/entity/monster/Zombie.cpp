#include "Zombie.hpp"

#include "world/item/Item.hpp"
#include "world/level/Level.hpp"
#include "util/Mth.hpp"
//#include "world/entity/MobType.hpp"
#include "world/entity/ai/goal/GoalSelector.hpp"
#include "world/entity/ai/control/JumpControl.hpp"
#include "world/entity/ai/goal/RandomStrollGoal.hpp"
#include "world/entity/ai/goal/MeleeAttackGoal.hpp"
#include "world/entity/ai/goal/target/NearestAttackableTargetGoal.hpp"
#include "world/entity/ai/goal/target/HurtByTargetGoal.hpp"
#include "world/entity/ai/goal/BreakDoorGoal.hpp"

Zombie::Zombie( Level* level )
:	super(level),
	fireCheckTick(0),
	_useNewAi(false)
{
	entityRendererId = ER_ZOMBIE_RENDERER;
	this->textureName = "mob/zombie.png";
	//pathfinderMask |= CAN_OPEN_DOORS;
	//navigation->canOpenDoors = true;

	runSpeed = 0.5f;
	attackDamage = 4;

	targetSelector = new GoalSelector();
	targetSelector->addGoal(1, new HurtByTargetGoal(this, false));
	targetSelector->addGoal(2, new NearestAttackableTargetGoal(this, 1, 16, 0, true));

	goalSelector = new GoalSelector();
	//goalSelector->addGoal(1, new BreakDoorGoal(this));
	goalSelector->addGoal(2, new MeleeAttackGoal(this, runSpeed, false, 0));
	goalSelector->addGoal(7, new RandomStrollGoal(this, runSpeed) );

	moveControl = new MoveControl(this);
	jumpControl = new JumpControl(this);
}

Zombie::~Zombie() {
	delete goalSelector;
	delete targetSelector;

	delete moveControl;
	delete jumpControl;
}

int Zombie::getMaxHealth() {
	return 12; // 16
}

void Zombie::aiStep() {
	if ((++fireCheckTick & 1) && level->isDay() && !level->isClientSide) {
		float br = getBrightness(1);
		if (br > 0.5f) {
			if (level->canSeeSky(Mth::floor(x), Mth::floor(y), Mth::floor(z)) && random.nextFloat() * 3.5f < (br - 0.4f)) {
				hurt(NULL, 1);

				for (int i = 0; i < 5; ++i) {
					float xa = (2.0f * random.nextFloat() - 1.0f) * (2.0f * random.nextFloat() - 1.0f) * 0.02f;
					float ya = (2.0f * random.nextFloat() - 1.0f) * (2.0f * random.nextFloat() - 1.0f) * 0.02f;
					float za = (2.0f * random.nextFloat() - 1.0f) * (2.0f * random.nextFloat() - 1.0f) * 0.02f;
					level->addParticle(PARTICLETYPE(explode), x + random.nextFloat() * bbWidth * 2 - bbWidth, y + random.nextFloat() * bbHeight, z + random.nextFloat() * bbWidth * 2 - bbWidth, xa, ya, za);
				}
				//setOnFire(8); //@todo
			}
		}
	}

	super::aiStep();
}

int Zombie::getEntityTypeId() const {
	return MobTypes::Zombie;
}

void Zombie::setUseNewAi( bool use ) {
	_useNewAi = use;
}

int Zombie::getArmorValue() {
	int armor = super::getArmorValue() + 2;
	if (armor > 20) armor = 20;
	return armor;
}

const char* Zombie::getAmbientSound() {
	return "mob.zombie";
}

std::string Zombie::getHurtSound() {
	return "mob.zombiehurt";
}

std::string Zombie::getDeathSound() {
	return "mob.zombiedeath";
}

int Zombie::getDeathLoot() {
	return 0; //@todo
	//return Item::rotten_flesh->id;
}

bool Zombie::useNewAi() {
	return _useNewAi;
}

void Zombie::die( Entity* source ) {
	super::die(source);
	if(!level->isClientSide) {
		if(random.nextInt(4) == 0) {
			spawnAtLocation(Item::feather->id, random.nextInt(1) + 1);
		}
	}	
}

int Zombie::getAttackDamage( Entity* target ) {
	ItemInstance* weapon = getCarriedItem();
	int damage = attackDamage;
	if(weapon != NULL) damage += weapon->getAttackDamage(this);
	return damage;
}

/*@Override*/ //@todo?
//MobType getMobType() {
//	return MobType::UNDEAD;
//}

