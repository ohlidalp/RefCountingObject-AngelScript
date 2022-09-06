
#include "RefCountingObject.h"
#include "RefCountingObjectPtr.h"

#include <string>
#include <vector>
#include <cassert>
#include <angelscript.h>

class Horse: public RefCountingObject<Horse>
{
public:
    void Neigh() { std::cout << this << ": neigh!" << std::endl; }
};

class Parrot: public RefCountingObject<Parrot>
{
public:
    void Idle() { std::cout << this << ": ..." <<std::endl; }
    void Chirp() { std::cout << this <<": chirp!" << std::endl; }
};

typedef RefCountingObjectPtr<Horse> HorsePtr;
typedef RefCountingObjectPtr<Parrot> ParrotPtr;

Horse* HorseFactory()
{
    return new Horse();
}

Parrot* ParrotFactory()
{
    return new Parrot();
}

static HorsePtr g_stable;
static ParrotPtr g_aviary;

void PutToStable(HorsePtr horse)
{
    std::cout << __FUNCTION__ << ": called with '" << horse.GetRef() << "'"  << std::endl;

    if (horse != nullptr && g_stable != nullptr)
    {
        std::cout << __FUNCTION__ << ": occupied! discarding horse." << std::endl;
        return;
    }

    g_stable = horse;
}

HorsePtr FetchFromStable()
{
    std::cout << __FUNCTION__ << " called"  << std::endl;

    return g_stable;
}

void PutToAviary(ParrotPtr parrot)
{
    std::cout << __FUNCTION__ << " called with '" << parrot.GetRef() << "'" << std::endl;

    if (parrot != nullptr && g_aviary != nullptr)
    {
        std::cout << "PutToAviary(): occupied! discarding parrot." << std::endl;
        return;
    }

    g_aviary = parrot;
}

ParrotPtr FetchFromAviary()
{
    std::cout << __FUNCTION__ << " called"  << std::endl;

    return g_aviary;
}

HorsePtr ExampleCppFunctionCall(HorsePtr argPtr)
{
    std::cout << "TestHorseFunctionCall() returning" << std::endl;
    return argPtr;
}



void ExampleCpp(asIScriptEngine *engine)
{
    int r;

    // -- Horse --
    // Registering the reference type
    Horse::RegisterRefCountingObject("Horse", engine);
    r = engine->RegisterObjectMethod("Horse", "void Neigh()", asMETHOD(Horse, Neigh), asCALL_THISCALL); assert( r >= 0 );
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Horse", asBEHAVE_FACTORY, "Horse@ f()", asFUNCTION(HorseFactory), asCALL_CDECL); assert( r >= 0 );
    // Register handle type
    HorsePtr::RegisterRefCountingObjectPtr("HorsePtr", "Horse", engine);
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToStable(HorsePtr@ h)", asFUNCTION(PutToStable), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("HorsePtr@ FetchFromStable()", asFUNCTION(FetchFromStable), asCALL_CDECL); assert( r >= 0 );

    // -- Parrot --
    // Registering the reference type
    Parrot::RegisterRefCountingObject("Parrot", engine);
    r = engine->RegisterObjectMethod("Parrot", "void Idle()", asMETHOD(Parrot, Idle), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Parrot", "void Chirp()", asMETHOD(Parrot, Chirp), asCALL_THISCALL); assert( r >= 0 );
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Parrot", asBEHAVE_FACTORY, "Parrot@ f()", asFUNCTION(ParrotFactory), asCALL_CDECL); assert( r >= 0 );
    // Register handle type
    HorsePtr::RegisterRefCountingObjectPtr("ParrotPtr", "Parrot", engine);
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToAviary(ParrotPtr@ h)", asFUNCTION(PutToAviary), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("ParrotPtr@ FetchFromAviary()", asFUNCTION(FetchFromAviary), asCALL_CDECL); assert( r >= 0 );

    // Test horse interface from C++
    std::vector<HorsePtr> horses;

    std::cout << "# ExampleCpp(): construct" << std::endl;
    HorsePtr ptr1 = HorseFactory(); // "Artax"
    std::cout << "# ExampleCpp(): add ref" << std::endl;
    HorsePtr ptr2 = ptr1;
    std::cout << "# ExampleCpp: vector push" << std::endl;
    horses.push_back(ptr1);
    std::cout << "# ExampleCpp(): release ref" << std::endl;
    ptr2 = nullptr;
    std::cout << "# ExampleCpp(): vector pop" << std::endl;
    horses.pop_back();
    std::cout << "# ExampleCpp(): function call" << std::endl;
    ptr1 = ExampleCppFunctionCall(ptr1);
    std::cout << "# ExampleCpp(): put to stable" << std::endl;
    PutToStable(ptr1);
    std::cout << "# ExampleCpp(): delete local ref" << std::endl;
    ptr1 = nullptr;
    std::cout << "# ExampleCpp(): fetch from stable" << std::endl;
    ptr1 = FetchFromStable();
    std::cout << "# ExampleCpp(): dump from stable" << std::endl;
    PutToStable(nullptr);
    std::cout << "# ExampleCpp(): create second object, assign to existing null handle" << std::endl;
    ptr2 = HorseFactory(); // "Gunpowder"
    std::cout << "# ExampleCpp(): release ref" << std::endl;
    ptr2 = nullptr;
    std::cout << "# ExampleCpp(): 1 ref goes out of scope, object will be deleted" << std::endl;
}
