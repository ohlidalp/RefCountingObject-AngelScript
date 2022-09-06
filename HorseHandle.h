
/// @file
/// @author Adopted from "scripthandle" AngelScript addon, distributed with AngelScript SDK.

#pragma once

#include <angelscript.h>
#include "RefCountingObject.h"

class Horse: public RefCountingObject<Horse>
{
public:
    Horse(std::string name): RefCountingObject(name) {}
    void Neigh() { std::cout << m_name << ": neigh!" << std::endl; }
};

class HorseHandle
{
public:
    // Constructors
    HorseHandle();
    HorseHandle(const HorseHandle &other);
    HorseHandle(Horse *ref);
    ~HorseHandle();

    // Copy the stored value from another any object
    HorseHandle &operator=(const HorseHandle &other);

    // Set the reference
    void Set(Horse* ref);

    // Compare equalness
    bool operator==(const HorseHandle &o) const;
    bool operator!=(const HorseHandle &o) const;

    // Get the reference
    void *GetRef();

    // GC callback
    void EnumReferences(asIScriptEngine *engine);
    void ReleaseReferences(asIScriptEngine *engine);

protected:
    friend void RegisterHorseHandle(asIScriptEngine *engine);

    void ReleaseHandle();
    void AddRefHandle();

    Horse        *m_ref;
};

void RegisterHorseHandle(asIScriptEngine *engine);

