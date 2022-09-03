#include "HorseHandle.h"
#include "RefCountingObject.h"

#include "Testbed/debug_log.h"

#include <new>
#include <assert.h>
#include <string.h>

static void Construct(HorseHandle *self) { new(self) HorseHandle(); }
static void Construct(HorseHandle *self, const HorseHandle &o) { new(self) HorseHandle(o); }
// This one is not static because it needs to be friend with the HorseHandle class
void Construct(HorseHandle *self, void *ref, int typeId) { new(self) HorseHandle(ref, typeId); }
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

// This constructor shouldn't be called from the application 
// directly as it requires an active script context
HorseHandle::HorseHandle(void *ref, int typeId)
{
    m_ref  = 0;
    m_type = 0;

    RCOH_DEBUGTRACE();

    Assign(ref, typeId);
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

// This method shouldn't be called from the application 
// directly as it requires an active script context
HorseHandle &HorseHandle::Assign(void *ref, int typeId)
{
    // When receiving a null handle we just clear our memory
    if( typeId == 0 )
    {
        Set(0, 0);
        return *this;
    }

    // Dereference received handles to get the object
    if( typeId & asTYPEID_OBJHANDLE )
    {
        // Store the actual reference
        ref = *(void**)ref;
        typeId &= ~asTYPEID_OBJHANDLE;
    }

    // Get the object type
    asIScriptContext *ctx    = asGetActiveContext();
    asIScriptEngine  *engine = ctx->GetEngine();
    asITypeInfo      *type   = engine->GetTypeInfoById(typeId);

    // If the argument is another HorseHandle, we should copy the content instead
    if( type && strcmp(type->GetName(), "HorseHandle") == 0 )
    {
        HorseHandle *r = (HorseHandle*)ref;
        ref  = r->m_ref;
        type = r->m_type;
    }

    Set(ref, type);

    return *this;
}

bool HorseHandle::operator==(const HorseHandle &o) const
{
    if( m_ref  == o.m_ref &&
        m_type == o.m_type )
        return true;

    // TODO: If type is not the same, we should attempt to do a dynamic cast,
    //       which may change the pointer for application registered classes

    return false;
}

bool HorseHandle::operator!=(const HorseHandle &o) const
{
    return !(*this == o);
}

bool HorseHandle::Equals(void *ref, int typeId) const
{
    // Null handles are received as reference to a null handle
    if( typeId == 0 )
        ref = 0;

    // Dereference handles to get the object
    if( typeId & asTYPEID_OBJHANDLE )
    {
        // Compare the actual reference
        ref = *(void**)ref;
        typeId &= ~asTYPEID_OBJHANDLE;
    }

    // TODO: If typeId is not the same, we should attempt to do a dynamic cast, 
    //       which may change the pointer for application registered classes

    if( ref == m_ref ) return true;

    return false;
}

// AngelScript: used as '@obj = cast<obj>(ref);'
void HorseHandle::Cast(void **outRef, int typeId)
{
    // If we hold a null handle, then just return null
    if( m_type == 0 )
    {
        *outRef = 0;
        return;
    }
    
    // It is expected that the outRef is always a handle
    assert( typeId & asTYPEID_OBJHANDLE );

    // Compare the type id of the actual object
    typeId &= ~asTYPEID_OBJHANDLE;
    asIScriptEngine  *engine = m_type->GetEngine();
    asITypeInfo      *type   = engine->GetTypeInfoById(typeId);

    *outRef = 0;

    // RefCastObject will increment the refCount of the returned object if successful
    engine->RefCastObject(m_ref, m_type, type, outRef);
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

    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(Construct, (HorseHandle *), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f(const HorseHandle &in)", asFUNCTIONPR(Construct, (HorseHandle *, const HorseHandle &), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_CONSTRUCT, "void f(const ?&in)", asFUNCTIONPR(Construct, (HorseHandle *, void *, int), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_DESTRUCT, "void f()", asFUNCTIONPR(Destruct, (HorseHandle *), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(HorseHandle,EnumReferences), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("HorseHandle", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(HorseHandle, ReleaseReferences), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("HorseHandle", "void opCast(?&out)", asMETHODPR(HorseHandle, Cast, (void **, int), void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "HorseHandle &opHndlAssign(const HorseHandle &in)", asMETHOD(HorseHandle, operator=), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "HorseHandle &opHndlAssign(const ?&in)", asMETHOD(HorseHandle, Assign), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "bool opEquals(const HorseHandle &in) const", asMETHODPR(HorseHandle, operator==, (const HorseHandle &) const, bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HorseHandle", "bool opEquals(const ?&in) const", asMETHODPR(HorseHandle, Equals, (void*, int) const, bool), asCALL_THISCALL); assert( r >= 0 );
}



