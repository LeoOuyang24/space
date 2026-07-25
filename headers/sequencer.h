#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

//...I hardly know her!

//code in this file run a function repeatedly until it returns true
//kinda like godot tweens?


#include <functional>
#include <list>
#include <memory>

class Sequencer
{
public:
    typedef std::function<bool(int)> Func;
private:
    struct SequenceNode;
    typedef std::shared_ptr<SequenceNode> RunPtr;
    typedef std::weak_ptr<SequenceNode> WeakRunPtr;


    //A linked list-esque wrapper around a callable function
    struct SequenceNode
    {

        RunPtr next;
        Func func; //function to run, takes in an int that is the number of times this has been run

        int ranNumTimes = 0; //number of times this function has been called

        SequenceNode(const Func& f) : func(f)
        {
            
        }
        virtual bool operator()()
        {
            bool val = func(ranNumTimes);
            ranNumTimes++;
            return val;
        }
    };

    RunPtr head;
    WeakRunPtr end; 
public:
    Sequencer(Func func){add(func);};
    Sequencer(){};

    RunPtr getHead(){return head;}
    WeakRunPtr getEnd(){return end;}
    
    Sequencer& add(Sequencer other)
    {
        if (!other.empty())
        {
            if (!empty())
            {
                end.lock()->next = other.getHead();
            }
            else
            {
                head = other.getHead();
            }
            end = other.getEnd();
        }
        return *this;
    }
    Sequencer& add(Func func)
    {
        SequenceNode* node = new SequenceNode(func);
        if (!empty())
        {
            end.lock()->next.reset(node);
            end = end.lock()->next;
        }
        else
        {
            head.reset(node);
            end = head;
        }
        return *this;
    }
    Sequencer& parallel(Func newFunc)
    {
        if (!empty())
        {
            //modify our end-most node's function to call this new function as well
            end.lock()->func = [newFunc,oldFunc=end.lock()->func](int times)
                                {
                                    bool val1 = oldFunc(times);
                                    bool val2 = newFunc(times); //gotta call them separately so we don't short circuit
                                    return val1 && val2;
                                }; 
        }
        else
        {
            add(newFunc);
        }

        return *this;
    }

    void pop()
    {
        if (head)
        {
            //move to the next node
            //possibly delete the current node if there are no other pointers pointing to it
            head = head->next;
        }
    }

    bool empty() const
    {
        return (head.get() == nullptr);
    }

    //clear the sequence by setting the head to past the last node
    void clear() 
    {
        if (!empty())
        {
            head = end.lock()->next;
            end = head;
        }
    }

    bool run()
    {
        if (empty())
        {
            return true;
        }
        else
        {
            return (*head)();
        }
    }
};


//runs the front most function until it returns true, at which point is is removed
//typedef std::list<RunThis> Sequencer;

//stores Sequencers, which can be set to run either once per frame or once per loop
struct Sequences
{
    typedef std::list<std::shared_ptr<Sequencer>> SequencerList;
    static SequencerList physicsSequences; //sequences that run once per frame
    static SequencerList renderSequences; //sequences that run once per loop

    static void run(SequencerList& lst);

public:

    //isPhysics = true means we add to "physicsSequences", else "renderSequences"
    /**
     * @brief Waits until the provided function returns true
     * 
     * @param runThis function to wait on
     * @param isPhysics true if physics
     * @return SequencePtr 
     */
    static std::shared_ptr<Sequencer>&  add(bool isPhysics, Sequencer::Func func);
    static std::shared_ptr<Sequencer>&  add(bool isPhysics, Sequencer&& seq);
    static std::shared_ptr<Sequencer>&  add(bool isPhysics, std::shared_ptr<Sequencer>& seq);

    static void runPhysics();
    static void runRenders();
};

//this class represents a function taht may be called in the middle of the sequence, where its internal variables have to be dynamically set.
//a common use case is if you are lerping a camera position from point A to B, then B to C. After the A-B transition,  you need to update the camera start position for the 
//transition from B-C. This function lets you do that by passing in a function that returns the new camera position after A-B before doing the B-C transition
template<typename... T>
struct InitFunc
{
    //initialized values
    std::tuple<std::invoke_result_t<T>...> values;
    //the functions to initialize values
    std::tuple<T...> initializers;  
    //the actual function we will call
    std::function<bool(std::invoke_result_t<T>...,int)> func;

    InitFunc(decltype(func) func_,T... initializers_) :  initializers(std::make_tuple(initializers_...)), func(func_)
    {
        
    }

    bool operator()(int times)
    {
        //initialize our values the first time this is called.
        if(times == 0)
        {
            init();
        }
        //assembled all our initialized values and pass to the function
        return std::apply(func,std::tuple_cat(values,std::make_tuple(times))); 
    }
private:
    /**
     * @brief Initializes our values
     * 
     */
    void init()
    {
        std::apply([this](T... funcs){

            values = std::make_tuple(funcs()...);

        },initializers);
    }
};


#endif // SEQUENCER_H_INCLUDED
