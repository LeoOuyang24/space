#include "../headers/sequencer.h"



Sequences::SequencerList Sequences::physicsSequences;
Sequences::SequencerList Sequences::renderSequences;

void Sequences::add(const Sequencer& lst, bool isPhysics)
{
    if (isPhysics)
    {
        physicsSequences.emplace_front(lst);
    }
    else
    {
        renderSequences.emplace_front(lst);
    }
}

void Sequences::add(const RunThis& lst, bool isPhysics)
{
    add(Sequencer{lst},isPhysics);
}


void Sequences::run(SequencerList& lst)
{
    for (auto it = lst.begin(); it != lst.end();)
    {
       if (run(*it))
       {
           it = lst.erase(it);
       }
       else
       {
           ++it;
       }
    }
}

bool Sequences::run(Sequencer& seq)
{
    if (seq.size() > 0 && (*seq.begin())()) //run first function, if it's done, remove it
    {
        seq.pop_front();
    }
    return seq.size() == 0;
}

void Sequences::runPhysics()
{
    run(physicsSequences);
}

void Sequences::runRenders()
{
    run(renderSequences);
}
