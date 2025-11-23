#include "../headers/factory.h"
#include "../headers/factories.h"
#include "../headers/objects.h"
#include "../headers/conversions.h"
#include "../headers/item.h"

std::shared_ptr<PhysicsBody> construct(std::string cereal)
{
    std::vector<std::string> params = split(cereal,' ');
    PhysicsBody* ptr = nullptr;

    if (params[0] == Factory<Key>::ObjectName)
    {
        Key* key = (new Key(Factory<Key>::Base::deserialize(&params[1])));
        key->tint = key->key;
        ptr = key;
    }
    else if (params[0] == Factory<PortalSpawner>::ObjectName)
    {
        ptr = new PortalSpawner(Factory<PortalSpawner>::Base::deserialize(&params[1]));
    }
    else
    {
        std::cerr << "ERROR construct: unable to construct object: " << params[1] << "\n";
    }

    return std::shared_ptr<PhysicsBody>(ptr);
}


