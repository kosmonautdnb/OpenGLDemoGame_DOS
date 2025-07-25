GO *endBoss1 = NULL;

// a round thing which rotates and changes shottype by that
class Enemy1 : public GO, public GO_Position, public GO_FrequencyCallback, public GO_FrequencyCallback2, public GO_Collider_Enemy, public GO_Paintable, public GO_HitPoints, public GO_ScoreHit, public GO_ScoreDestructed {
public:
  Enemy1(const Vector &p) : GO(), GO_Position(p), GO_FrequencyCallback(0.2), GO_FrequencyCallback2(0.15), GO_Collider_Enemy(10), GO_Paintable(), GO_HitPoints(64), GO_ScoreHit(1), GO_ScoreDestructed(200) {
  }
  virtual void frequent(int iteration) {
    double k2 = fmod(seconds,9);
    int step = (int)floor(seconds/9);
    if (k2 < 1)
      return;

    int k = iteration / 6 + 1;
    if (k > 5) k = 5;
    for (int j = 0; j < k; j++) {
      double t = (seconds*((step&1)?90:-90)+j*360/5)*PI/180;
      double sp = 20;
      placeEmitExplosion(position+Vector(cos(t)*10,sin(t)*10,0));
      GO *enemyShot = go_(new EnemyShot(position,Vector(cos(t)*sp,sin(t)*sp,0)));
      dynamic_cast<EnemyShot*>(enemyShot)->red=step&1?false:true;
      gameObjects.push_back(enemyShot);
    }
    enemyShotSound->play(position);
  }
  virtual void frequent2(int iteration) {
    double k2 = fmod(seconds,9);
    int step = (int)floor(seconds/9);
    if (k2 >= 1)
      return;
    for (int i = 0; i < 10; i++) {
      double a = i * 2 * PI / 10.0;
      placeEmitExplosion(position+Vector(sin(a)*9,cos(a)*9,0));
    }
  }
  double lk2;
  virtual void paint(double dt) {
    glPushMatrix();
    glTranslatef(position.x,position.y,position.z);
    float k = sin(seconds*3)*0.1+0.9;
    double k2 = fmod(seconds,9);
    int step = (int)floor(seconds/9);
    glRotatef(seconds*((step&1)?90:-90),0,0,1);
    glScalef(k,k,k);
    if (k2 < 1) {
      glRotatef(k2*360,0,1,0);
      if (lk2 >= 1) slideSound->play(position);
      if (seconds > 5)
        position.x += dt*20;
    }
    lk2 = k2;
    markDebug = debugMark;
    drawMesh(enemy1);
    markDebug = false;
    glPopMatrix();
  }

  virtual void destruct() {
    placeExplosion(position);
    GO::destruct();
  }
};

