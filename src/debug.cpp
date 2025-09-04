#include "../headers/debug.h"

std::list<DeferredCall> Debug::DeferRenders;
bool Debug::paused = false;

void Debug::togglePaused()
{
    paused = !paused;
}

bool Debug::isPaused()
{
    return paused;
}

void Debug::addDeferRender(DeferredCall call)
{
    DeferRenders.push_back(call);
}

void Debug::renderDefers()
{
    for (auto func : DeferRenders)
    {
        func();
    }
}

void Debug::clearRenderDefers()
{
    DeferRenders.clear();
}
