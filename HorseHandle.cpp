#include "HorseHandle.h"
#include "RefCountingObject.h"

#include "Testbed/debug_log.h"

#include <new>
#include <assert.h>
#include <string.h>

static void Construct(HorseHandle *self) { new(self) HorseHandle(); }
static void Construct(HorseHandle *self, const HorseHandle &o) { new(self) HorseHandle(o); }
static void Destruct(HorseHandle *self) { self->~HorseHandle(); }

HorseHandle::HorseHandle()
{
    m_ref  = 0;
    m_type = 0;

    RCOH_DEBUGTRACE();
}

HorseHandle::HorseHandle(const HorseHandle &other)
{
    m_ref  = other.m_ref;
    m_type = other.m_type;

    AddRefHandle();

    RCOH_DEBUGTRACE();
}

HorseHandle::HorseHandle(void *ref, asITypeInfo *type)
{
    m_ref  = ref;
    m_type = type;

    AddRefHandle();

    RCOH_DEBUGTRACE();
}



HorseHandle::~HorseHandle()
{
    RCOH_DEBUGTRACE();

    ReleaseHandle();
}

void HorseHandle::ReleaseHandle()
{
    RCOH_DEBUGTRACE();

    if( m_ref && m_type )
    {
        asIScriptEngine *engine = m_type->GetEngine();
        engine->ReleaseScriptObject(m_ref, m_type);

        engine->Release();

        m_ref  = 0;
        m_type = 0;
    }
}

void HorseHandle::AddRefHandle()
{
    RCOH_DEBUGTRACE();

    if( m_ref && m_type )
    {
        asIScriptEngine *engine = m_type->GetEngine();
        engine->AddRefScriptObject(m_ref, m_type);

        // Hold on to the engine so it isn't destroyed while
        // a reference to a script object is still held
        engine->AddRef();
    }
}

HorseHandle &HorseHandle::operator =(const HorseHandle &other)
{
    RCOH_DEBUGTRACE();

    Set(other.m_ref, other.m_type);

    return *this;
}

void HorseHandle::Set(void *ref, asITypeInfo *type)
{
    if( m_ref == ref ) return;

    ReleaseHandle();

    m_ref  = ref;
    m_type = type;

    AddRefHandle();
}

void *HorseHandle::GetRef()
{
    return m_ref;
}

asITypeInfo *HorseHandle::GetType() const
{
    return m_type;
}

int HorseHandle::GetTypeId() const
{
    if( m_type == 0 ) return 0;

    return m_type->GetTypeId() | asTYPEID_OBJHANDLE;
}

bool HorseHandle::operator==(const HorseHandle &o) const
{
    if( m_ref  == o.m_ref &&
        m_type == o.m_type )
        return true;

    return false;
}

bool HorseHandle::operator!=(const HorseHandle &o) const
{
    return !(*this == o);
}

void HorseHandle::EnumReferences(asIScriptEngine *inEngine)
{
    // If we're holding a reference, we'll notify the garbage collector of it
    if (m_ref)
        inEngine->GCEnumCallback(m_ref);

    // The object type itself is also garbage collected
    if( m_type)
        inEngine->GCEnumCallback(m_type);
}

void HorseHandle::ReleaseReferences(asIScriptEngine * /*inEngine*/)
{
    // Simply clear the content to release the references
    Set(0, 0);
}

void RegisterHorseHandle(asIScriptEngine *engine)
{
    int r;

    // With C++11 it is possible to use asGetTypeTraits to automatically determine the flags that represent the C++ class
    r = engine->RegisterObjectType("HorseHandle", sizeof(HorseHandle), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asGetTypeTraits<HorseHandle>()); assert( r >= 0 );
    // construct/destruct
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(Construct, (HorseHandle *), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f(const HorseHandle &in)", asFUNCTIONPR(Construct, (HorseHandle *, const HorseHandle &), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_DESTRUCT, "void f()", asFUNCTIONPR(Destruct, (HorseHandle *), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    // GC
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(HorseHandle,EnumReferences), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(HorseHandle, ReleaseReferences), asCALL_THISCALL); assert(r >= 0);
    // Assign
    r = engine->RegisterObjectMethod("HorseHandle", "HorseHandle &opHndlAssign(const HorseHandle &in)", asMETHOD(HorseHandle, operator=), asCALL_THISCALL); assert( r >= 0 );
    // Equals
    r = engine->RegisterObjectMethod("HorseHandle", "bool opEquals(const HorseHandle &in) const", asMETHODPR(HorseHandle, operator==, (const HorseHandle &) const, bool), asCALL_THISCALL); assert( r >= 0 );
}