// a space ship which comes from upwards, shots ands flies back upwards again
class Enemy2 : public GO, public GO_Position, public GO_FrequencyCallback, public GO_FrequencyCallback2, public GO_Collider_Enemy, public GO_Paintable, public GO_HitPoints, public GO_ScoreHit, public GO_ScoreDestructed {
public:
  bool red;
  bool _circularShots;
  int type;
  Enemy2(const Vector &p, bool red, int type) : GO(), GO_Position(p), GO_FrequencyCallback(0.35), GO_FrequencyCallback2(0.025), GO_Collider_Enemy(10), GO_Paintable(), GO_HitPoints(64), GO_ScoreHit(1), GO_ScoreDestructed(200) {
    this->red = red;
    this->type = type;
    _circularShots = false;
  }
  virtual void frequent(int iteration) {
    double k = (seconds/2.0)-1;
    k = 1.0-pow(fabs(k),2.0);
    if (k<0) return;
    for (int i = 0; i <= 10; i++) {
      double t = ((i-5)*(_circularShots?90:25)/5+90-atan2(playerPos.x-position.x,playerPos.y-position.y)*180/PI)*PI/180.f;
      double t2 = ((i-5)*(_circularShots?90:25)/5+90)*PI/180.f;
      double sp = 10+(5-fabs(i-5))*4;
      placeEmitExplosion(position+Vector(cos(t2)*sp*0.5,sin(t2)*sp*0.5,0));
      GO *enemyShot = go_(new EnemyShot(position,Vector(cos(t)*sp,sin(t)*sp,0)));
      dynamic_cast<EnemyShot*>(enemyShot)->red=red;
      gameObjects.push_back(enemyShot);
    }
    enemyShotSound->play(position);
  }
  virtual void frequent2(int iteration) {
    double k = (seconds/2.0)-1;
    k = 1.0-pow(fabs(k),2.0);
    if (k>0) return;
    placeTailExplosion(position+Vector(randomLike(iteration*33)*8-4,randomLike(iteration*77)*8-4,0));
  }
  double lk;
  virtual void paint(double dt) {
    glPushMatrix();
    glTranslatef(position.x,position.y,position.z);
    double k = (seconds/2.0)-1;
    k = 1.0-pow(fabs(k),2.0);
    position.y += k;
    position.x -= position.x * dt * 0.1;
    glRotatef(180.f,0,1,0);
    glRotatef(90.f-(k>-1?k:-1)*90.f+180,1,0,0);
    if (k<0) glRotatef(k*180,0,1,0);
    if (k<0&&lk>=0) exploSound->play(position);
    if (type == 1) {
      reColor[0xffffffff] = 0xff000000;
    }
    if (type == 2) {
      reColor[0xffffffff] = 0xff000000;
      reColor[0xff0000ff] = 0xffffffff;
    }
    markDebug = debugMark;
    drawMesh(enemy2);
    markDebug = false;
    reColor.clear();
    glPopMatrix();
    lk = k;
  }
  Enemy2 *circularShots() {
    _circularShots = true;
    return this;
  }
  virtual void destruct() {
    placeExplosion(position);
    GO::destruct();
  }
};

// a ball that shoots many shots in all directions
class Enemy3 : public GO, public GO_Position, public GO_FrequencyCallback, public GO_Collider_Enemy, public GO_Paintable, public GO_HitPoints, public GO_ScoreHit, public GO_ScoreDestructed {
public:
  double shotSpeed;
  int sectors;
  int type;
  Enemy3(const Vector &p) : GO(), GO_Position(p), GO_FrequencyCallback(0.35), GO_Collider_Enemy(10), GO_Paintable(), GO_HitPoints(64), GO_ScoreHit(1), GO_ScoreDestructed(200) {
    static unsigned int k = 0; k++;
    type = k & 1;
    shotSpeed = 10;
    sectors = 20;
  }
  virtual void frequent(int iteration) {
    float speed = 10.0;
    for (int i = 0; i < sectors; i++) {
      float a = i * 2 * PI / sectors + iteration;
      Vector dir(sin(a)*shotSpeed,cos(a)*shotSpeed,0);
      placeEmitExplosion(position+Vector(cos(a)*10,sin(a)*10,0));
      GO *enemyShot = go_(new EnemyShot(position,dir));
      if (((iteration % 12)==0)&&(i&1))
        dynamic_cast<EnemyShot*>(enemyShot)->red=true;
      gameObjects.push_back(enemyShot);
    }
    enemyShotSound->play(position);
  }
  virtual void paint(double dt) {
    glPushMatrix();
    glTranslatef(position.x,position.y,position.z);
    glRotatef(seconds*90,sin(seconds*3),cos(seconds*3),0);
    if (type == 1) {
      reColor[0xffffffff] = 0xff000000;
      reColor[0xff000000] = 0xff0000ff;
    }
    markDebug = debugMark;
    drawMesh(enemy3);
    markDebug = false;
    reColor.clear();
    glPopMatrix();
  }
  virtual void destruct() {
    placeExplosion(position);
    GO::destruct();
  }
};


