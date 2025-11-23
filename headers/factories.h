#ifndef FACTORIES_H_INCLUDED
#define FACTORIES_H_INCLUDED

//defines Factory specializations for each object

#include "factory.h"

#include "portal.h"

template<>
struct Factory<Key>
{
  static constexpr std::string ObjectName = "key";
  using Base = FactoryBase<Key,
                        access<Key,&Key::key>,
                        access<Key,&Key::orient,&Orient::pos>,
                        access<Key,&Key::orient,&Orient::layer>,
                        access<Key,&Key::collider,&RectCollider::width>,
                        access<Key,&Key::collider,&RectCollider::height>,
                        access<Key,&Key::renderer,&TextureRenderer::sprite>>;
};

template<>
struct Factory<PortalSpawner>
{
    static constexpr std::string ObjectName = "portal_spawner";
    using Base = FactoryBase<PortalSpawner,
                                access<PortalSpawner,&PortalSpawner::orient,&Orient::pos>,
                                access<PortalSpawner,&PortalSpawner::orient,&Orient::layer>,
                                access<PortalSpawner,&PortalSpawner::collider,&CircleCollider::radius>,
                                access<PortalSpawner,&PortalSpawner::renderer,&TextureRenderer::sprite>,
                                access<PortalSpawner,&PortalSpawner::collideTrigger,&TriggerPortalSpawn::start>,
                                access<PortalSpawner,&PortalSpawner::collideTrigger,&TriggerPortalSpawn::end>,
                                access<PortalSpawner,&PortalSpawner::collideTrigger,&TriggerPortalSpawn::absolute>>;

};


#endif // FACTORIES_H_INCLUDED
