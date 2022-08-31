#pragma once

#include "RefCountingObject.h"
#include "RefCountingObjectHandle.h"
#include "Testbed/debug_log.h"

template <class T> class RefCountingObjectPtr: public RefCountingObjectHandle
{
public:
    RefCountingObjectPtr() {} // null handle

    RefCountingObjectPtr(T* obj)
    {
        // IMPORTANT: Do not increase refcount!
        // already done by constructor/factory/AngelScript(if retrieved from script context).
        // See README.
        // ------------------------------------------
        RCOP_DEBUGTRACE_SELF();
        m_ref = obj;
        m_type = RefCountingObject<T>::GetTypeInfo();
    }

    RefCountingObjectPtr(RefCountingObjectPtr const& orig)
    {
        RCOP_DEBUGTRACE_ARG_PTR(orig);
        this->assign(orig);
    }

    ~RefCountingObjectPtr() { this->reset(); }

    RefCountingObjectPtr& operator=(RefCountingObjectPtr const& rhs)
    {
        RCOP_DEBUGTRACE_ARG_PTR(rhs);
        this->assign(rhs);
        return *this;
    }

    RefCountingObjectPtr& operator=(T* obj)
    {
        // DO NOT increase refcount!
        // already done by constructor/factory/AngelScript(if retrieved from script context).
        // See README.
        // ------------------------------------------
        RCOP_DEBUGTRACE_ARG_OBJ(obj);
        this->reset();
        m_ref = obj;
        m_type = RefCountingObject<T>::GetTypeInfo();
        return *this;
    }

    T* operator->() { return static_cast<T*>(m_ref); }

    T* get() { return static_cast<T*>(m_ref); }

    bool operator==(nullptr_t) { return m_ref == nullptr; }
    bool operator!=(nullptr_t) { return m_ref != nullptr; }

    operator bool() { return m_ref != nullptr; }

private:
    void assign(RefCountingObjectPtr const& orig)
    {
        this->reset();
        m_ref = orig.m_ref;
        if (m_ref)
        {
            static_cast<T*>(m_ref)->AddRef();
            m_type = RefCountingObject<T>::GetTypeInfo();
        }
    }
  
    void reset()
    {
        // `m_type` can legitimately be always null, if the RefCountingObject was never registered with AngelScript.
        if (m_ref)
        { 
            static_cast<T*>(m_ref)->Release();
            m_ref = nullptr;
            m_type = nullptr;
        }
    }
};


