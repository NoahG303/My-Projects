#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool alive, bool isCAWA, int health, double vert, int hztl, StudentWorld* sw)
	: GraphObject(imageID, startX, startY, dir, size, depth), m_alive(alive), m_isCAWA(isCAWA), m_HP(health), m_vspeed(vert), m_hspeed(hztl), m_sw(sw)
{
}

bool Actor::getAlive() const
{
	return m_alive;
}

void Actor::setAlive(bool newAlive)
{
	m_alive = newAlive;
}

bool Actor::getCAWA() const
{
	return m_isCAWA;
}

int Actor::getHP() const
{
	return m_HP;
}

void Actor::setHP(int hp)
{
	m_HP = hp;
}

bool Actor::damageMe(int amt)
{
	setHP(getHP() - amt);
	if (getHP() <= 0) {
		setAlive(false);
		return true;
	}
	return false;
}

void Actor::getDamaged(int amt)
{
	damageMe(amt);
}

double Actor::getVSpeed() const
{
	return m_vspeed;
}

void Actor::setVSpeed(double speed)
{
	m_vspeed = speed;
}

int Actor::getHSpeed() const
{
	return m_hspeed;
}

void Actor::setHSpeed(int speed)
{
	m_hspeed = speed;
}

StudentWorld* Actor::getWorld() const
{
	return m_sw;
}

bool Actor::isRealActor() const
{
	return true;
}

bool Actor::overlapCheck() const
{
	double delta_x = abs(getX() - getWorld()->getRacer()->getX());
	double delta_y = abs(getY() - getWorld()->getRacer()->getY());
	double radius_sum = getRadius() + getWorld()->getRacer()->getRadius();
	if (delta_x < radius_sum * .25 && delta_y < radius_sum * .6) { // if 2 Actors are close enough, they're overlapping
		return true;
	}
	return false;
}

void Actor::moveMe()
{
	moveTo(getX() + getHSpeed(), getY() + getVSpeed() - getWorld()->getRacer()->getVSpeed()); // mvmt algorithm given in spec
}

bool Actor::isOffMap() const
{
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		return true;
	}
	return false;
}

bool Actor::isAffectedByHW() const
{
	return false;
}

GhostRacer::GhostRacer(StudentWorld* sw)
	: Actor(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, true, true, 100, 0, 0, sw), m_spray(10)
{
}

