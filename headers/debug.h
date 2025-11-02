#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <functional>
#include <list>

typedef std::function<void()> DeferredCall;
class Debug
{
    static std::list<DeferredCall> DeferRenders; //deferred render calls

    static bool paused;
public:
    static void togglePaused();
    static bool isPaused();

    static void addDeferRender(DeferredCall call);

    static void clearRenderDefers();
    static void renderDefers();


};


#endif // DEBUG_H_INCLUDED
