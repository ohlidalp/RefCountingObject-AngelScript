
/// @file
/// @author Adopted from "scripthandle" AngelScript addon, distributed with AngelScript SDK.

#pragma once

#include <angelscript.h>

class HorseHandle
{
public:
    // Constructors
    HorseHandle();
    HorseHandle(const HorseHandle &other);
    HorseHandle(void *ref, asITypeInfo *type);
    ~HorseHandle();

    // Copy the stored value from another any object
    HorseHandle &operator=(const HorseHandle &other);

    // Set the reference
    void Set(void *ref, asITypeInfo *type);

    // Compare equalness
    bool operator==(const HorseHandle &o) const;
    bool operator!=(const HorseHandle &o) const;
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
    friend void Construct(HorseHandle *self, void *ref, int typeId);
    friend void RegisterHorseHandle(asIScriptEngine *engine);

    void ReleaseHandle();
    void AddRefHandle();

    // These shouldn't be called directly by the 
    // application as they requires an active context
    HorseHandle(void *ref, int typeId);
    HorseHandle &Assign(void *ref, int typeId);

    void        *m_ref;
    asITypeInfo *m_type;
};

void RegisterHorseHandle(asIScriptEngine *engine);

