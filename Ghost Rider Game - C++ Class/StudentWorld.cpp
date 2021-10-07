#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath)
{
    m_gr = nullptr;
    m_soulsToCollect = 0; // so VS doesn't whine about uninitialized vars, even though they always will end up initialized in init()
    m_soulsCollected = 0;
    m_bonusPoints = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp(); // just has to delete actors, nothing else
}

int StudentWorld::init()
{
    // sets variables needed for gameplay
    m_soulsCollected = 0;
    m_soulsToCollect = 2 * getLevel() + 5;
    m_bonusPoints = 5000;

    // creates GR and the BLs shown at the initiation of a level
    m_gr = new GhostRacer(this);

    for (int j = 0; j < VIEW_HEIGHT / SPRITE_HEIGHT; j++) {
        apv.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, 1.0 * ROAD_CENTER - ROAD_WIDTH / 2, 1.0 * j * SPRITE_HEIGHT, this));
        apv.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, 1.0 * ROAD_CENTER + ROAD_WIDTH / 2, 1.0 * j * SPRITE_HEIGHT, this));
    }

    for (int j = 0; j < VIEW_HEIGHT / (4 * SPRITE_HEIGHT); j++) {
        apv.push_back(new BorderLine(IID_WHITE_BORDER_LINE, 1.0 * ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, 1.0 * j * 4 * SPRITE_HEIGHT, this));
        apv.push_back(new BorderLine(IID_WHITE_BORDER_LINE, 1.0 * ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, 1.0 * j * 4 * SPRITE_HEIGHT, this));
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // GR does something
    if (m_gr->getAlive()) {
        m_gr->doSomething();
    }

    // everyone else does something
    for (auto it = apv.begin(); it != apv.end(); it++) {
        if ((*it)->getAlive()) {
            (*it)->doSomething();
        }
        if (!(m_gr->getAlive())) { // oh no! GR died this tick! gotta tell the skeleton!
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_soulsCollected >= m_soulsToCollect) { // yay! GR beat the level this tick! gotta tell the skeleton!
            playSound(SOUND_FINISHED_LEVEL);
            increaseScore(m_bonusPoints);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    // if any actors died this tick, including any that fell off the map, they are to be exterminated
    auto it = apv.begin();
    while (it != apv.end()) {
        if (!((*it)->getAlive())) {
            delete* it;
            it = apv.erase(it);
        }
        else {
            it++;
        }
    }

    // add new actors as desired each tick (in each if statement, you can tell what's being added by the int var name or by what comes after "apv.push_back(new " <classNameHere>
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT * 1.0;
    double lastYVal = 0;
    auto it2 = apv.end();
    it2--; // pointing to last item in apv
    while (it2 != apv.begin()) {
        if (!((*it2)->isRealActor())) { // is a BL
            if (((*it2)->getX()) == (1.0 * ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3) || ((*it2)->getX()) == (1.0 * ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3)) { // is WBL not YBL
                lastYVal = (*it2)->getY();
                break;
            }
        }
        it2--;
    }
    double delta_y = new_border_y - lastYVal;
    if (delta_y >= SPRITE_HEIGHT * 1.0) {
        apv.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, 1.0 * ROAD_CENTER - ROAD_WIDTH / 2, new_border_y, this));
        apv.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, 1.0 * ROAD_CENTER + ROAD_WIDTH / 2, new_border_y, this));
    }
    if (delta_y >= 1.0 * 4 * SPRITE_HEIGHT) {
        apv.push_back(new BorderLine(IID_WHITE_BORDER_LINE, 1.0 * ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, new_border_y, this));
        apv.push_back(new BorderLine(IID_WHITE_BORDER_LINE, 1.0 * ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, new_border_y, this));
    }
    int chanceLostSoul = randInt(0, 99); // [0, 100)
    if (chanceLostSoul == 0) {
        apv.push_back(new LostSoul(randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT, this));
    }
    int chanceHolyWaterRefill = randInt(0, 99 + 10 * getLevel());
    if (chanceHolyWaterRefill == 0) {
        apv.push_back(new HolyWaterRefill(randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT, this));
    }
    int chanceHumanPed = randInt(0, max(200 - getLevel() * 10, 30) - 1);
    if (chanceHumanPed == 0) {
        apv.push_back(new HumanPed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this));
    }
    int chanceZombiePed = randInt(0, max(100 - getLevel() * 10, 20) - 1);
    if (chanceZombiePed == 0) {
        apv.push_back(new ZombiePed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this));
    }
    int chanceVehicle = randInt(0, max(100 - getLevel() * 10, 20) - 1);
    if (chanceVehicle == 0) {
        int cur_lane = randInt(1, 3); // left center right
        int leftEdge, rightEdge, laneNum;
        double yCoord, vertSpeed;
        int chosenLane = -1;
        bool grFound = false;
        double gr_x = getRacer()->getX();
        double gr_y = getRacer()->getY();
        for (int i = cur_lane; i < cur_lane + 3; i++) {
            laneNum = i % 3;
            if (laneNum == 0) {
                laneNum = 3;
            }
            laneNum--;
            double minHeight = VIEW_HEIGHT * 1.0;
            leftEdge = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3 * laneNum;
            rightEdge = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3 * (laneNum + 1);
            if (gr_x >= leftEdge && gr_x < rightEdge && gr_y < minHeight) { // if GR is in this lane, we definitely can't spawn a new cab
                grFound = true;
                minHeight = gr_y;
            }
            auto it = apv.begin();
            auto answer = apv.end();
            while (it != apv.end()) {
                if ((*it)->getCAWA() && (*it)->getX() >= leftEdge && (*it)->getX() < rightEdge && (*it)->getY() < minHeight) {
                    answer = it;
                    minHeight = (*it)->getY();
                }
                it++;
            }
            if (!grFound && (answer == apv.end() || minHeight > VIEW_HEIGHT / 3)) { // if GR is not in this lane, and there's either no other CAWAs in it, or the lowest one on the screen isn't too low, we can add a cab here
                chosenLane = laneNum + 1;
                yCoord = SPRITE_HEIGHT / 2;
                vertSpeed = getRacer()->getVSpeed() + randInt(2, 4) * 1.0;
                break;
            }
            it = apv.begin();
            answer = apv.end();
            double maxHeight = 0.0;
            while (it != apv.end()) {
                if ((*it)->getCAWA() && (*it)->getX() >= leftEdge && (*it)->getX() < rightEdge && (*it)->getY() > maxHeight) {
                    answer = it;
                    maxHeight = (*it)->getY();
                }
                it++;
            }
            if (answer == apv.end() || maxHeight < VIEW_HEIGHT * 2 / 3) { // if there's either no other CAWAs in this lane, or the highest one on the screen isn't too high, we can add a cab here
                chosenLane = laneNum + 1;
                yCoord = VIEW_HEIGHT - SPRITE_HEIGHT / 2 * 1.0;
                vertSpeed = getRacer()->getVSpeed() - randInt(2, 4) * 1.0;
                break;
            }
            // too dangerous, check next lane
        }
        if (chosenLane != -1) { // if we found a valid lane to add a cab, add one
            apv.push_back(new ZombieCab(1.0 * ROAD_CENTER - ROAD_WIDTH / 3 + (ROAD_WIDTH / 3 * (chosenLane - 1)), yCoord, vertSpeed, this));
        }
    }
    int chanceOilSlick = randInt(0, max(150 - getLevel() * 10, 40) - 1);
    if (chanceOilSlick == 0) {
        apv.push_back(new OilSlick(randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT, this));
    }

    // prints out the game status text at the top, with setw() to ensure 2 spaces between each statement
    ostringstream oss;
    oss << "Score: " << getScore() << setw(7) << "Lvl: " << getLevel() << setw(14) << "Souls2Save: " << m_soulsToCollect - m_soulsCollected << setw(9) << "Lives: " << getLives() << setw(10) << "Health: " << m_gr->getHP() << setw(10) << "Sprays: " << m_gr->getSpray() << setw(9) << "Bonus: " << m_bonusPoints;
    setGameStatText(oss.str());

    if (m_bonusPoints > 0) {
        m_bonusPoints--;
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // gotta remove all of our dynamically allocated memory each time the level ends (or the destructor is called, when the game is fully over)
    delete m_gr;
    m_gr = nullptr;

    auto it = apv.begin();
    while (it != apv.end()) {
        delete (*it);
        it = apv.erase(it);
    }
}

