#pragma once

#include "../RefCountingObject.h"
#include "../RefCountingObjectPtr.h"
#include "../RefCountingObjectHandle.h"
#include <angelscript.h>
#include <cassert>
#include <vector>
#include <string>

class Horse: public RefCountingObject<Horse>
{
public:
    Horse(std::string name): RefCountingObject(name) {}

    ~Horse() {}

    void Neigh()
    {
        std::cout << "Neigh!" << std::endl;
    }
};

typedef RefCountingObjectPtr<Horse> HorsePtr;

static HorsePtr g_stable;

Horse* HorseFactory(std::string name)
{
    return new Horse(name);
}

void PutToStableInternal(HorsePtr horse)
{
    std::cout << "PutToStableInternal(): called with '" << (horse!=nullptr ? horse->m_name : "nullptr") << "'"  << std::endl;

    if (horse != nullptr && g_stable != nullptr)
    {
        std::cout << "PutToStableInternal(): occupied! discarding horse." << std::endl;
        return;
    }

    g_stable = horse;
    if (g_stable != nullptr)
    {
        g_stable->Neigh();
    }
}

void PutToStable(Horse* horse_raw)
{
    // refcount already increased by angelscript!
    // if we don't store the pointer, we must call Release()!
    //----------------------------------------------------
    HorsePtr horse(horse_raw);
    std::cout << "PutToStable(): called with '" << (horse!=nullptr ? horse->m_name : "nullptr") << "'"  << std::endl;

    PutToStableInternal(horse);
}

void PutToStableViaHandle(HorsePtr horse)
{
    // The Handle object (base of Ptr objects) takes care of increasing/decreasing refcount for the scope of this function.
    // If passing it onwards, the Ptr object's copy constructor will increase the refcount.
    // -------------------------------------------------

    // TODO: check type
    std::cout << __FUNCTION__ << ": called with '" << (horse!=nullptr ? horse->m_name : "nullptr") << "'"  << std::endl;

    PutToStableInternal(horse);
}

Horse* FetchFromStable()
{
    // when passing the object to AngelScript, we must increse refcount ourselves!
    // --------------------------------------------------------------------------
    std::cout << "FetchFromStable(): called"  << std::endl;
    if (g_stable.get())
    {
        g_stable.get()->AddRef();
    }
    return g_stable.get();
}

HorsePtr FetchFromStableViaHandle()
{
    // when passing the object to AngelScript, we must increse refcount ourselves!
    // --------------------------------------------------------------------------
    std::cout << __FUNCTION__ << " called"  << std::endl;

    return g_stable;
}


void RegisterHorse(asIScriptEngine *engine)
{
    int r;
    // Registering the reference type
    Horse::RegisterRefCountingObject("Horse", engine);
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Horse", asBEHAVE_FACTORY, "Horse@ f(string name)", asFUNCTION(HorseFactory), asCALL_CDECL); assert( r >= 0 );
    

    r = engine->RegisterGlobalFunction("void PutToStable(Horse@ h)", asFUNCTION(PutToStable), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void PutToStableViaHandle(RefCountingObjectHandle@ h)", asFUNCTION(PutToStableViaHandle), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Horse@ FetchFromStable()", asFUNCTION(FetchFromStable), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("RefCountingObjectHandle@ FetchFromStableViaHandle()", asFUNCTION(FetchFromStableViaHandle), asCALL_CDECL); assert( r >= 0 );
}

HorsePtr TestHorseFunctionCall(HorsePtr argPtr)
{
    std::cout << "TestHorseFunctionCall() returning" << std::endl;
    return argPtr;
}

void TestHorse()
{
    std::vector<HorsePtr> horses;

    std::cout << "# TestHorse(): construct" << std::endl;
    HorsePtr ptr1 = HorseFactory("Artax");
    std::cout << "# TestHorse(): add ref" << std::endl;
    HorsePtr ptr2 = ptr1;
    std::cout << "# TestHorse: vector push" << std::endl;
    horses.push_back(ptr1);
    std::cout << "# TestHorse(): release ref" << std::endl;
    ptr2 = nullptr;
    std::cout << "# TestHorse(): vector pop" << std::endl;
    horses.pop_back();
    std::cout << "# TestHorse(): function call" << std::endl;
    ptr1 = TestHorseFunctionCall(ptr1);
    std::cout << "# TestHorse(): delete" << std::endl;
    ptr1 = nullptr;
}


