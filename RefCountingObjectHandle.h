
/// @file
/// @author Adopted from "scripthandle" AngelScript addon, distributed with AngelScript SDK.

#pragma once

#include <angelscript.h>

class RefCountingObjectHandle
{
public:
    // Constructors
    RefCountingObjectHandle();
    RefCountingObjectHandle(const RefCountingObjectHandle &other);
    RefCountingObjectHandle(void *ref, asITypeInfo *type);
    ~RefCountingObjectHandle();

    // Copy the stored value from another any object
    RefCountingObjectHandle &operator=(const RefCountingObjectHandle &other);

    // Set the reference
    void Set(void *ref, asITypeInfo *type);

    // Compare equalness
    bool operator==(const RefCountingObjectHandle &o) const;
    bool operator!=(const RefCountingObjectHandle &o) const;
    bool Equals(void *ref, int typeId) const;

    // Dynamic cast to desired handle type
    void Cast(void **outRef, int typeId);

    // Returns the type of the reference held
    asITypeInfo *GetType() const;
    int          GetTypeId() const;

    // Get the reference
    void *GetRef();

    // GC callback
    void EnumReferences(asIScriptEngine *engine);
    void ReleaseReferences(asIScriptEngine *engine);

protected:
    // These functions need to have access to protected
    // members in order to call them from the script engine
    friend void Construct(RefCountingObjectHandle *self, void *ref, int typeId);
    friend void RegisterRefCountingObjectHandle(asIScriptEngine *engine);

    void ReleaseHandle();
    void AddRefHandle();

    // These shouldn't be called directly by the 
    // application as they requires an active context
    RefCountingObjectHandle(void *ref, int typeId);
    RefCountingObjectHandle &Assign(void *ref, int typeId);

    void        *m_ref;
    asITypeInfo *m_type;
};

void RegisterRefCountingObjectHandle(asIScriptEngine *engine);

