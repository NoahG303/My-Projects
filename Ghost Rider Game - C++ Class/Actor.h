#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool alive, bool isCAWA, int health, double vert, int hztl, StudentWorld* sw);
	virtual void doSomething() = 0;
	bool getAlive() const;
	void setAlive(bool newAlive);
	bool getCAWA() const;
	int getHP() const;
	void setHP(int hp);
	bool damageMe(int amt);
	virtual void getDamaged(int amt);
	double getVSpeed() const;
	void setVSpeed(double speed);
	int getHSpeed() const;
	void setHSpeed(int speed);
	StudentWorld* getWorld() const;
	virtual bool isRealActor() const;
	bool overlapCheck() const;
	void moveMe();
	bool isOffMap() const;
	virtual bool isAffectedByHW() const;
private:
	bool m_alive;
	bool m_isCAWA;
	int m_HP;
	double m_vspeed;
	int m_hspeed;
	StudentWorld* m_sw;
};

class GhostRacer : public Actor
{
public:
	GhostRacer(StudentWorld* sw);
	virtual void doSomething();
	virtual void getDamaged(int amt);
	int getSpray() const;
	void setSpray(int spray);
	void spin();
private:
	int m_spray;
};

class BorderLine : public Actor
{
public:
	BorderLine(int imageID, double startX, double startY, StudentWorld* sw);
	virtual void doSomething();
	virtual bool isRealActor() const;
private:
};

class Goodie : public Actor
{
public:
	Goodie(int imageID, double startX, double startY, int dir, double size, StudentWorld* sw);
	virtual void doSomething();
	void getConsumed(int scoreToAdd, int soundToPlay);
	virtual void doSpecificGoodieThing(bool overlap) = 0;
};

class LostSoul : public Goodie
{
public:
	LostSoul(double startX, double startY, StudentWorld* sw);
	virtual void doSpecificGoodieThing(bool overlap);
};

class HolyWaterRefill : public Goodie
{
public:
	HolyWaterRefill(double startX, double startY, StudentWorld* sw);
	virtual void doSpecificGoodieThing(bool overlap);
	virtual bool isAffectedByHW() const;
};

class HealGoodie : public Goodie
{
public:
	HealGoodie(double startX, double startY, StudentWorld* sw);
	virtual void doSpecificGoodieThing(bool overlap);
	virtual bool isAffectedByHW() const;
};

class OilSlick : public Goodie
{
public:
	OilSlick(double startX, double startY, StudentWorld* sw);
	virtual void doSpecificGoodieThing(bool overlap);
};

class IntelligentAgent : public Actor
{
public:
	IntelligentAgent(int imageID, double startX, double startY, int dir, double size, int health, int vert, StudentWorld* sw);
	virtual void doSomething();
	virtual void doSpecificIAThing(bool overlap) = 0;
	void potentiallyAddActor(int type);
	int getMPD() const;
	void setMPD(int newMPD);
	virtual bool isAffectedByHW() const;
private:
	int m_mvmtPlanDist;
};

class Pedestrian : public IntelligentAgent
{
public:
	Pedestrian(int imageID, double startX, double startY, double size, StudentWorld* sw);
	void pedestrianAction();
};

class HumanPed : public Pedestrian
{
public:
	HumanPed(double startX, double startY, StudentWorld* sw);
	virtual void doSpecificIAThing(bool overlap);
	virtual void getDamaged(int amt);
};

class ZombiePed : public Pedestrian
{
public:
	ZombiePed(double startX, double startY, StudentWorld* sw);
	virtual void doSpecificIAThing(bool overlap);
	virtual void getDamaged(int amt);
private:
	int m_ticksTillGrunt;
};

class ZombieCab : public IntelligentAgent
{
public:
	ZombieCab(double startX, double startY, int vert, StudentWorld* sw);
	virtual void doSpecificIAThing(bool overlap);
	virtual void getDamaged(int amt);
private:
	bool m_hasDamagedGR;
};

class HolyWaterProjectile : public Actor
{
public:
	HolyWaterProjectile(double startX, double startY, int dir, double vert, int hztl, StudentWorld* sw);
	virtual void doSomething();
private:
	int m_maxTravelDistance;
};

#endif // ACTOR_H_