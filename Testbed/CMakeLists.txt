#-------------------------------------------------------
# Testbed Main Build Script
#-------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})
cmake_minimum_required(VERSION 3.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


project(Testbed)

option(USE_CONAN "Use conan for installing deps" ON)

if (USE_CONAN)
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
    endif ()

    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_configure(REQUIRES angelscript/2.35.1 GENERATORS cmake_find_package)
    conan_cmake_autodetect(settings)
    conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
endif ()

find_package(Angelscript REQUIRED)

set(CMAKE_CXX_STANDARD 11)

file(COPY "${CMAKE_SOURCE_DIR}/../Example.as" DESTINATION "${CMAKE_BINARY_DIR}")

set(SRC
    ../RefCountingObject.h
    ../RefCountingObjectPtr.h
    debug_log.h
    scriptstdstring.h

    ../Example.cpp
    main.cpp
    scriptstdstring.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_compile_definitions(${PROJECT_NAME} PRIVATE RCO_ENABLE_DEBUGTRACE)
target_link_libraries(${PROJECT_NAME} PRIVATE Angelscript::angelscript)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Winmm)
endif()