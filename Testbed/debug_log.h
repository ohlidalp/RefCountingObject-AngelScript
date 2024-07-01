#pragma once


#include <iostream>
#include <string>

// Colors: https://www.codeproject.com/Tips/5255355/How-to-Put-Color-on-Windows-Console
#define COLOR_LIGHT_RED "\033[91m"
#define COLOR_LIGHT_GREEN "\033[92m"
#define COLOR_LIGHT_YELLOW "\033[93m"
#define COLOR_LIGHT_BLUE "\033[94m"
#define COLOR_LIGHT_MAGENTA "\033[95m"
#define COLOR_LIGHT_CYAN "\033[96m"
#define COLOR_WHITE "\033[97m"
#define COLOR_RESET "\033[0m"

// Theme
#define COLOR_THEME_MSG COLOR_LIGHT_YELLOW
#define COLOR_THEME_CPP COLOR_LIGHT_CYAN
#define COLOR_THEME_OBJ COLOR_LIGHT_MAGENTA
#define COLOR_THEME_MAIN COLOR_LIGHT_GREEN

#define RefCoutingObjectPtr_DEBUGTRACE(_arg_) {             \
    std::cout << __FUNCTION__ << " ref: (" << m_ref << ")"; \
    if (_arg_)                                              \
        std::cout << ", arg: (" << _arg_ << ")";            \
    std::cout << std::endl;                                 \
}

#define RefCoutingObjectPtr_DEBUGTRACE_STATIC(_self_, _arg_) {             \
    std::cout << __FUNCTION__ << " ref: (" << (_self_)->m_ref << ")"; \
    if (_arg_)                                              \
        std::cout << ", arg: (" << _arg_ << ")";            \
    std::cout << std::endl;                                 \
}

#define RefCoutingObject_DEBUGTRACE() {              \
    std::cout << __FUNCTION__ << " (" << this        \
        << ") refcount:" << m_refcount << std::endl; \
}


inline void PrintString(const std::string &str)
{
	std::cout << COLOR_THEME_MSG << str << COLOR_RESET;
}