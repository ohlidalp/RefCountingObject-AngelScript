
#include "RefCountingObject.h"
#include "RefCountingObjectPtr.h"

#include <string>
#include <vector>
#include <cassert>
#include <angelscript.h>

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

Horse* HorseFactory(std::string name)
{
    return new Horse(name);
}

static HorsePtr g_stable;

void PutToStableViaHandle(HorsePtr horse)
{
    // TODO: check type
    std::cout << __FUNCTION__ << ": called with '" << (horse!=nullptr ? horse->m_name : "nullptr") << "'"  << std::endl;

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

HorsePtr FetchFromStableViaHandle()
{
    std::cout << __FUNCTION__ << " called"  << std::endl;

    return g_stable;
}

HorsePtr TestHorseFunctionCall(HorsePtr argPtr)
{
    std::cout << "TestHorseFunctionCall() returning" << std::endl;
    return argPtr;
}

void ExampleCpp(asIScriptEngine *engine)
{
    RegisterRefCountingObjectHandle(engine);

    int r;
    // Registering the reference type
    Horse::RegisterRefCountingObject("Horse", engine);
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Horse", asBEHAVE_FACTORY, "Horse@ f(string name)", asFUNCTION(HorseFactory), asCALL_CDECL); assert( r >= 0 );
    // Registering example interface
    r = engine->RegisterGlobalFunction("void PutToStableViaHandle(RefCountingObjectHandle@ h)", asFUNCTION(PutToStableViaHandle), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("RefCountingObjectHandle@ FetchFromStableViaHandle()", asFUNCTION(FetchFromStableViaHandle), asCALL_CDECL); assert( r >= 0 );

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
    PutToStableViaHandle(ptr1);
    std::cout << "# TestHorse(): delete local ref" << std::endl;
    ptr1 = nullptr;
    std::cout << "# TestHorse(): fetch from stable" << std::endl;
    ptr1 = FetchFromStableViaHandle();
    std::cout << "# TestHorse(): dump from stable" << std::endl;
    PutToStableViaHandle(nullptr);
    std::cout << "# TestHorse(): delete local instance" << std::endl;
    ptr1 = nullptr;
}