// a mine
class Enemy4 : public GO, public GO_Position, public GO_FrequencyCallback, public GO_Collider_Enemy, public GO_Paintable, public GO_HitPoints, public GO_ScoreHit, public GO_ScoreDestructed {
public:
  bool red;                       
  int a;
  Enemy4(const Vector &p) : GO(), GO_Position(p), GO_FrequencyCallback(0.5), GO_Collider_Enemy(10), GO_Paintable(), GO_HitPoints(128), GO_ScoreHit(1), GO_ScoreDestructed(200) {
    static unsigned int k = 0; k++;
    red= k & 1;
    a = 0;
  }
  virtual void frequent(int iteration) {
    if (length(playerPos-position)<40.0) {
      a++;if ((a%10)<5+1) {
        float speed = 10.0;
        int sectors = 30;
        float shotSpeed = 10;
        for (int i = 0; i < sectors; i++) {
          float a = i * 2 * PI / sectors;
          Vector dir(sin(a)*shotSpeed,cos(a)*shotSpeed,0);
          placeEmitExplosion(position+Vector(cos(a)*10,sin(a)*10,0));
          GO *enemyShot = go_(new EnemyShot(position+normalize(dir)*10,dir));
          dynamic_cast<EnemyShot*>(enemyShot)->red=red;
          gameObjects.push_back(enemyShot);
        }
        enemyShotSound->play(position);
      }
    }
  }
  virtual void paint(double dt) {
    glPushMatrix();
    glTranslatef(position.x,position.y,position.z);
    glRotatef(seconds*2,sin(seconds*3),cos(seconds*3),0);
    reColor[0xffffffff] = red ? 0xff0000ff : 0xff000000;
    markDebug = debugMark;
    drawMesh(enemy4);
    markDebug = false;
    reColor.clear();
    glPopMatrix();
  }
  virtual void destruct() {
    placeExplosion(position);
    GO::destruct();
  }
};

class Boss1FlyOver : public GO, public GO_Position, public GO_Paintable, public GO_LifeTime {
public:
  Boss1FlyOver(const Vector &p) : GO(), GO_Position(p), GO_Paintable(), GO_LifeTime(2) {
  }
  virtual void paint(double dt) {
    glPushMatrix();
    glTranslatef(position.x,position.y,position.z);
    glRotatef(seconds*90,0,1,0);
    position.y -= dt * 128;
    markDebug = debugMark;
    reColor[0x80ffc000] = 0xffffff00;
    drawMesh(boss1);
    reColor.clear();
    markDebug = false;
    glPopMatrix();
  }
  virtual void activated() {
    position.y += 190;
  }
  virtual void deActivated() {
    deleteIt = false;
    active = true;
  }
};

class ShipFlyOver : public GO, public GO_Position, public GO_Paintable, public GO_LifeTime {
public:
  ShipFlyOver(const Vector &p) : GO(), GO_Position(p), GO_Paintable(), GO_LifeTime(4) {
  }
  virtual void paint(double dt) {
    glPushMatrix();
    glTranslatef(position.x,position.y,position.z);
    position.y -= dt * 64;
    markDebug = debugMark;
    glScalef(1.25,1,1);
    drawMesh(player);
    markDebug = false;
    glPopMatrix();
  }
  virtual void activated() {
    position.y += 190;
  }
  virtual void deActivated() {
    deleteIt = false;
    active = true;
  }
};

class Boss1 : public GO, public GO_Paintable, public GO_FrequencyCallback, public GO_Collider_Enemy, public GO_HitPoints, public GO_ScoreHit,  public GO_ScoreDestructed, public GO_IdleCallback {
public:
  Vector position;
  int state;
  bool playerShotHit;
  bool flash;
  Boss1() : GO(), GO_Paintable(), GO_FrequencyCallback(0.5), GO_Collider_Enemy(20), GO_HitPoints(1024), GO_ScoreHit(15),  GO_ScoreDestructed(10000), GO_IdleCallback() {
    state = 0;                                     
    playerShotHit = false;
    flash = false;
  }

