#pragma once

#include "../RefCountingObject.h"
#include <angelscript.h>
#include <cassert>

class Dog: public RefCountingObject<Dog>
{
public:
    Dog(): RefCountingObject("Dog") {}

    ~Dog() {}
};

Dog* DogFactory()
{
    return new Dog();
}

void RegisterDog(asIScriptEngine *engine)
{
    int r;
    // Registering the reference type
    Dog::RegisterRefCountingObject("Dog", engine);
    // Registering the factory behaviour
    r = engine->RegisterObjectBehaviour("Dog", asBEHAVE_FACTORY, "Dog@ f()", asFUNCTION(DogFactory), asCALL_CDECL); assert( r >= 0 );
    
}
