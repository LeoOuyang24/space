#include <algorithm>

#include "../headers/collideTriggers.h"
#include "../headers/item.h"
#include "../headers/objects.h"
#include "../headers/player.h"

void HoldThis::collideWith(PhysicsBody& self, PhysicsBody& other)
{
    if (&other  == Globals::Game.player.get())
    {
       // static_cast<Player*>(&other)->setHolding(*static_cast<Item*>(&self));
    }
}

TriggerSpawnPlanets::TriggerSpawnPlanets(const SplitString& split)
{
    if (split.size() > 2)
    {
        duration = fromString<decltype(duration)>(split[1]);
        for (int i = 2; i < split.size();)
        {
            points.push_back(Circle::Factory::deserialize(split,i));
            i += Circle::Factory::size;
        }
    }
}

void TriggerSpawnPlanets::operator()(PhysicsBody& body)
{
    for (int i = 0; i < points.size(); i ++)
    {
        Globals::Game.terrain.getTerrain(body.getOrient().layer)->generatePlanet(
                                                                                 points[i].center + body.getPos()*(!points[i].absolute),
                                                                                 points[i].radius,
                                                                                 points[i].color);
    }

}

std::string TriggerSpawnPlanets::to_string()
{
    std::string str = "trigger_spawn_planets|" + toString<decltype(duration)>(duration) + "|";
    std::for_each(points.begin(),points.end(),[&str](Circle& circle){
                  str = str +Circle::Factory::serialize(circle,"") + ",";
                  });
    return str;
}
