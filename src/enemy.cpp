#include "../headers/enemy.h"
#include "../headers/player.h"
#include "../headers/portal.h"

GrapplePoint::GrapplePoint()
{
    followGravity = false;
    collider.radius = 50;
    renderer.sprite = Globals::Game.Sprites.getSprite("grapple.png");
}

void GrapplePoint::update(Terrain& terrain)
{
    Object<CircleCollider,TextureRenderer,GrapplePoint>::update(terrain);

    if (Vector2DistanceSqr(screenToWorld(GetMousePosition(),Globals::Game.Camera.getCamera(),Globals::Game.getCurrentZ()),getPos()) <= collider.radius*collider.radius &&
        IsMouseButtonDown(MOUSE_LEFT_BUTTON)
        )
    {
        Player* playuh = static_cast<Player*>(Globals::Game.getPlayer());
       /* playuh->setState(Player::SWINGING);
        playuh->setGrapplePoint(getPos());
        playuh->grappleForce = playuh->forces.getTotalForce();*/
        playuh->forces.addForce({Vector2Normalize(getPos() - playuh->getPos())},Forces::SWINGING);
    }
}

void LaserBeamEnemy::render()
{

    renderer.render(Object<RectCollider,TextureRenderer,LaserBeamEnemy>::getShape(),tint);

    Shape laser = getShape();

        DrawLine3D(toVector3(getPos()),
               toVector3(getPos() + Vector2(cos(orient.rotation),sin(orient.rotation))*laser.collider.dimens.x),
               RED,laser.collider.dimens.y);


    /*DrawSprite3D(laserBeam.texture,
                 Rectangle(laser.orient.pos.x,laser.orient.pos.y,laser.collider.dimens.x,laser.collider.dimens.y),
                 orient.rotation);*/
    //DrawSphere(toVector3(laser.orient.pos),10,BLACK);
}

void LaserBeamEnemy::update(Terrain& t)
{
    //Object<RectCollider,TextureRenderer,LaserBeamEnemy>::update(t);

    orient.rotation = (func == SINE ?
                        startingRot*DEG2RAD + arc/2*DEG2RAD*(sin(GetTime())) :
                        fmod(startingRot*DEG2RAD + GetTime(),std::max(1.0f,arc*DEG2RAD))
                        );
    setPos(movement(orient,orient.getStartingPos()));
   // orient.rotation += 0.01;

}

Shape LaserBeamEnemy::getShape() const
{
    Vector2 endPos = Globals::Game.getCurrentTerrain()->lineBlockIntersect(orient.pos,
                                                      orient.pos + Vector2(cos(orient.rotation),sin(orient.rotation))*beamLength,
                                                      true);

    Shape shape = {RECT,{(endPos + orient.pos)*0.5,orient.layer,orient.rotation},ShapeCollider(Vector2(Vector2Distance(endPos,getPos()),10))};
    return shape;
}

void LaserBeamEnemy::collideWith(PhysicsBody& other)
{
    if (&other == Globals::Game.getPlayer())
    {
        Globals::Game.getPlayer()->setDead(true);
    }
}

void Disintegrate::collideWith(PhysicsBody& other)
{
    CircleTerrain::collideWith(other);
    if (start == 0 || getDisintegratedState() == 1)
    {
        start = GetTime();
    }
}

bool Disintegrate::isTangible()
{
    return getDisintegratedState() > 0;
}

void Disintegrate::render()
{
    tint.a = getDisintegratedState()*255;
    CircleTerrain::render();
}

float Disintegrate::getDisintegratedState()
{
    if (start != 0)
    {
        float duration = GetTime() - start;
        if (duration >= TOTAL_DURATION) //we have reconstituted
        {
            return 1; 
        }
        else if (duration > DISINTEGRATE_TIME) //we have disintegrated and not reconstituted
        {
            return 0;
        }
        else //in the proces of disintegrating
        {
            return 1 - duration/DISINTEGRATE_TIME; 
        }
    }
    return 1; //havent' even begun disintegrating yet
}

std::string Disintegrate::serialize()
{
    return Factory<Disintegrate>::Base::serialize(*this);
}

/*Shape GravityStream::getShape() const
{
    return {ShapeType::RECT,getOrient(),Vector2{std::max(abs(gravDir.x)*10,100.0f),std::max(abs(gravDir.y)*10,100.0f)}};
}*/

