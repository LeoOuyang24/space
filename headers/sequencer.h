#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

//...I hardly know her!

//code in this file run a function repeatedly until it returns true
//kinda like godot tweens?


#include <functional>
#include <list>
#include <memory>

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

//an actual sequence
//runs until empty. If a Sequence is the only pointer to a Sequencer, it is deleted
//however, Sequencers with multiple pointers are kept, even if empty. This allows "queueing", where an object (like GameCamera),
//can continuously add RunThis to one Sequence
typedef std::shared_ptr<Sequencer> Sequence;
//stores Sequencers, which can be set to run either once per frame or once per loop
struct Sequences
{
    typedef std::list<Sequence> SequencerList;
    static SequencerList physicsSequences; //sequences that run once per frame
    static SequencerList renderSequences; //sequences that run once per loop

    static void run(SequencerList& lst);
    static bool run(Sequence& seq); //run a sequence, return true if it is done (has no more units to run)

public:
    //isPhysics = true means we add to "physicsSequences", else "renderSequences"
    static Sequence add(Sequencer* sequence, bool isPhysics);
    static Sequence add(const RunThis& runThis, bool isPhysics);
    static void add(Sequence& seq, bool isPhysics);  //add a Sequence, that already exists, probably because you want to have multiple pointers

    template<typename... Callable>
    static Sequence add(bool isPhysics, Callable... callable)
    {
        return add((new Sequencer{RunThis::Func(callable)...}),isPhysics);
    }

    static void runPhysics();
    static void runRenders();
};



#endif // SEQUENCER_H_INCLUDED
