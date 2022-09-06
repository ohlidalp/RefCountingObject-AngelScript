
// RefCountingObject system for AngelScript
// Copyright (c) 2022 Petr Ohlidal
// https://github.com/only-a-ptr/RefCountingObject-AngelScript

#pragma once

#include "Testbed/debug_log.h"
#include <angelscript.h>
#include <cassert>

/// Self reference-counting objects, as requred by AngelScript garbage collector.
template<class T> class RefCountingObject
{
public:
    RefCountingObject()
    {
        RCO_DEBUGTRACE();
    }

    RefCountingObject(std::string name)
    {
        m_name = name;
        RCO_DEBUGTRACE();
    }

    virtual ~RefCountingObject()
    {
        RCO_DEBUGTRACE();
    }

    void AddRef()
    {
        m_refcount++;
        RCO_DEBUGTRACE();
    }

    void Release()
    {
        m_refcount--;
        RCO_DEBUGTRACE();
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

    std::string m_name;
    int m_refcount = 1; // Initial refcount for any angelscript object.
};