void GravityStream::onDeserialize()
{
    Vector2 pos = getPos();
    Vector2 endpoint = Globals::Game.terrain.getTerrain(orient.layer)->lineBlockIntersect(pos,pos + gravDir*(Terrain::MAX_TERRAIN_SIZE),false);
    endpoint.x = Clamp(endpoint.x,0,Terrain::MAX_TERRAIN_SIZE);
    endpoint.y = Clamp(endpoint.y,0,Terrain::MAX_TERRAIN_SIZE);
    
    collider.width = Vector2Distance(endpoint,pos);
    collider.height = 200;

    setOrient({(pos + endpoint)*0.5,orient.layer,atan2(endpoint.y - pos.y, endpoint.x - pos.x)});
}

void GravityStream::collideWith(PhysicsBody& other)
{
    if (other.get_followGravity())
    {
        Shape shape = other.getShape();
        other.getForces().addForce(gravDir,Forces::ENEMY);

    }
}

void GravityStream::render()
{
    int horizComponent = gravDir.x ? gravDir.x/abs(gravDir.x) : 0; //1 if gravDir.x > 0, 0 if gravDir.x == 0, -1 if gravDir.x < 0
    int vertComponent = gravDir.y ? gravDir.y/abs(gravDir.y) : 0; //1 if gravDir.y > 0, 0 if gravDir.y == 0, -1 if gravDir.y < 0
    tint = Color{static_cast<unsigned char>(horizComponent*255/2.0f + 255/2.0f),static_cast<unsigned char>(vertComponent*255/2.0f + 255/2.0f),0,255};
    Object::render();
}

void PushBot::update(Terrain& t)
{
    Object::update(t);
   // std::cout << getPos() << "\n";
    //std::cout << GetTime() - time << " " << onGround << "\n";
    forces.setForce(orient.getFacingVector(),Forces::MOVE);
}


void PushBot::onCollide(PhysicsBody& other)
{
    if (other.get_followGravity())
    {
        other.forces.addForce(Vector2Normalize(other.getPos()-getPos()),Forces::ENEMY);
        other.forces.setForce({},Forces::MOVE);
        Sequences::add(false,[pos=getPos(),radius=collider.width](int){
                       DrawCircle3D(toVector3(pos),radius,{},0,WHITE);return true;
                       });
    }
}

void LargePushBot::activate(int pushAmount)
{
    push = pushAmount;
}

void LargePushBot::update(Terrain& t)
{
    Object::update(t);
}

void LargePushBot::onCollide(PhysicsBody& other)
{
    PushBot::onCollide(other);

    if (other.getKeyVal() > 0)
    {
        other.setDead(true);
        Globals::Game.Camera.setCameraFollow(getPos() + Vector2(100,0),120);
        Sequences::add(true,[](int){

            return Globals::Game.Camera.isDone();

        },
        [this](int x ){
            if (Globals::Game.objects.getObject(this))
            { 
                setPos(getPos() + Vector2(10*pow(0.9,1 - x/50.0f),0));
            } 
        return x >= 100;},
        [](int x){
            Globals::Game.Camera.setCameraFollow(true,120);
            return true;
        });
    }
}


void GlowStone::onCollide(PhysicsBody& other)
{
    if (Vector2LengthSqr(forces.getForce(Forces::ENEMY)) > 0)
    {
        setDead(true);
        //Globals::Game.addObject(*(new Portal(getPos(),orient.layer,{2433,1020},0)),orient.layer);
        Globals::Game.addObject(*(new BigGear()),{getPos(),orient.layer});
        Sequences::add(false,[start=GetTime(),pos=getPos()](int frames){
                   const Anime* anime = Globals::Game.Sprites.getAnime("death.png");
                   DrawAnime3D(anime->spritesheet,start,anime->info,{pos.x,pos.y,500,500},Globals::Game.getCurrentZ(),0,GRAY);
                   return isAnimeDone(anime->info,frames);
                   });
    }
}

void CameraMoveRegion::collideWith(PhysicsBody& other)
{
    if (&other == Globals::Game.getPlayer())
    {
        if (!wasActivated)
        {
            Globals::Game.Camera.clear();
            Globals::Game.Camera.setCameraFollow(toVector3(getPos()) - Vector3(0,0,Globals::CAMERA_Z_DISP) + cameraTarget,100); //okay for this to not be in queue so other camera moves can be queued up, including by leaving the region
        }
        activated = true;
    }

}

void CameraMoveRegion::update(Terrain& t)
{
    if (wasActivated && !activated)
    {
        Globals::Game.Camera.clear();
        Globals::Game.Camera.setCameraFollow(true,100);
    }
    wasActivated = activated;
    activated = false;
}