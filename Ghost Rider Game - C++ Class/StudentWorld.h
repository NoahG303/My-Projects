#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getRacer() const;
    void gotSoul();
    bool ZCHelperFunction(int lane, int zc_y, int type);
    void addHWProj(double x, double y, int dir, double speed);
    void addActor(double x, double y, int type);
    bool checkOverlaps(Actor* a);
private:
    GhostRacer* m_gr;
    std::vector<Actor*> apv;
    int m_soulsToCollect;
    int m_soulsCollected;
    int m_bonusPoints;
};

#endif // STUDENTWORLD_H_