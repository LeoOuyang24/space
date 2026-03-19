#include "../headers/factory.h"
#include "../headers/objects.h"
#include "../headers/conversions.h"
#include "../headers/item.h"
#include "../headers/portal.h"
#include "../headers/interactives.h"
#include "../headers/enemy.h"

std::unordered_map<std::string,std::function<PhysicsBody*(const SplitString& params)>> ClassDeserializer::funcs;

void ClassDeserializer::init()
{
    registerName<Key>();
    registerName<Portal>();
    registerName<Collectible>();
    registerName<Sign>();
    registerName<Rover>();
    registerName<BigSign>();
    registerName<LaserBeamEnemy>();
    registerName<Barrel>();
    registerName<BarrelReceiver>();
    registerName<BarrelSpawner>();
    registerName<MovingTerrain>();
    registerName<AntiGravPod>("antigravpod");
    registerName<PushBot>();
    registerName<LargePushBot>();
    registerName<GlowStone>("glowstone");
    registerName<GravitySwitch>();
    registerName<DestroyLaser>();
}

std::shared_ptr<PhysicsBody> ClassDeserializer::construct(std::string_view cereal)
{
    SplitString params(cereal,'\t');
    PhysicsBody* ptr = nullptr;
    if (params.size() > 0)
    {
        std::string str2(params[0]);
        if (funcs.find(str2) != funcs.end())
        {
            ptr = funcs[str2](params);
        }
        else
        {
            std::cerr << "ERROR construct: unable to construct object: " << params[0] << "\n";
        }
    }
    return std::shared_ptr<PhysicsBody>(ptr);
}


