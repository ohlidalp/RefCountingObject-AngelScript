#pragma once



#include <iostream>

#define RefCoutingObjectPtr_DEBUGTRACE(_arg_) {             \
    std::cout << __FUNCTION__ << " ref: (" << m_ref << ")"; \
    if (_arg_)                                              \
        std::cout << ", arg: (" << _arg_ << ")";            \
    std::cout << std::endl;                                 \
}



class DebugDummyClass {};

// internal
#define RCO_TRACE_ID_OBJ(_T_, _varname_) "'" << ((_varname_) ? static_cast<RefCountingObject<_T_>*>(_varname_)->m_name : "nullptr") << "'"
#define RCO_TRACE_ID_PTR(_T_, _varname_) "'" << ((_varname_.m_ref) ? static_cast<_T_*>(_varname_.m_ref)->m_name : "nullptr") << "'"
#define RCO_TRACE_ID_HORSEOBJ(_T_, _varname_) "'" << ((_varname_) ? reinterpret_cast<RefCountingObject<_T_>*>(_varname_)->m_name : "nullptr") << "'"

// global func
#define FUNC_DEBUGTRACE() {std::cout << __FUNCTION__ << std::endl;}

// RefCountingObjectHandle
#define RCOH_DEBUGTRACE() {std::cout << __FUNCTION__ << " " <<RCO_TRACE_ID_OBJ(DebugDummyClass, m_ref) << std::endl;}

// RefCountingObjectPtr
#define RCOP_DEBUGTRACE_SELF() {std::cout << __FUNCTION__ << " " << RCO_TRACE_ID_OBJ(T, m_ref) << "" << std::endl;}
#define RCOP_DEBUGTRACE_ARG_PTR(_argname_) {std::cout << __FUNCTION__ << " " << RCO_TRACE_ID_OBJ(T, m_ref) << ", arg:" << RCO_TRACE_ID_PTR(T, _argname_) << std::endl;}
#define RCOP_DEBUGTRACE_ARG_OBJ(_argname_) {std::cout << __FUNCTION__ << " " << RCO_TRACE_ID_OBJ(T, m_ref) << ", arg=" << RCO_TRACE_ID_OBJ(T, _argname_) << std::endl;}

// RefCountingObject
#define RCO_DEBUGTRACE() {std::cout << __FUNCTION__ << " '"<<m_name << "' new refcount:" << m_refcount << std::endl; }


