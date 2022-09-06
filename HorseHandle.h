
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

    // Returns the type of the reference held
    asITypeInfo *GetType() const;
    int          GetTypeId() const;

    // Get the reference
    void *GetRef();

    // GC callback
    void EnumReferences(asIScriptEngine *engine);
    void ReleaseReferences(asIScriptEngine *engine);

protected:
    friend void RegisterHorseHandle(asIScriptEngine *engine);

    void ReleaseHandle();
    void AddRefHandle();

    void        *m_ref;
    asITypeInfo *m_type;
};

void RegisterHorseHandle(asIScriptEngine *engine);

