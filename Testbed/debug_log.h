#pragma once


#include <iostream>

#define RefCoutingObjectPtr_DEBUGTRACE(_arg_) {             \
    std::cout << __FUNCTION__ << " ref: (" << m_ref << ")"; \
    if (_arg_)                                              \
        std::cout << ", arg: (" << _arg_ << ")";            \
    std::cout << std::endl;                                 \
}

#define RefCoutingObject_DEBUGTRACE() {              \
    std::cout << __FUNCTION__ << " (" << this        \
        << ") refcount:" << m_refcount << std::endl; \
}