  void shootSlow1(const Vector &p,bool red) {
    for(double i = -1;i<=1; i+=0.25) {
      Vector dir;
      double speed = 25;
      dir.x = sin((i*40+0)*PI*2/360.0)*speed;
      dir.y = cos((i*40+0)*PI*2/360.0)*speed;
      dir.z = 0;
      GO *enemyShot = go_(new EnemyShot2(p,dir));
      dynamic_cast<EnemyShot2*>(enemyShot)->red=red;
      gameObjects.push_back(enemyShot);
    }
    placeEmitExplosion(p);
  }

  virtual void idleCallback(double dt) {
    if (levelScroll == levelLength) {
      activated();
      active=true; // removes idle
      displayWarning = false;
    } else {
      double d = fabs(levelScroll-levelLength);
      if (d<150)  {
        displayWarning = fmod(d,50)<25.0;
      }
    }
  }

  virtual void frequent(int iteration) {
    double posisX[6] = {-70,-45,-20,20,45,70};
    double posisY[6] = {7.5,7.5,12.5,12.5,7.5,7.5};
    if (state == 1) {
      if ((iteration & 15) < 12) {
        for (int i = 0; i < 6; i++) {
          if (((iteration+i)%3)==0)
            shootSlow1(position+Vector(posisX[i]*1.25,posisY[i],0),(i+iteration) & 4);
        }
      } else {
        if ((iteration & 15) == 12) {
          for (int i = 0; i < 6; i++) {
            if (((iteration+i)%3)==0)
              shootSlow1(position+Vector(posisX[i]*1.25,posisY[i],0),true);
          }
        }
      }
    }
  }
  virtual void paint(double dt) {
    glPushMatrix();
    switch(state) {
    case 0: {
        position.y += dt * 30;
        if (position.y > -levelScrollY-50) {position.y = -levelScrollY-50; state = 1; seconds = 0;}
        glRotatef(((-levelScrollY-50)-position.y)*5.f,0,1,0);
      } break;
    case 1: {
        position.x = sin(seconds)*50;
      } break;
    }
    glTranslatef(position.x,position.y,position.z);
    if (state == 1) {
      glRotatef(sin(seconds*1.5)*2.5,0,0,1);
      glRotatef(sin(seconds*2)*2.5,0,1,0);
      glRotatef(sin(seconds*0.5)*2.5,1,0,0);
    }
    markDebug = debugMark;
    glRotatef(180,0,0,1);
    glScalef(1.25,1,1);
    reColor[0x80ffc000] = 0xffffff00;
    if (flash) {
      reColor[0xff000000] = 0xffffffff;
      reColor[0x80ffc000] = 0xffffff80;
      flash=false;
      for (int i = 0; i < 1; i++) {
        static unsigned int ka = 0; ka++;
        if ((ka &3)==1)
          placeExplosion(position+Vector(randomLike(ka*33)*100.0-50.0,randomLike(ka*77)*40.0-20.0,-25));
      }
    }
    drawMesh(boss1);
    reColor.clear();
    markDebug = false;
    glPopMatrix();
  }
  virtual void activated() {
    position = Vector(0,-levelScrollY-120,0); // no GO_Position
    state = 0;
  }

  virtual bool collideWithCapsule(int capsuleSlot) {
    if (!__RUNNING(this)) return false;
    capsule[CAPSULE_COLLIDER] = Capsule(position+Vector(0,-30,0),position+Vector(0,50,0),5);
    if (collide(CAPSULE_COLLIDER,capsuleSlot)) {colliderEnemyPosition=position+Vector(0,50,0); colliderEnemyRadius = 10; if (capsuleSlot==CAPSULE_PLAYERSHOT) playerShotHit=true;return true;}
    capsule[CAPSULE_COLLIDER] = Capsule(position+Vector(-110,-30,0),position+Vector(110,-30,0),40);
    if (collide(CAPSULE_COLLIDER,capsuleSlot)) {colliderEnemyPosition=Vector(collisionCenter.x,position.y+10,collisionCenter.z); colliderEnemyRadius = 10; return true;}
    return false;
  }

