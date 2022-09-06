
#include "RefCountingObject.h"
#include "RefCountingObjectPtr.h"
#include "HorseHandle.h"

#include <string>
#include <vector>
#include <cassert>
#include <angelscript.h>

// TEMPORARILY moved to HorseHandle.h
//  class Horse: public RefCountingObject<Horse>
//  {
//  public:
//      Horse(std::string name): RefCountingObject(name) {}
//      void Neigh() { std::cout << m_name << ": neigh!" << std::endl; }
//  };

class Parrot: public RefCountingObject<Parrot>
{
public:
    Parrot(): RefCountingObject("not-a-horse"){}
    void Idle() { std::cout << m_name << ": ..." <<std::endl; }
    void Chirp() { std::cout << m_name <<": chirp!" << std::endl; }
};

typedef RefCountingObjectPtr<Horse> HorsePtr;
typedef RefCountingObjectPtr<Parrot> ParrotPtr;

Horse* HorseFactory(std::string name)
{
    return new Horse(name);
}

Parrot* ParrotFactory()
{
    return new Parrot();
}

static HorsePtr g_stable;
static ParrotPtr g_aviary;

void PutToStable(HorsePtr horse)
{
    // TODO: check type
    std::cout << __FUNCTION__ << ": called with '" << (horse!=nullptr ? horse->m_name : "nullptr") << "'"  << std::endl;

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
    std::cout << __FUNCTION__ << " called"  << std::endl;

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

HorsePtr TestHorseFunctionCall(HorsePtr argPtr)
{
    std::cout << "TestHorseFunctionCall() returning" << std::endl;
    return argPtr;
}

// This will be a template in the future...
static void ConstructHorseHandleFromPtr(HorseHandle* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Assign()`.
    void* obj = *static_cast<void**>(objhandle);

    new(self)HorseHandle(static_cast<Horse*>(obj));
}

// This will be a template in the future...
static Horse* ImplicitCastHorseHandle(HorseHandle* self)
{
    Horse* h = static_cast<Horse*>(self->GetRef());
    h->AddRef();
    return h;
}

// This will be a template in the future...
static HorseHandle & HorseHandleOpAssign(HorseHandle* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Assign()`.
    void* obj = *static_cast<void**>(objhandle);

    self->Set(static_cast<Horse*>(obj));
    return *self;
}

// This will be a template in the future...
static bool EquineOpEquals(HorseHandle* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Equals()`.
    void* obj = *static_cast<void**>(objhandle);

    return self->GetRef() == static_cast<Horse*>(obj);
}

void ExampleCpp(asIScriptEngine *engine)
{
    RegisterRefCountingObjectHandle(engine);

    int r;

    // -- Horse --
    // Registering the reference type
    Horse::RegisterRefCountingObject("Horse", engine);
    r = engine->RegisterObjectMethod("Horse", "void Neigh()", asMETHOD(Horse, Neigh), asCALL_THISCALL); assert( r >= 0 );
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Horse", asBEHAVE_FACTORY, "Horse@ f(string name)", asFUNCTION(HorseFactory), asCALL_CDECL); assert( r >= 0 );
    // Register handle type
    RegisterHorseHandle(engine);
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f(Horse@&in)", asFUNCTION(ConstructHorseHandleFromPtr), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "Horse@ opImplCast()", asFUNCTION(ImplicitCastHorseHandle), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "HorseHandle &opHndlAssign(const Horse@&in)", asFUNCTION(HorseHandleOpAssign), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "bool opEquals(const Horse@&in) const", asFUNCTION(EquineOpEquals), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToStable(HorseHandle@ h)", asFUNCTION(PutToStable), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("HorseHandle@ FetchFromStable()", asFUNCTION(FetchFromStable), asCALL_CDECL); assert( r >= 0 );

    // -- Parrot --
    // Registering the reference type
    Parrot::RegisterRefCountingObject("Parrot", engine);
    r = engine->RegisterObjectMethod("Parrot", "void Idle()", asMETHOD(Parrot, Idle), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Parrot", "void Chirp()", asMETHOD(Parrot, Chirp), asCALL_THISCALL); assert( r >= 0 );
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Parrot", asBEHAVE_FACTORY, "Parrot@ f()", asFUNCTION(ParrotFactory), asCALL_CDECL); assert( r >= 0 );
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToAviary(RefCountingObjectHandle@ h)", asFUNCTION(PutToAviary), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("RefCountingObjectHandle@ FetchFromAviary()", asFUNCTION(FetchFromAviary), asCALL_CDECL); assert( r >= 0 );

    // Test horse interface from C++
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
    std::cout << "# TestHorse(): put to stable" << std::endl;
    PutToStable(ptr1);
    std::cout << "# TestHorse(): delete local ref" << std::endl;
    ptr1 = nullptr;
    std::cout << "# TestHorse(): fetch from stable" << std::endl;
    ptr1 = FetchFromStable();
    std::cout << "# TestHorse(): dump from stable" << std::endl;
    PutToStable(nullptr);
    std::cout << "# TestHorse(): 1 ref goes out of scope, object will be deleted" << std::endl;
}
