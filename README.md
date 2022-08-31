
# RefCountingObject system for AngelScript

_Created by Petr Ohlidal_

This mini-framework lets you use a single reference counting mechanism for both
AngelScript's built-in garbage collection and C++ smart pointers. This allows you to fluently
pass objects from/to script context(s) without caring who created the object and where may
references be held.

## Motivation

I maintain [a game project](www.rigsofrods.org) where AngelScript is an optional dependency
but at the same time plays an important gameplay role.
I wanted to expand the scripting interface by exposing
the internal objects and interfaces as directly as possible, but I didn't want to do
AngelScript refcounting manually on the C++ side.
I wanted a classic C++ smart pointer (like `std::shared_ptr<>`) that would do it for me,
and which would remain fully functional for C++ even if built without AngelScript.

## How to use

To install, just copy the "gc" subdirectory to your project.

Define your C++ classes by implementing `RefCountingObject`
and calling `RegisterRefCountingObject()` for each type.

```
class Foo: RefCountingObject<Foo>{}
Foo::RegisterRefCountingObject("Foo", engine);
```

Define your C++ smart pointers by qualifying `RefCountingObjectPtr<>`
and use them in your interfaces. 
These will become usable interchangeably from both C++ and script.

```
typedef RefCountingObjectPtr<Foo> FooPtr;
// demo API:
static FooPtr gf;
static void SetFoo(FooPtr f) { gf = f; }
static FooPtr GetFoo() { return gf; }
```

Register `RefCountingObjectHandle` with the script engine
and use it to register your interface.

```
RegisterRefCountingObjectHandle(engine);
// ...
engine->RegisterGlobalFunction("void SetFoo(RefCountingObjectHandle@)", asFUNCTION(SetFoo), asCALL_CDECL);
engine->RegisterGlobalFunction("RefCountingObjectHandle@ GetFoo()", asFUNCTION(GetFoo), asCALL_CDECL);
```

In C++, use just the smart pointers and you'll be safe.

```
FooPtr f1 = new Foo(); // refcount 1
SetFoo(f1);            // refcount 2
FooPtr f2 = GetFoo();  // refcount 3
f2 = nullptr;          // refcount 2
f1 = nullptr;          // refcount 1
SetFoo(nullptr);       // refcount 0 -> deleted.
```

In AngelScript, use the native handles.
Currently a cast is needed for the getter function, but I'm working on it.

```
Foo@ f1 = new Foo();            // refcount 1
SetFoo(f1);                     // refcount 2
Foo@ f2 = cast<Foo>(GetFoo());  // refcount 3
@f2 = null;                     // refcount 2
@f1 = null;                     // refcount 1
SetFoo(null);                   // refcount 0 -> deleted.
```

## How it works

Weakly sorted notes from learning AngelScript mechanics and designing this library.

### How the AngelScript refcounting works

 Intuitively, you would call AddRef() every time you obtain a raw pointer to the object, 
 i.e. when creating a new object or retrieving one from script engine. Well, don't.
 
 * New objects have refcount initialized to 1.
   You must call Release() to dispose of it, but don't call AddRef() unless you're creating additional pointers.
 * When passing the object as parameter from script to C++ function, the refcount is already incremented by the script engine.
   If you don't store the pointer, you must call Release() before the function returns.
 * When passing the object from C++ to script, you must increase the refcount yourself.
 
Documentation: 
   https://www.angelcode.com/angelscript/sdk/docs/manual/doc_as_vs_cpp_types.html#doc_as_vs_cpp_types_3
   https://www.angelcode.com/angelscript/sdk/docs/manual/doc_obj_handle.html#doc_obj_handle_3
   
### How RefCountingObject fits into it

xPtr can only be constructed in C++, it's never registered with AngelScript. Since RefCountingObject
is constructed with refcount=1, the xPtr doesn't increase the refcount when initialized/assigned.

Using xHandle in the registered function signatures relies on xHandle and xPtr being binary-compatible,
so the AngelScript engine type-puns the xHandle to/from xPtr.
This probably relies on UB (=undefined behavior) somehow, but it's too convenient to dismiss.
  