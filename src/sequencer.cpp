#include "../headers/sequencer.h"

Sequences::SequencerList Sequences::physicsSequences;
Sequences::SequencerList Sequences::renderSequences;

std::shared_ptr<Sequencer>& Sequences::add(bool isPhysics, Sequencer::Func func)
{
    return add(isPhysics,Sequencer(func));
}

std::shared_ptr<Sequencer>& Sequences::add(bool isPhysics, Sequencer&& seq)
{
    auto ptr = std::make_shared<Sequencer>(std::move(seq));
    return add(isPhysics,ptr);
}

std::shared_ptr<Sequencer>& Sequences::add(bool isPhysics, std::shared_ptr<Sequencer>& ptr)
{
     if (isPhysics)
    {
        physicsSequences.push_back(ptr);
        return *std::prev(physicsSequences.end());
    }
    else
    {
        renderSequences.push_back(ptr);
        return *std::prev(renderSequences.end());
    }   
}

void Sequences::run(SequencerList& lst)
{
    for (auto it = lst.begin(); it != lst.end();)
    {
       if (!(*it) || (*it)->empty()) //remove if a sequence is null or if it is empty
       {
           it = lst.erase(it);
       }
       else
       {
            //run and possibly pop
            if ((*it)->run())
            {
                (*it)->pop();
            }
            ++it;
       }
    }
}

void Sequences::runPhysics()
{
    run(physicsSequences);
}

void Sequences::runRenders()
{
    run(renderSequences);
}