  virtual void subtractHitPoints(int count) {
    if (!__RUNNING(this)) return;
    if (playerShotHit) {
      playerShotHit=false;
      flash = true;
      GO_HitPoints::subtractHitPoints(count);
    }
  }
  virtual void destruct() {
    for (int i = 0; i < 10; i++) {
      static unsigned int ka = 0; ka++;
      placeExplosion(position+Vector(randomLike(ka*33)*100.0-50.0,randomLike(ka*77)*40.0-20.0,-25));
    }
    GO::destruct();
    levelCompleted = true;
  }
};

void loadLevel1() {
  if (enemy1 != NULL)  {delete enemy1; enemy1 = NULL;}
  if (enemy2 != NULL) {delete enemy2; enemy2 = NULL;}
  if (enemy3 != NULL) {delete enemy3; enemy3 = NULL;}
  if (enemy4 != NULL) {delete enemy4; enemy4 = NULL;}
  if (boss1 != NULL)    {delete boss1; boss1 = NULL;}
  if (collect != NULL)  {delete collect; collect = NULL;}
  if (heart != NULL)    {delete heart; heart = NULL;}
  if (object1 != NULL)  {delete object1; object1 = NULL;}
  if (object2 != NULL) {delete object2; object2 = NULL;}
  if (object3 != NULL) {delete object3; object3 = NULL;}
  enemy1 = loadObject("enemy1.obj");
  enemy2 = loadObject("enemy2.obj");
  enemy3 = loadObject("enemy3.obj");
  enemy4 = loadObject("enemy4.obj");
  boss1 = loadObject("boss1.obj");
  collect = loadObject("collect.obj");
  heart = loadObject("heart.obj");
  object1 = loadObject("object1.obj");
  object2 = loadObject("object2.obj");
  object3 = loadObject("object3.obj");
  centerAndResizeObject(enemy1,10.0);
  centerAndResizeObject(enemy2,15.0);
  centerAndResizeObject(enemy3,10.0);
  centerAndResizeObject(enemy4,10.0);
  centerAndResizeObject(boss1,100.0);
  centerAndResizeObject(collect,7.5);
  centerAndResizeObject(heart,7.5);
  centerAndResizeObject(object1,30.0);
  centerAndResizeObject(object2,15.0);
  centerAndResizeObject(object3,10.0);
}

