#include "RefCountingObjectHandle.h"
#include "RefCountingObject.h"

#include "Testbed/debug_log.h"

#include <new>
#include <assert.h>
#include <string.h>

static void Construct(RefCountingObjectHandle *self) { new(self) RefCountingObjectHandle(); }
static void Construct(RefCountingObjectHandle *self, const RefCountingObjectHandle &o) { new(self) RefCountingObjectHandle(o); }
// This one is not static because it needs to be friend with the RefCountingObjectHandle class
void Construct(RefCountingObjectHandle *self, void *ref, int typeId) { new(self) RefCountingObjectHandle(ref, typeId); }
static void Destruct(RefCountingObjectHandle *self) { self->~RefCountingObjectHandle(); }

RefCountingObjectHandle::RefCountingObjectHandle()
{
    m_ref  = 0;
    m_type = 0;

    RCOH_DEBUGTRACE();
}

RefCountingObjectHandle::RefCountingObjectHandle(const RefCountingObjectHandle &other)
{
    m_ref  = other.m_ref;
    m_type = other.m_type;

    AddRefHandle();

    RCOH_DEBUGTRACE();
}

RefCountingObjectHandle::RefCountingObjectHandle(void *ref, asITypeInfo *type)
{
    m_ref  = ref;
    m_type = type;

    AddRefHandle();

    RCOH_DEBUGTRACE();
}

// This constructor shouldn't be called from the application 
// directly as it requires an active script context
RefCountingObjectHandle::RefCountingObjectHandle(void *ref, int typeId)
{
    m_ref  = 0;
    m_type = 0;

    RCOH_DEBUGTRACE();

    Assign(ref, typeId);
}

RefCountingObjectHandle::~RefCountingObjectHandle()
{
    RCOH_DEBUGTRACE();

    ReleaseHandle();
}

void RefCountingObjectHandle::ReleaseHandle()
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

void RefCountingObjectHandle::AddRefHandle()
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

RefCountingObjectHandle &RefCountingObjectHandle::operator =(const RefCountingObjectHandle &other)
{
    RCOH_DEBUGTRACE();

    Set(other.m_ref, other.m_type);

    return *this;
}

void RefCountingObjectHandle::Set(void *ref, asITypeInfo *type)
{
    if( m_ref == ref ) return;

    ReleaseHandle();

    m_ref  = ref;
    m_type = type;

    AddRefHandle();
}

void *RefCountingObjectHandle::GetRef()
{
    return m_ref;
}

asITypeInfo *RefCountingObjectHandle::GetType() const
{
    return m_type;
}

int RefCountingObjectHandle::GetTypeId() const
{
    if( m_type == 0 ) return 0;

    return m_type->GetTypeId() | asTYPEID_OBJHANDLE;
}

// This method shouldn't be called from the application 
// directly as it requires an active script context
RefCountingObjectHandle &RefCountingObjectHandle::Assign(void *ref, int typeId)
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

    // If the argument is another RefCountingObjectHandle, we should copy the content instead
    if( type && strcmp(type->GetName(), "RefCountingObjectHandle") == 0 )
    {
        RefCountingObjectHandle *r = (RefCountingObjectHandle*)ref;
        ref  = r->m_ref;
        type = r->m_type;
    }

    Set(ref, type);

    return *this;
}

bool RefCountingObjectHandle::operator==(const RefCountingObjectHandle &o) const
{
    if( m_ref  == o.m_ref &&
        m_type == o.m_type )
        return true;

    // TODO: If type is not the same, we should attempt to do a dynamic cast,
    //       which may change the pointer for application registered classes

    return false;
}

bool RefCountingObjectHandle::operator!=(const RefCountingObjectHandle &o) const
{
    return !(*this == o);
}

bool RefCountingObjectHandle::Equals(void *ref, int typeId) const
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
void RefCountingObjectHandle::Cast(void **outRef, int typeId)
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

void RefCountingObjectHandle::EnumReferences(asIScriptEngine *inEngine)
{
    // If we're holding a reference, we'll notify the garbage collector of it
    if (m_ref)
        inEngine->GCEnumCallback(m_ref);

    // The object type itself is also garbage collected
    if( m_type)
        inEngine->GCEnumCallback(m_type);
}

void RefCountingObjectHandle::ReleaseReferences(asIScriptEngine * /*inEngine*/)
{
    // Simply clear the content to release the references
    Set(0, 0);
}

void RegisterRefCountingObjectHandle(asIScriptEngine *engine)
{
    int r;

#if AS_CAN_USE_CPP11
    // With C++11 it is possible to use asGetTypeTraits to automatically determine the flags that represent the C++ class
    r = engine->RegisterObjectType("RefCountingObjectHandle", sizeof(RefCountingObjectHandle), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asGetTypeTraits<RefCountingObjectHandle>()); assert( r >= 0 );
#else
    r = engine->RegisterObjectType("RefCountingObjectHandle", sizeof(RefCountingObjectHandle), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asOBJ_APP_CLASS_CDAK); assert( r >= 0 );
#endif
    r = engine->RegisterObjectBehaviour("RefCountingObjectHandle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(Construct, (RefCountingObjectHandle *), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("RefCountingObjectHandle", asBEHAVE_CONSTRUCT, "void f(const RefCountingObjectHandle &in)", asFUNCTIONPR(Construct, (RefCountingObjectHandle *, const RefCountingObjectHandle &), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("RefCountingObjectHandle", asBEHAVE_CONSTRUCT, "void f(const ?&in)", asFUNCTIONPR(Construct, (RefCountingObjectHandle *, void *, int), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("RefCountingObjectHandle", asBEHAVE_DESTRUCT, "void f()", asFUNCTIONPR(Destruct, (RefCountingObjectHandle *), void), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("RefCountingObjectHandle", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(RefCountingObjectHandle,EnumReferences), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("RefCountingObjectHandle", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(RefCountingObjectHandle, ReleaseReferences), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RefCountingObjectHandle", "void opCast(?&out)", asMETHODPR(RefCountingObjectHandle, Cast, (void **, int), void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("RefCountingObjectHandle", "RefCountingObjectHandle &opHndlAssign(const RefCountingObjectHandle &in)", asMETHOD(RefCountingObjectHandle, operator=), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("RefCountingObjectHandle", "RefCountingObjectHandle &opHndlAssign(const ?&in)", asMETHOD(RefCountingObjectHandle, Assign), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("RefCountingObjectHandle", "bool opEquals(const RefCountingObjectHandle &in) const", asMETHODPR(RefCountingObjectHandle, operator==, (const RefCountingObjectHandle &) const, bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("RefCountingObjectHandle", "bool opEquals(const ?&in) const", asMETHODPR(RefCountingObjectHandle, Equals, (void*, int) const, bool), asCALL_THISCALL); assert( r >= 0 );
}



