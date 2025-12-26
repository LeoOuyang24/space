#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

//...I hardly know her!

//code in this file run a function repeatedly until it returns true
//kinda like godot tweens?


#include <functional>
#include <list>

//once this returns true, we stop running it
struct RunThis
{
    typedef std::function<bool(int)> Func;
    Func func; //function to run, takes in an int that is the number of times this has been run

    int ranNumTimes = 0; //number of times this function has been called

    RunThis(const Func& f) : func(f)
    {

    }

    bool operator()()
    {
        bool val = func(ranNumTimes);
        ranNumTimes++;
        return val;
    }
};

//runs the front most function until it returns true, at which point is is removed
typedef std::list<RunThis> Sequencer;

//stores Sequencers, which can be set to run either once per frame or once per loop
struct Sequences
{
    typedef std::list<Sequencer> SequencerList;
    static SequencerList physicsSequences; //sequences that run once per frame
    static SequencerList renderSequences; //sequences that run once per loop

    static void run(SequencerList& lst);
    static bool run(Sequencer& seq); //run a sequence, return true if it is done (has no more units to run)

public:
    //isPhysics = true means we add to "physicsSequences", else "renderSequences"
    static void add(const Sequencer& sequence, bool isPhysics);
    static void add(const RunThis& runThis, bool isPhysics);

    template<typename... Callable>
    static void add(bool isPhysics, Callable... callable)
    {
        Sequencer seq = {RunThis::Func(callable)...};
        add(seq,isPhysics);
    }

    static void runPhysics();
    static void runRenders();
};



#endif // SEQUENCER_H_INCLUDED