void buildLevel1() {
  gameObjects.clear();

  // level enemies   
  int i,j;

  for (i = 0; i < 10; i++) {
    gameObjects.push_back(go_(new Enemy1(Vector(randomLike(i*33)*150-75,-100-i*20,0))));
  }
  for (i = 0; i < 5; i++) {
    bool red = i & 1;
    double x = randomLike(i*77)*50+50;
    gameObjects.push_back(go_(new Enemy2(Vector(x,-450-i*50,0),red,0)));
    gameObjects.push_back(go_(new Enemy2(Vector(-x,-450-i*50,0),red,0)));
    if (i>2)
      gameObjects.push_back(go_(new Enemy2(Vector(0,-450-i*50-25,0),red,1)));
  }
  for (i = 0; i < 3; i++) {
    gameObjects.push_back(go_(new Enemy1(Vector(-75,-450-5*50-i*80,0))));
    gameObjects.push_back(go_(new Enemy1(Vector(75,-450-5*50-i*80,0))));
  }

  for (i = 0; i < 10; i++) {
    if (i & 1) {
      gameObjects.push_back(go_(new Enemy3(Vector(randomLike(i*33)*150-75,-1100-i*80,0))));
    } else {
      gameObjects.push_back(go_((new Enemy2(Vector(randomLike(i*33)*150-75,-1100-i*80,0),true,2))->circularShots()));
      gameObjects.push_back(go_((new Enemy2(Vector(randomLike(i*33)*150-75,-1100-i*80-40,0),false,1))->circularShots()));
    }
  }

  for (i = 0; i < 20; i++) {
    float z = 0;
    float kx = randomLike(i*77+33)*200.0-100.0;
    float ky = 2000+randomLike(i*44+33)*800;
    gameObjects.push_back(go_(new Enemy4(Vector(kx,-ky,z))));
  }

  for (i = 0; i < 5; i++) {
    bool red = i & 1;
    double x = randomLike(i*77)*50+50;
    gameObjects.push_back(go_(new Enemy2(Vector(x,-2200-i*50,0),red,0)));
    gameObjects.push_back(go_(new Enemy2(Vector(-x,-2200-i*50,0),red,0)));
    if (i>2)
      gameObjects.push_back(go_(new Enemy2(Vector(0,-2200-i*50-25,0),red,1)));
  }

  // level objects
  for (i = 0; i < 10; i++) {
    double xp = 75;
    gameObjects.push_back(go_((new LevelObject(Vector(-xp,-1100-i*5,0), Vector(0,1,0,0), object1))->randomRotate()));
    gameObjects.push_back(go_((new LevelObject(Vector(xp,-1100-i*5,0), Vector(0,1,0,180), object1))->randomRotate()));
  }

  for (i=-45*2;i<=45*2;i+=45) {
    gameObjects.push_back(go_((new LevelObject(Vector(i,-1050,-20), Vector(0,0,0,0), object3))->scale(Vector(4.5,3,3))->rotate(Vector(45,1,0,0))));
    gameObjects.push_back(go_((new LevelObject(Vector(i,-2050,-20), Vector(0,0,0,0), object3))->scale(Vector(4.5,3,3))->rotate(Vector(45,1,0,0))));
    gameObjects.push_back(go_((new LevelObject(Vector(i,-2850,-20), Vector(0,0,0,0), object3))->scale(Vector(4.5,3,3))->rotate(Vector(45,1,0,0))));
  }


  for (i = 0; i < 100; i++) {
    float z = 40;
    int secs = 7;
    float kx = (double(i % secs)/(secs-1)*2-1)*100.0;
    float ky = (i/secs)*7.5+2000;
    if (randomLike(i*33)<0.5)
      gameObjects.push_back(go_((new LevelObject(Vector(kx,-ky,z), Vector(0,0,0,0), object2))->randomRotate()));
  }

  for (j = 0; j < 2; j++) {
    for (int k = 0; k < 3; ++k) {
      double r = 1.0-(double)k/3;
      for (i = 0; i < 5; i++) {
        double xp = 75;
        gameObjects.push_back(go_((new LevelObject(Vector((-80+j*160)*r,-500-i*8-k*100,-xp), Vector(0,1,0,0-90), object1))));
        gameObjects.push_back(go_((new LevelObject(Vector((-80+j*160)*r,-500-i*8-k*100,xp), Vector(0,1,0,180-90), object1))));
      }
    }
  }

  for (j = 0; j < 2; j++) {
   for (i = 0; i < (j==0?8:18); i++) {
      double xp = 95;
      double a = i * 20;
      double a2 = a * PI / 180.0;
      a += 90;
      double lp = -100 - j * 1240;
      gameObjects.push_back(go_(new LevelObject(Vector(sin(a2)*xp,lp-i*4,cos(a2)*xp*0.2), Vector(0,1,0,0+a), object1)));
      gameObjects.push_back(go_(new LevelObject(Vector(sin(a2+PI)*xp,lp-i*4,cos(a2+PI)*xp*0.2), Vector(0,1,0,180+a), object1)));
    }
  }

  // cutscene stuff
  gameObjects.push_back(go_(new Boss1FlyOver(Vector(0,-200,-50))));
  gameObjects.push_back(go_(new ShipFlyOver(Vector(-30,-2700,-150))));
  gameObjects.push_back(go_(new ShipFlyOver(Vector(+30,-2725,-120))));

  // collectables
  gameObjects.push_back(go_((new Collectable(Vector(50,-400,0)))->weaponGreen()));
  gameObjects.push_back(go_((new Collectable(Vector(-50,-800,0)))->weaponGreen()));
  gameObjects.push_back(go_((new Collectable(Vector(0,-500,0)))->life()));
  gameObjects.push_back(go_((new Collectable(Vector(0,-1500,0)))->life()));
  gameObjects.push_back(go_((new Collectable(Vector(0,-2900,0)))->life()));

  // endboss
  gameObjects.push_back(go_(new Boss1()));
}
