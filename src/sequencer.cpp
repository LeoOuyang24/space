#include "../headers/sequencer.h"



Sequences::SequencerList Sequences::physicsSequences;
Sequences::SequencerList Sequences::renderSequences;

SequencePtr Sequences::add(Sequencer* lst, bool isPhysics)
{
    SequencePtr temp;
    temp.reset(lst);
    add(temp,isPhysics);
    return isPhysics ? physicsSequences.front() : renderSequences.front();
}

SequencePtr Sequences::add(const RunThis& lst, bool isPhysics)
{
    return add((new Sequencer{lst}),isPhysics);
}

void Sequences::add(SequencePtr& seq, bool isPhysics)
{
    if (isPhysics)
    {
        physicsSequences.push_front(seq);
    }
    else
    {
        renderSequences.push_front(seq);
    }
}

SequencePtr Sequences::waitFor(std::function<bool()> runThis,bool isPhysics)
{
    SequencePtr ptr;
    ptr.reset(new Sequencer{RunThis([runThis](int){return runThis();})});
    Sequences::add(ptr,isPhysics);
    return ptr;
}

void Sequences::run(SequencerList& lst)
{
    for (auto it = lst.begin(); it != lst.end();)
    {
       if (!it->get() || (run(*it) && it->use_count() == 1)) //remove if a sequence is null or if it is empty AND there are no other pointers pointing to it. Other pointers pointing to it implies that it is a reserved sequence
       {
           it = lst.erase(it);
       }
       else
       {
           ++it;
       }
    }
}
bool Sequences::run(SequencePtr& seq)
{
    if (seq->size() > 0) //run first function, if it's done, remove it
    {
        auto it = seq->begin();
        if ((*it)())
        {
            seq->erase(it); //erase the iterator in case new RunThis were addeddas part of the run
        }
    }
    return seq->size() == 0;
}

void Sequences::runPhysics()
{
    run(physicsSequences);
}

void Sequences::runRenders()
{
    run(renderSequences);
}
