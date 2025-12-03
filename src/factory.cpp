#include "../headers/factory.h"
#include "../headers/objects.h"
#include "../headers/conversions.h"
#include "../headers/item.h"
#include "../headers/portal.h"

std::shared_ptr<PhysicsBody> construct(std::string cereal)
{
    std::vector<std::string> params = split(cereal,' ');
    PhysicsBody* ptr = nullptr;
    if (params.size() > 0)
    {
        if (params[0] == Factory<Key>::ObjectName)
        {
            Key* key = (new Key(Factory<Key>::Base::deserialize(params)));
            key->tint = key->key;
            ptr = key;
        }
        else if (params[0] == Factory<PortalSpawner>::ObjectName)
        {
            ptr = new PortalSpawner(Factory<PortalSpawner>::Base::deserialize(params));
        }
        else if (params[0] == Factory<Portal>::ObjectName)
        {
            ptr = new Portal(Factory<Portal>::Base::deserialize(params));
        }
        else if (params[0] == Factory<Collectible>::ObjectName)
        {
            ptr = new Collectible(Factory<Collectible>::Base::deserialize(params));
        }
        else
        {
            std::cerr << "ERROR construct: unable to construct object: " << params[0] << "\n";
        }
    }

    return std::shared_ptr<PhysicsBody>(ptr);
}


