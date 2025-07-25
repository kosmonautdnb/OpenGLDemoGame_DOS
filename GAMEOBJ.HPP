#ifndef __GAMEOBJ_HPP__
#define __GAMEOBJ_HPP__

#include "vector.hpp"
#include "colision.hpp"

#define go_(__v__) dynamic_cast<GO*>(__v__)
#define __RUNNING(__o__) ((go_(__o__)->active)&&(!go_(__o__)->deleteIt))

class GO {
public:
  bool deleteIt;
  unsigned int index;
  double seconds;
  bool active;
  bool debugCountIt;
  bool debugMark;
  GO() {
    deleteIt = false;
    static unsigned int k=0;
    index = k; 
    k++;
    seconds = 0;
    active = false;
    debugCountIt = true;
    debugMark = false;
  }
  virtual ~GO() {
  }
  virtual void destruct() {
    deleteIt = true;
  }
  void mark(bool m) {
    debugMark = m;
  }
  virtual void activated() {
  }
  virtual void deActivated() {
  }
};

class GO_Position {
public:
  Vector position;
  GO_Position() {
  }
  GO_Position(const Vector &position) {
    this->position = position;
  }
  virtual ~GO_Position() {
  }
};

class GO_Rotation {
public:
  Vector axis;
  double angle;
  double angleAddPerSecond;
  GO_Rotation() {
    angle = 0;
    angleAddPerSecond = 0;
  }
  GO_Rotation(double angle, const Vector &axis) {
    this->angle = angle;
    this->axis = axis;
    angleAddPerSecond = 0;
  }
  virtual ~GO_Rotation() {
  }
};

class GO_State {
public:
  int state;
  GO_State() {
    state = 0;
  }
  virtual ~GO_State() {
  }
};

class GO_Callback {
public:
  GO_Callback1() {
  }
  virtual ~GO_Callback() {
  }
  virtual void callback(double dt) = 0;
};

class GO_IdleCallback {
public:
  GO_IdleCallback1() {
  }
  virtual ~GO_IdleCallback() {
  }
  virtual void idleCallback(double dt) = 0;
};

class GO_FrequencyCallback {
public:
  double durationTillNext;
  double durationFull;
  int frequencyStep;
  GO_FrequencyCallback(double duration) {
    durationFull = duration;
    durationTillNext = duration;
    frequencyStep = 0;
  }
  virtual ~GO_FrequencyCallback() {
  }
  virtual void frequent(int iteration) = 0;
};

class GO_FrequencyCallback2 {
public:
  double durationTillNext2;
  double durationFull2;
  int frequencyStep2;
  GO_FrequencyCallback2(double duration) {
    durationFull2 = duration;
    durationTillNext2 = duration;
    frequencyStep2 = 0;
  }
  virtual ~GO_FrequencyCallback2() {
  }
  virtual void frequent2(int iteration) = 0;
};

class GO_Physical {
public:
  Vector velocity;
  Vector force;
  GO_Physical() {
  }
  GO_Physical(const Vector &v) : velocity(v) {
  }
  virtual ~GO_Physical() {
  }
};


class GO_LifeTime {
public:
  double lifeTime;
  double initialLifeTime;
  GO_LifeTime(double seconds=0) {
    lifeTime = seconds;
    initialLifeTime = seconds;
  }
  virtual ~GO_LifeTime() {
  }
  void initLifeTime(double s) {lifeTime = s; initialLifeTime = s;}
};

class GO_AliveDistance {
public:
  Vector aliveLastPosition;
  bool aliveFresh;
  double aliveDistance;
  GO_AliveDistance(double d=0) {
    aliveDistance = d;
    aliveFresh = true;
  }
  virtual ~GO_AliveDistance() {
  }
  void initAliveDistance(double d) {aliveDistance = d; aliveFresh = true;}
};


class GO_Paintable {
public:
  GO_Paintable() {
  }
  virtual ~GO_Paintable() {
  }
  virtual void paint(double dt) = 0;
};

void playerHit(bool explosion);

class GO_Collider_Enemy {
public:
  Vector colliderEnemyPosition;
  Vector lastColliderEnemyPosition;
  double colliderEnemyRadius;
  int colliderEnemyFresh;
  GO_Collider_Enemy(double rad) : colliderEnemyRadius(rad) {
    colliderEnemyFresh = 2;
  }
  virtual ~GO_Collider_Enemy() {
  }
  virtual void collidedWithPlayer() {
    playerHit(true);
  }
  virtual bool collideWithCapsule(int capsuleSlot) {
    if (!__RUNNING(this)) return false;
    if (colliderEnemyFresh>0) return false;
    capsule[CAPSULE_COLLIDER] = Capsule(colliderEnemyPosition, lastColliderEnemyPosition, colliderEnemyRadius);
    return collide(CAPSULE_COLLIDER,capsuleSlot);
  }
};

class GO_Collider_LevelObject {
public:
  Vector colliderLevelObjectPosition;
  Vector lastColliderLevelObjectPosition;
  double colliderLevelObjectRadius;
  int colliderLevelObjectFresh;
  GO_Collider_LevelObject(double rad) : colliderLevelObjectRadius(rad) {
    colliderLevelObjectFresh = 2;
  }
  virtual ~GO_Collider_LevelObject() {
  }
  virtual void collidedWithPlayer() {
    playerHit(true);
  }
  virtual bool collideWithCapsule(int capsuleSlot) {
    if (!__RUNNING(this)) return false;
    if (colliderLevelObjectFresh>0) return false;
    capsule[CAPSULE_COLLIDER] = Capsule(colliderLevelObjectPosition, lastColliderLevelObjectPosition, colliderLevelObjectRadius);
    return collide(CAPSULE_COLLIDER,capsuleSlot);
  }
};

class GO_ScoreHit {
public:
  int scoreHit;
  GO_ScoreHit(int score) : scoreHit(score) {;}
  virtual ~GO_ScoreHit() {;}
};

class GO_ScoreDestructed {
public:
  int scoreDestructed;
  GO_ScoreDestructed(int score) : scoreDestructed(score) {;}
  virtual ~GO_ScoreDestructed() {;}
};

extern int32_t score;
void enemyEnergyBar(double energy);

class GO_HitPoints {
public:
  int hitPoints;
  int initialHitPoints;
  GO_HitPoints(int hitPoints) : hitPoints(hitPoints), initialHitPoints(hitPoints) {;}
  virtual ~GO_HitPoints() {;}
  virtual void hitPoints0() {
    if (dynamic_cast<GO_ScoreDestructed*>(this)!=NULL) {
      score += dynamic_cast<GO_ScoreDestructed*>(this)->scoreDestructed;
    }
    go_(this)->destruct();
  }
  virtual void subtractHitPoints(int count) {
    if (count > hitPoints) count = hitPoints;
    hitPoints-=count;
    if (dynamic_cast<GO_ScoreHit*>(this)!=NULL) {
      score += dynamic_cast<GO_ScoreHit*>(this)->scoreHit*count;
    }
    enemyEnergyBar((double)hitPoints/(initialHitPoints>0?initialHitPoints:1));
    if (hitPoints<=0)
      hitPoints0();
  }
};

#endif // __GAMEOBJ_HPP__