GhostRacer* StudentWorld::getRacer() const
{
    return m_gr;
}

void StudentWorld::gotSoul()
{
    m_soulsCollected++;
}

bool StudentWorld::ZCHelperFunction(int lane, int zc_y, int type)
{
    // 0 slowDown 1 speedUp
    int leftSide = 256; // wont slow down if not in a lane and these aren't reset, not that that should happen
    int rightSide = 0;
    if (lane == 1) {
        leftSide = ROAD_CENTER - ROAD_WIDTH / 2;
        rightSide = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
    }
    else if (lane == 2) {
        leftSide = ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3;
        rightSide = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;
    }
    else if (lane == 3) {
        leftSide = ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3;
        rightSide = ROAD_CENTER + ROAD_WIDTH / 2;
    }

    // now that lane boundaries are clear, check if there are any actors meeting the checking conditions, to make a given ZC speed up or slow down during a tick
    auto it = apv.begin();
    double curr_x, curr_y;
    while (it != apv.end()) {
        curr_x = (*it)->getX();
        curr_y = (*it)->getY();
        if ((*it)->getCAWA() && curr_x >= leftSide && curr_x < rightSide) { // there's a CAWA in my lane!
            if (type == 0 && curr_y > zc_y && curr_y <= zc_y + 96.0) {
                return true;
            }
            if (type == 1 && curr_y < zc_y && curr_y >= zc_y - 96.0) {
                return true;
            }
        }
        it++;
    }
    return false; // CAWA was far enough away I don't need to alter my speed to avoid it
}

void StudentWorld::addHWProj(double x, double y, int dir, double speed)
{
    apv.push_back(new HolyWaterProjectile(x, y, dir, speed, 0, this));
}

void StudentWorld::addActor(double x, double y, int type)
{
    if (type == 0) {
        apv.push_back(new HealGoodie(x, y, this));
    }
    if (type == 1) {
        apv.push_back(new OilSlick(x, y, this));
    }
}

bool StudentWorld::checkOverlaps(Actor* a)
{
    double x = a->getX();
    double y = a->getY();
    double radius = a->getRadius();
    auto it = apv.begin();
    double delta_x, delta_y, radius_sum;
    while (it != apv.end()) {
        if ((*it)->isAffectedByHW()) { // this actor would be affected if we are overlapping. lets check
            delta_x = abs(x - (*it)->getX());
            delta_y = abs(y - (*it)->getY());
            radius_sum = radius + (*it)->getRadius();
            if (delta_x < radius_sum * .25 && delta_y < radius_sum * .6) { // HW-affected actor is overlapping with HWP!
                (*it)->getDamaged(1);
                return true;
            }
        }
        it++;
    }
    return false;
}