
#include "RefCountingObject.h"
#include "RefCountingObjectPtr.h"

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <angelscript.h>

class Horse: public RefCountingObject<Horse>
{
public:
    void Neigh() { std::cout << COLOR_THEME_OBJ << this << ": neigh!"<< COLOR_RESET <<  std::endl; }
};

class Parrot: public RefCountingObject<Parrot>
{
public:
    void Chirp() { std::cout << COLOR_THEME_OBJ << this <<": chirp!"<< COLOR_RESET << std::endl; }
};

typedef RefCountingObjectPtr<Horse> HorsePtr;
typedef RefCountingObjectPtr<Parrot> ParrotPtr;

Horse* HorseFactory()
{
    Horse* obj = new Horse();
    obj->AddRef(); // This function is registered as "Horse@ f()" so we must manually increase refcount.
    return obj;
}

Parrot* ParrotFactory()
{
    return new Parrot(); // This function is registered as "Parrot@+ f()" (auto handle) so AngelScript will increase the refcount itself.
}

static HorsePtr g_stable;
static ParrotPtr g_aviary;

void PutToStable(HorsePtr horse)
{
    std::cout << COLOR_THEME_CPP << __FUNCTION__ << ": called with '" << horse.GetRef() << "'" << COLOR_RESET << std::endl;

    if (horse != nullptr && g_stable != nullptr)
    {
        std::cout << COLOR_THEME_CPP << __FUNCTION__ << ": occupied! discarding horse."<< COLOR_RESET << std::endl;
        return;
    }

    g_stable = horse;
}

HorsePtr FetchFromStable()
{
    std::cout << COLOR_THEME_CPP << __FUNCTION__ << " called" << COLOR_RESET << std::endl;

    return g_stable;
}

void PutToAviary(ParrotPtr parrot)
{
    std::cout << COLOR_THEME_CPP << __FUNCTION__ << " called with '" << parrot.GetRef() << "'" << COLOR_RESET<< std::endl;

    if (parrot != nullptr && g_aviary != nullptr)
    {
        std::cout << COLOR_THEME_CPP << "PutToAviary(): occupied! discarding parrot." << COLOR_RESET<< std::endl;
        return;
    }

    g_aviary = parrot;
}

ParrotPtr FetchFromAviary()
{
    std::cout << COLOR_THEME_CPP << __FUNCTION__ << " called" << COLOR_RESET << std::endl;

    return g_aviary;
}

HorsePtr ExampleCppFunctionCall(HorsePtr argPtr)
{
    std::cout << COLOR_THEME_CPP << __FUNCTION__ << " returning" << COLOR_RESET << std::endl;
    return argPtr;
}



void ExampleCpp(asIScriptEngine *engine)
{
    PrintString("ExampleCpp(): ^ global vars were constructed\n");

    int r;

    // -- Horse --
    // Registering the reference type
    Horse::RegisterRefCountingObject(engine, "Horse");
    r = engine->RegisterObjectMethod("Horse", "void Neigh()", asMETHOD(Horse, Neigh), asCALL_THISCALL); assert( r >= 0 );
    // Registering the factory behaviour (without 'auto handle' syntax, function must call AddRef()!)
    r = engine->RegisterObjectBehaviour("Horse", asBEHAVE_FACTORY, "Horse@ f()", asFUNCTION(HorseFactory), asCALL_CDECL); assert( r >= 0 );
    // Register handle type
    HorsePtr::RegisterRefCountingObjectPtr(engine, "HorsePtr", "Horse");
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToStable(HorsePtr@ h)", asFUNCTION(PutToStable), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("HorsePtr@ FetchFromStable()", asFUNCTION(FetchFromStable), asCALL_CDECL); assert( r >= 0 );

    // -- Parrot --
    // Registering the reference type
    Parrot::RegisterRefCountingObject(engine, "Parrot");
    r = engine->RegisterObjectMethod("Parrot", "void Chirp()", asMETHOD(Parrot, Chirp), asCALL_THISCALL); assert( r >= 0 );
    // Registering the factory behaviour (using 'auto handle' syntax)
    r = engine->RegisterObjectBehaviour("Parrot", asBEHAVE_FACTORY, "Parrot@+ f()", asFUNCTION(ParrotFactory), asCALL_CDECL); assert( r >= 0 );
    // Register handle type
    ParrotPtr::RegisterRefCountingObjectPtr(engine, "ParrotPtr", "Parrot");
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToAviary(ParrotPtr@ h)", asFUNCTION(PutToAviary), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("ParrotPtr@ FetchFromAviary()", asFUNCTION(FetchFromAviary), asCALL_CDECL); assert( r >= 0 );

    // Test horse interface from C++
    std::vector<HorsePtr> horses;

    PrintString("ExampleCpp(): construct\n");
    HorsePtr ptr1 = new Horse(); // "Artax"
    PrintString("ExampleCpp(): add ref\n");
    HorsePtr ptr2 = ptr1;
    PrintString("ExampleCpp: vector push\n");
    horses.push_back(ptr1);
    PrintString("ExampleCpp(): release ref\n");
    ptr2 = nullptr;
    PrintString("ExampleCpp(): vector pop\n");
    horses.pop_back();
    PrintString("ExampleCpp(): function call\n");
    ptr1 = ExampleCppFunctionCall(ptr1);
    PrintString("ExampleCpp(): put to stable\n");
    PutToStable(ptr1);
    PrintString("ExampleCpp(): delete local ref\n");
    ptr1 = nullptr;
    PrintString("ExampleCpp(): fetch from stable\n");
    ptr1 = FetchFromStable();
    PrintString("ExampleCpp(): dump from stable\n");
    PutToStable(nullptr);
    PrintString("ExampleCpp(): create second object, assign to existing null handle\n");
    ptr2 = new Horse(); // "Gunpowder"
    PrintString("ExampleCpp(): release ref\n");
    ptr2 = nullptr;
    PrintString("ExampleCpp(): 1 ref goes out of scope, object will be deleted\n");
}