void GhostRacer::doSomething()
{
	if (getHP() <= 0) {
		return;
	}

	if (getX() <= (1.0 * ROAD_CENTER - ROAD_WIDTH / 2 )) { // if X-coord overlaps with left BL
		if (getDirection() > 90) {
			getDamaged(10);
			if (getHP() <= 0) {
				setAlive(false);
				return;
			}
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() >= (1.0 * ROAD_CENTER + ROAD_WIDTH / 2)) { // if X-coord overlaps with right BL
		if (getDirection() < 90) {
			getDamaged(10);
			if (getHP() <= 0) {
				setAlive(false);
				return;
			}
		}
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else {
		int ch;
		if (getWorld()->getKey(ch)) { // get user input
			switch (ch) {
			case KEY_PRESS_LEFT:
				if (getDirection() < 114) {
					setDirection(getDirection() + 8);
				}
				break;
			case KEY_PRESS_RIGHT:
				if (getDirection() > 66) {
					setDirection(getDirection() - 8);
				}
				break;
			case KEY_PRESS_UP:
				if (getVSpeed() < 5) {
					setVSpeed(getVSpeed() + 1);
				}
				break;
			case KEY_PRESS_DOWN:
				if (getVSpeed() > -1) {
					setVSpeed(getVSpeed() - 1);
				}
				break;
			case KEY_PRESS_SPACE:
				if (m_spray >= 1) { // creates a HWP object, with same direction and speed as the GR & SPRITE_HEIGHT units in front
					double delta_x = getX() + 1.0 * SPRITE_HEIGHT * cos(getDirection() * 3.1415926536 / 180);
					double delta_y = getY() + 1.0 * SPRITE_HEIGHT * sin(getDirection() * 3.1415926536 / 180);;
					getWorld()->addHWProj(delta_x, delta_y, getDirection(), getVSpeed());
					getWorld()->playSound(SOUND_PLAYER_SPRAY);
					m_spray--;
				}
				break;
			default:
				break;
			}
		}
	}

	double max_shift_per_tick = 4.0;
	int dir = getDirection();
	double direction = dir * 3.1415926536 / 180; // cos() requires RADIANS
	double delta_x = cos(direction) * max_shift_per_tick;
	moveTo(getX() + delta_x, getY()); // move according to the user update change
}

void GhostRacer::getDamaged(int amt)
{
	if (damageMe(amt)) {
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
}

int GhostRacer::getSpray() const
{
	return m_spray;
}

void GhostRacer::setSpray(int spray)
{
	m_spray = spray;
}

void GhostRacer::spin()
{
	// randomly spins the player +- 5 to 20 degrees, but doesn't exceed the max & min direction
	int sign = randInt(0, 1);
	int amt = randInt(5, 20);
	if (sign == 0) {
		setDirection(getDirection() + amt);
	}
	else {
		setDirection(getDirection() - amt);
	}
	if (getDirection() < 60) {
		setDirection(60);
	}
	if (getDirection() > 120) {
		setDirection(120);
	}
}

BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* sw)
	: Actor(imageID, startX, startY, 0, 2.0, 2, true, false, 0, -4, 0, sw)
{
}

void BorderLine::doSomething()
{
	moveMe();
	if (isOffMap()) {
		setAlive(false);
		return;
	}
}

bool BorderLine::isRealActor() const
{
	return false;
}

Goodie::Goodie(int imageID, double startX, double startY, int dir, double size, StudentWorld* sw)
	: Actor(imageID, startX, startY, dir, size, 2, true, false, 0, -4, 0, sw)
{
}

void Goodie::doSomething()
{
	// does generic goodie things, and then calls specific goodie functions for them to do their unique things after
	moveMe();
	if (isOffMap()) {
		setAlive(false);
		return;
	}
	if (overlapCheck()) {
		doSpecificGoodieThing(true);
	}
	else {
		doSpecificGoodieThing(false);
	}
}

void Goodie::getConsumed(int scoreToAdd, int soundToPlay = SOUND_GOT_GOODIE)
{
	// if a goodie overlaps with GR, it will disappear, increase the player's score, and play a sound (default is SGG)
	setAlive(false);
	getWorld()->increaseScore(scoreToAdd);
	getWorld()->playSound(soundToPlay);
}

LostSoul::LostSoul(double startX, double startY, StudentWorld* sw)
	: Goodie(IID_SOUL_GOODIE, startX, startY, 0, 4.0, sw)
{
}

void LostSoul::doSpecificGoodieThing(bool overlap)
{
	if (overlap) {
		getWorld()->gotSoul();
		getConsumed(100, SOUND_GOT_SOUL);
	}
	setDirection(getDirection() - 10);
}

HolyWaterRefill::HolyWaterRefill(double startX, double startY, StudentWorld* sw)
	: Goodie(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0, sw)
{
}

void HolyWaterRefill::doSpecificGoodieThing(bool overlap)
{
	if (overlap) {
		GhostRacer* gr = getWorld()->getRacer();
		gr->setSpray(gr->getSpray() + 10);
		getConsumed(50);
	}
}

bool HolyWaterRefill::isAffectedByHW() const
{
	return true;
}

HealGoodie::HealGoodie(double startX, double startY, StudentWorld* sw)
	: Goodie(IID_HEAL_GOODIE, startX, startY, 0, 1.0, sw)
{
}

void HealGoodie::doSpecificGoodieThing(bool overlap)
{
	if (overlap) {
		GhostRacer* gr = getWorld()->getRacer();
		gr->setHP(gr->getHP() + 10);
		if (gr->getHP() > 100) {
			gr->setHP(100);
		}
		getConsumed(250);
	}
}

bool HealGoodie::isAffectedByHW() const
{
	return true;
}

OilSlick::OilSlick(double startX, double startY, StudentWorld* sw)
	: Goodie(IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), sw)
{
}

void OilSlick::doSpecificGoodieThing(bool overlap)
{
	if (overlap) {
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getRacer()->spin();
	}
}

IntelligentAgent::IntelligentAgent(int imageID, double startX, double startY, int dir, double size, int health, int vert, StudentWorld* sw)
	: Actor(imageID, startX, startY, dir, size, 0, true, true, health, vert, 0, sw), m_mvmtPlanDist(0)
{
}

void IntelligentAgent::doSomething()
{
	// does generic IA things, and then calls specific IA functions for them to do their unique things after
	if (!(getAlive())) {
		return;
	}
	if (overlapCheck()) {
		doSpecificIAThing(true);
	}
	else {
		doSpecificIAThing(false);
	}
}

void IntelligentAgent::potentiallyAddActor(int type)
{
	// if a zombie dies, it has a chance to spawn a new actor where it died (Oil Slick for Cabs, Heal Goodie for Peds)
	int chance = randInt(1, 5);
	if (chance == 1) {
		getWorld()->addActor(getX(), getY(), type);
	}
}

int IntelligentAgent::getMPD() const
{
	return m_mvmtPlanDist;
}

void IntelligentAgent::setMPD(int newMPD)
{
	m_mvmtPlanDist = newMPD;
}

bool IntelligentAgent::isAffectedByHW() const
{
	return true;
}

Pedestrian::Pedestrian(int imageID, double startX, double startY, double size, StudentWorld* sw)
	: IntelligentAgent(imageID, startX, startY, 0, size, 2, -4, sw)
{
}

void Pedestrian::pedestrianAction()
{
	// does generic pedestrian things, once they've done their individual stuff
	moveMe();
	if (isOffMap()) {
		setAlive(false);
		return;
	}
	setMPD(getMPD() - 1);
	if (getMPD() > 0) {
		return;
	}
	else { // if MPD <= 0, time for a new one! random MPD and HSpeed, with appropriate direction faced
		int decision = randInt(0, 1);
		if (decision) { // 1 is true, 0 is false
			setHSpeed(randInt(1, 3)); // either 1 2 or 3
		}
		else {
			setHSpeed(randInt(-3, -1)); // either -3 -2 or -1
		}
		setMPD(randInt(4, 32));
		if (getHSpeed() < 0) {
			setDirection(180);
		}
		else if (getHSpeed() > 0) {
			setDirection(0);
		}
	}
}

HumanPed::HumanPed(double startX, double startY, StudentWorld* sw)
	: Pedestrian(IID_HUMAN_PED, startX, startY, 2.0, sw)
{
}

void HumanPed::doSpecificIAThing(bool overlap)
{
	if (overlap) {
		getWorld()->getRacer()->setAlive(false);
		return;
	}
	pedestrianAction();
}

void HumanPed::getDamaged(int amt)
{
	// ignore HP damage - instead, run away!
	setHSpeed(getHSpeed() * -1);
	if (getDirection() == 180) {
		setDirection(0);
	}
	else if (getDirection() == 0) {
		setDirection(180);
	}
	getWorld()->playSound(SOUND_PED_HURT);
}

ZombiePed::ZombiePed(double startX, double startY, StudentWorld* sw)
	: Pedestrian(IID_ZOMBIE_PED, startX, startY, 3.0, sw), m_ticksTillGrunt(0)
{
}

void ZombiePed::doSpecificIAThing(bool overlap)
{
	if (overlap) {
		getWorld()->getRacer()->getDamaged(5);
		getDamaged(2);
		return;
	}
	// if close to the GR and in front, try to get in its way
	if (abs(getX() - getWorld()->getRacer()->getX()) <= 30 && getY() > getWorld()->getRacer()->getY()) {
		setDirection(270);
		if (getX() < getWorld()->getRacer()->getX()) {
			setHSpeed(1);
		}
		else if (getX() > getWorld()->getRacer()->getX()) {
			setHSpeed(-1);
		}
		else {
			setHSpeed(0);
		}
		m_ticksTillGrunt--;
		if (m_ticksTillGrunt <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			m_ticksTillGrunt = 20;
		}
	}
	pedestrianAction();
}

void ZombiePed::getDamaged(int amt)
{
	if (damageMe(amt)) {
		getWorld()->playSound(SOUND_PED_DIE);
		if (!overlapCheck()) {
			potentiallyAddActor(0); // HealGoodie
		}
		getWorld()->increaseScore(150);
	}
	else {
		getWorld()->playSound(SOUND_PED_HURT);
	}
}

ZombieCab::ZombieCab(double startX, double startY, int vert, StudentWorld* sw)
	: IntelligentAgent(IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 3, vert, sw), m_hasDamagedGR(false)
{
}

void ZombieCab::doSpecificIAThing(bool overlap)
{
	// if a ZC crashes into the GR, and hasn't yet damaged it, it'll play crash sound, damage it, and then fall away, off to the side
	if (overlap) {
		if (!m_hasDamagedGR) {
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getRacer()->getDamaged(20);
			if (getX() <= getWorld()->getRacer()->getX()) { // to the left
				setHSpeed(-5);
				setDirection(120 + randInt(0,19));
			}
			else { // to the right
				setHSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			m_hasDamagedGR = true;
		}
	}
	moveMe();
	if (isOffMap()) {
		setAlive(false);
		return;
	}
	int lane = 0;
	if (getX() >= 1.0 * ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3) {
		lane = 3;
	}
	else if (getX() >= 1.0 * ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3) {
		lane = 2;
	}
	else {
		lane = 1;
	}
	// if moving faster than the GR... (see SW.cpp for ZCHF implementation)
	if (getVSpeed() > getWorld()->getRacer()->getVSpeed()) {
		if (getWorld()->ZCHelperFunction(lane, getY(), 0)) {
			setVSpeed(getVSpeed() - 0.5);
			return;
		}
	}
	// if moving slower than or at the same speed as the GR... (see SW.cpp for ZCHF implementation)
	else {
		if (getWorld()->ZCHelperFunction(lane, getY(), 1)) {
			setVSpeed(getVSpeed() + 0.5);
			return;
		}
	}
	setMPD(getMPD() - 1);
	if (getMPD() > 0) {
		return;
	}
	else { // if MPD <= 0, time for a new one! random MPD, and random altering of VSpeed
		setMPD(randInt(4, 32));
		setVSpeed(getVSpeed() + randInt(-2, 2));
	}
}

void ZombieCab::getDamaged(int amt)
{
	if (damageMe(amt)) {
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		potentiallyAddActor(1); // OilSlick
		getWorld()->increaseScore(200);
		return;
	}
	else {
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}
}

HolyWaterProjectile::HolyWaterProjectile(double startX, double startY, int dir, double vert, int hztl, StudentWorld* sw)
	: Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1, true, false, 0, vert, hztl, sw), m_maxTravelDistance(160)
{
}

void HolyWaterProjectile::doSomething()
{
	if (!(getAlive())) {
		return;
	}
	if (getWorld()->checkOverlaps(this)) {
		setAlive(false);
		return;
	}
	else { // move in current direction, SPRITE_HEIGHT pixels, and track the movement amount
		moveForward(SPRITE_HEIGHT);
		m_maxTravelDistance -= SPRITE_HEIGHT;
	}
	if (isOffMap() || m_maxTravelDistance <= 0) { // if off the map, or has traveled too far, it disappears/dissipates
		setAlive(false);
		return;
	}
}