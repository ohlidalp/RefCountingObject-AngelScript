
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
    HorseHandle(Horse *ref); // Only invoke directly using C++! AngelScript must use a wrapper.
    ~HorseHandle();

    // Assignments
    HorseHandle &operator=(const HorseHandle &other);
    // Intentionally omitting raw-pointer assignment, for simplicity - see raw pointer constructor.

    // Set the reference
    void Set(Horse* ref);

    // Compare equalness
    bool operator==(const HorseHandle &o) const;
    bool operator!=(const HorseHandle &o) const;

    // Get the reference
    void *GetRef();
    Horse* operator->() { return m_ref; }

    // GC callback
    void EnumReferences(asIScriptEngine *engine);
    void ReleaseReferences(asIScriptEngine *engine);

    static void RegisterHorseHandle(asIScriptEngine *engine);

protected:

    void ReleaseHandle();
    void AddRefHandle();

    // Wrapper functions, to be invoked by AngelScript only!
    static void ConstructDefault(HorseHandle *self) { new(self) HorseHandle(); }
    static void ConstructCopy(HorseHandle *self, const HorseHandle &o) { new(self) HorseHandle(o); }
    static void ConstructRef(HorseHandle* self, void* objhandle);
    static void Destruct(HorseHandle *self) { self->~HorseHandle(); }
    static Horse* OpImplCast(HorseHandle* self);
    static HorseHandle & OpAssign(HorseHandle* self, void* objhandle);
    static bool OpEquals(HorseHandle* self, void* objhandle);

    Horse        *m_ref;
};

void HorseHandle::RegisterHorseHandle(asIScriptEngine *engine)
{
    int r;

    // With C++11 it is possible to use asGetTypeTraits to automatically determine the flags that represent the C++ class
    r = engine->RegisterObjectType("HorseHandle", sizeof(HorseHandle), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asGetTypeTraits<HorseHandle>()); assert( r >= 0 );
    // construct/destruct
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(HorseHandle::ConstructDefault), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f(Horse@&in)", asFUNCTION(HorseHandle::ConstructRef), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f(const HorseHandle &in)", asFUNCTION(HorseHandle::ConstructCopy), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(HorseHandle::Destruct), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    // GC
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(HorseHandle,EnumReferences), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(HorseHandle, ReleaseReferences), asCALL_THISCALL); assert(r >= 0);
    // Cast
    r = engine->RegisterObjectMethod("HorseHandle", "Horse@ opImplCast()", asFUNCTION(HorseHandle::OpImplCast), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    // Assign
    r = engine->RegisterObjectMethod("HorseHandle", "HorseHandle &opHndlAssign(const HorseHandle &in)", asMETHOD(HorseHandle, operator=), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "HorseHandle &opHndlAssign(const Horse@&in)", asFUNCTION(HorseHandle::OpAssign), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    // Equals
    r = engine->RegisterObjectMethod("HorseHandle", "bool opEquals(const HorseHandle &in) const", asMETHODPR(HorseHandle, operator==, (const HorseHandle &) const, bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "bool opEquals(const Horse@&in) const", asFUNCTION(HorseHandle::OpEquals), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
}


// ---------------------------- Internals ------------------------------


inline void HorseHandle::ConstructRef(HorseHandle* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Assign()`.
    void* obj = *static_cast<void**>(objhandle);
    Horse* ref = static_cast<Horse*>(obj);

    new(self)HorseHandle(ref);

    // Increase refcount manually because constructor is designed for C++ use only.
    if (ref)
        ref->AddRef();
}

inline Horse* HorseHandle::OpImplCast(HorseHandle* self)
{
    Horse* ref = static_cast<Horse*>(self->GetRef());
    ref->AddRef();
    return ref;
}

inline HorseHandle & HorseHandle::OpAssign(HorseHandle* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Assign()`.
    void* obj = *static_cast<void**>(objhandle);
    Horse* ref = static_cast<Horse*>(obj);

    self->Set(ref);
    return *self;
}

inline bool HorseHandle::OpEquals(HorseHandle* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Equals()`.
    void* obj = *static_cast<void**>(objhandle);
    Horse* ref = static_cast<Horse*>(obj);

    return self->GetRef() == ref;
}

inline HorseHandle::HorseHandle()
{
    m_ref  = 0;

    HORSEHANDLE_DEBUGTRACE();
}

inline HorseHandle::HorseHandle(const HorseHandle &other)
{
    m_ref  = other.m_ref;

    AddRefHandle();

    HORSEHANDLE_DEBUGTRACE();
}

inline HorseHandle::HorseHandle(Horse *ref)
{
    // Used directly from C++, DO NOT increase refcount!
    // It's already been done by constructor/factory/AngelScript(if retrieved from script context).
    // See README.
    // ------------------------------------------

    m_ref  = ref;

    HORSEHANDLE_DEBUGTRACE();
}

inline HorseHandle::~HorseHandle()
{
    HORSEHANDLE_DEBUGTRACE();

    ReleaseHandle();
}

inline void HorseHandle::ReleaseHandle()
{
    HORSEHANDLE_DEBUGTRACE();

    if( m_ref )
    {
        m_ref->Release();
        m_ref = nullptr;
    }
}

inline void HorseHandle::AddRefHandle()
{
    HORSEHANDLE_DEBUGTRACE();

    if( m_ref )
    {
        m_ref->AddRef();
    }
}

inline HorseHandle &HorseHandle::operator =(const HorseHandle &other)
{
    HORSEHANDLE_DEBUGTRACE();

    Set(other.m_ref);

    return *this;
}

inline void HorseHandle::Set(Horse* ref)
{
    if( m_ref == ref ) return;

    ReleaseHandle();

    m_ref  = ref;

    AddRefHandle();
}

inline void *HorseHandle::GetRef()
{
    return m_ref;
}

inline bool HorseHandle::operator==(const HorseHandle &o) const
{
    return m_ref  == o.m_ref;
}

inline bool HorseHandle::operator!=(const HorseHandle &o) const
{
    return !(*this == o);
}

inline void HorseHandle::EnumReferences(asIScriptEngine *inEngine)
{
    // If we're holding a reference, we'll notify the garbage collector of it
    if (m_ref)
        inEngine->GCEnumCallback(m_ref);
}

inline void HorseHandle::ReleaseReferences(asIScriptEngine * /*inEngine*/)
{
    // Simply clear the content to release the references
    Set(nullptr);
}

