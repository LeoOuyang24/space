#include "../headers/enemy.h"
#include "../headers/player.h"

GrapplePoint::GrapplePoint()
{
    followGravity = false;
    collider.radius = 50;
    renderer.sprite = Globals::Game.Sprites.getSprite("grapple.png");
}

void GrapplePoint::update(Terrain& terrain)
{
    Object<CircleCollider,TextureRenderer,GrapplePoint>::update(terrain);

    Vector2 balls = screenToWorld(GetMousePosition(),Globals::Game.getCamera(),Globals::Game.getCurrentZ());

    if (Vector2DistanceSqr(screenToWorld(GetMousePosition(),Globals::Game.getCamera(),Globals::Game.getCurrentZ()),getPos()) <= collider.radius*collider.radius &&
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

    //DrawLine3D(toVector3(orient.pos),toVector3(endPos),RED);
    DrawSprite3D(laserBeam.texture,
                 Rectangle(laser.orient.pos.x,laser.orient.pos.y,laser.collider.dimens.x,laser.collider.dimens.y),
                 orient.rotation);
    //DrawSphere(toVector3(laser.orient.pos),10,BLACK);
}

void LaserBeamEnemy::update(Terrain& t)
{
    Object<RectCollider,TextureRenderer,LaserBeamEnemy>::update(t);

    orient.rotation = (func == SINE ?
                        arc/2*DEG2RAD*(sin(GetTime())) :
                        fmod(GetTime(),std::max(1.0f,arc*DEG2RAD))
                        );
   // orient.rotation += 0.01;

}

Shape LaserBeamEnemy::getShape()
{
    Vector2 endPos = Globals::Game.getCurrentTerrain()->lineBlockIntersect(orient.pos,
                                                      orient.pos + Vector2(cos(orient.rotation),sin(orient.rotation))*beamLength,
                                                      true);

    Shape shape = {RECT,{(endPos + orient.pos)*0.5,orient.layer,orient.rotation},ShapeCollider(Vector2(Vector2Length(endPos - orient.pos),10))};
    return shape;
}

void LaserBeamEnemy::collideWith(PhysicsBody& other)
{
    if (&other == Globals::Game.getPlayer())
    {
        //std::cout << "collided " << GetTime() << "\n";
        Globals::Game.getPlayer()->setDead(true);
    }
}
