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

#ifdef RCO_ENABLE_DEBUGTRACE
    RefCountingObject(std::string name)
    {
        m_name = name;
        RCO_DEBUGTRACE();
    }
#endif

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

    static asITypeInfo* GetTypeInfo()
    {
        return ms_type_info;
    }

    static void  RegisterRefCountingObject(const char* name, asIScriptEngine *engine)
    {
        int r;
        // Registering the reference type
        r = engine->RegisterObjectType(name, 0, asOBJ_REF); assert( r >= 0 );

        // Registering the addref/release behaviours
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(T,AddRef), asCALL_THISCALL); assert( r >= 0 );
        r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(T,Release), asCALL_THISCALL); assert( r >= 0 );

        // Obtain type info for constructing handles in C++ without active script context.
        RefCountingObject<T>::ms_type_info = engine->GetTypeInfoByName(name); assert(ms_type_info);
    }

#ifdef RCO_ENABLE_DEBUGTRACE
    std::string m_name;
#endif
    int m_refcount = 1; // Initial refcount for any angelscript object.
    static asITypeInfo *ms_type_info; // 'nullptr' means not registered with angelscript at all.
};

template<class T>
asITypeInfo* RefCountingObject<T>::ms_type_info = nullptr;
