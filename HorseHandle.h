
/// @file
/// @author Adopted from "scripthandle" AngelScript addon, distributed with AngelScript SDK.

#pragma once

#include <angelscript.h>
#include "RefCountingObject.h"


template<class T>
class HorseHandle
{
public:
    // Constructors
    HorseHandle();
    HorseHandle(const HorseHandle<T> &other);
    HorseHandle(T *ref); // Only invoke directly using C++! AngelScript must use a wrapper.
    ~HorseHandle();

    // Assignments
    HorseHandle &operator=(const HorseHandle<T> &other);
    // Intentionally omitting raw-pointer assignment, for simplicity - see raw pointer constructor.

    // Set the reference
    void Set(T* ref);

    // Compare equalness
    bool operator==(const HorseHandle<T> &o) const { return m_ref == o.m_ref; }
    bool operator!=(const HorseHandle<T> &o) const { return m_ref != o.m_ref; }

    // Get the reference
    T *GetRef() { return m_ref; }
    T* operator->() { return m_ref; }

    // GC callback
    void EnumReferences(asIScriptEngine *engine);
    void ReleaseReferences(asIScriptEngine *engine);

    static void RegisterHorseHandle(asIScriptEngine *engine);

protected:

    void ReleaseHandle();
    void AddRefHandle();

    // Wrapper functions, to be invoked by AngelScript only!
    static void ConstructDefault(HorseHandle<T> *self) { new(self) HorseHandle(); }
    static void ConstructCopy(HorseHandle<T> *self, const HorseHandle &o) { new(self) HorseHandle(o); }
    static void ConstructRef(HorseHandle<T>* self, void* objhandle);
    static void Destruct(HorseHandle<T> *self) { self->~HorseHandle(); }
    static T* OpImplCast(HorseHandle<T>* self);
    static HorseHandle & OpAssign(HorseHandle<T>* self, void* objhandle);
    static bool OpEquals(HorseHandle<T>* self, void* objhandle);

    T *m_ref;
};

template<class T>
void HorseHandle<T>::RegisterHorseHandle(asIScriptEngine *engine)
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

template<class T>
inline void HorseHandle<T>::ConstructRef(HorseHandle<T>* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Assign()`.
    void* obj = *static_cast<void**>(objhandle);
    T* ref = static_cast<T*>(obj);

    new(self)HorseHandle(ref);

    // Increase refcount manually because constructor is designed for C++ use only.
    if (ref)
        ref->AddRef();
}

template<class T>
inline T* HorseHandle<T>::OpImplCast(HorseHandle<T>* self)
{
    T* ref = self->GetRef();
    ref->AddRef();
    return ref;
}

template<class T>
inline HorseHandle<T> & HorseHandle<T>::OpAssign(HorseHandle<T>* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Assign()`.
    void* obj = *static_cast<void**>(objhandle);
    T* ref = static_cast<T*>(obj);

    self->Set(ref);
    return *self;
}

template<class T>
inline bool HorseHandle<T>::OpEquals(HorseHandle<T>* self, void* objhandle)
{
    // Dereference the handle to get the object itself.
    // See AngelScript SDK, addon 'generic handle', function `Equals()`.
    void* obj = *static_cast<void**>(objhandle);
    T* ref = static_cast<T*>(obj);

    return self->GetRef() == ref;
}

template<class T>
inline HorseHandle<T>::HorseHandle()
{
    m_ref  = 0;

    HORSEHANDLE_DEBUGTRACE();
}

template<class T>
inline HorseHandle<T>::HorseHandle(const HorseHandle<T> &other)
{
    m_ref  = other.m_ref;

    AddRefHandle();

    HORSEHANDLE_DEBUGTRACE();
}

template<class T>
inline HorseHandle<T>::HorseHandle(T *ref)
{
    // Used directly from C++, DO NOT increase refcount!
    // It's already been done by constructor/factory/AngelScript(if retrieved from script context).
    // See README.
    // ------------------------------------------

    m_ref  = ref;

    HORSEHANDLE_DEBUGTRACE();
}

template<class T>
inline HorseHandle<T>::~HorseHandle()
{
    HORSEHANDLE_DEBUGTRACE();

    ReleaseHandle();
}

template<class T>
inline void HorseHandle<T>::ReleaseHandle()
{
    HORSEHANDLE_DEBUGTRACE();

    if( m_ref )
    {
        m_ref->Release();
        m_ref = nullptr;
    }
}

template<class T>
inline void HorseHandle<T>::AddRefHandle()
{
    HORSEHANDLE_DEBUGTRACE();

    if( m_ref )
    {
        m_ref->AddRef();
    }
}

template<class T>
inline HorseHandle<T> &HorseHandle<T>::operator =(const HorseHandle<T> &other)
{
    HORSEHANDLE_DEBUGTRACE();

    Set(other.m_ref);

    return *this;
}

template<class T>
inline void HorseHandle<T>::Set(T* ref)
{
    if( m_ref == ref ) return;

    ReleaseHandle();

    m_ref  = ref;

    AddRefHandle();
}

template<class T>
inline void HorseHandle<T>::EnumReferences(asIScriptEngine *inEngine)
{
    // If we're holding a reference, we'll notify the garbage collector of it
    if (m_ref)
        inEngine->GCEnumCallback(m_ref);
}

template<class T>
inline void HorseHandle<T>::ReleaseReferences(asIScriptEngine * /*inEngine*/)
{
    // Simply clear the content to release the references
    Set(nullptr);
}

