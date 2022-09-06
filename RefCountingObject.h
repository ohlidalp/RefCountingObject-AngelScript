
// RefCountingObject system for AngelScript
// Copyright (c) 2022 Petr Ohlidal
// https://github.com/only-a-ptr/RefCountingObject-AngelScript

#pragma once

#include <angelscript.h>
#include <cassert>

#if !defined(RefCoutingObject_DEBUGTRACE)
#   define RefCoutingObject_DEBUGTRACE
#endif

/// Self reference-counting objects, as requred by AngelScript garbage collector.
template<class T> class RefCountingObject
{
public:
    RefCountingObject()
    {
        RefCoutingObject_DEBUGTRACE();
    }

    virtual ~RefCountingObject()
    {
        RefCoutingObject_DEBUGTRACE();
    }

    void AddRef()
    {
        m_refcount++;
        RefCoutingObject_DEBUGTRACE();
    }

    void Release()
    {
        m_refcount--;
        RefCoutingObject_DEBUGTRACE();
        if (m_refcount == 0)
        {
            delete this; // commit suicide! This is legit in C++
        }
    }

    static void  RegisterRefCountingObject(const char* name, asIScriptEngine *engine)
    {
        int r;
        // Registering the reference type
        r = engine->RegisterObjectType(name, 0, asOBJ_REF); assert( r >= 0 );

        // Registering the addref/release behaviours
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(T,AddRef), asCALL_THISCALL); assert( r >= 0 );
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(T,Release), asCALL_THISCALL); assert( r >= 0 );
    }

    int m_refcount = 1; // Initial refcount for any angelscript object.
};
