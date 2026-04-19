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
    if (push > 0)
    {
        forces.addForce(orient.getFacingVector()*5,Forces::MOVE);
        push --;
    }
    Object::update(t);
}

void GlowStone::onCollide(PhysicsBody& other)
{
    if (Vector2LengthSqr(forces.getForce(Forces::ENEMY)) > 0)
    {
        setDead(true);
        Globals::Game.addObject(*(new Portal(getPos(),orient.layer,{},orient.layer)),orient.layer);
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
            std::cout << "entered\n";
            Globals::Game.Camera.clear();
            Globals::Game.Camera.startQueue();
                Globals::Game.Camera.setCameraFollow(toVector3(getPos()) - Vector3(0,0,Globals::CAMERA_Z_DISP) + cameraTarget,100); //okay for this to not be in queue so other camera moves can be queued up, including by leaving the region
            Globals::Game.Camera.stopQueue();
        }
        activated = true;
    }

}

void CameraMoveRegion::update(Terrain& t)
{
    if (wasActivated && !activated)
    {
        std::cout << "left\n";
        Globals::Game.Camera.clear();
        Globals::Game.Camera.startQueue();
            Globals::Game.Camera.setCameraFollow(true,100);
        Globals::Game.Camera.stopQueue();
    }
    wasActivated = activated;
    activated = false;
}