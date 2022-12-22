
# RefCountingObject system for AngelScript

_Created 2022 by Petr Ohlídal_

_https://github.com/only-a-ptr/RefCountingObject-AngelScript_

This mini-framework lets you use a single reference counting mechanism for both
AngelScript's built-in garbage collection and C++ smart pointers. This allows you to fluently
pass objects between C++ and script context(s) without caring who created the object and where may
references be held.

## Motivation

I maintain [a game project](https://github.com/RigsOfRods/rigs-of-rods)
where AngelScript is an optional dependency
but at the same time plays an important gameplay role.
I wanted to expand the scripting interface by exposing
the internal objects and functions as directly as possible, but I didn't want to do
AngelScript refcounting manually on the C++ side.
I wanted a classic C++ smart pointer (like `std::shared_ptr<>`) that would do it for me,
and which would remain fully functional for C++ even if built without AngelScript.

## How to use

The project was developed against AngelScript 2.35.1; but any reasonably recent version should do.

To install, just copy the 'RefCountingObject\*' files to your project.

Define your C++ classes by implementing `RefCountingObject`
and calling `RegisterRefCountingObject()` for each type.
**Important:** Register the factory function as `"@+"` (auto handle)
to satisfy the script ref counting expectation, see "How it works" below.

```cpp
class Foo: RefCountingObject<Foo>{}
Foo::RegisterRefCountingObject(engine, "Foo");

static Foo* FooFactory() { return new Foo(); }
engine->RegisterObjectBehaviour("Foo", asBEHAVE_FACTORY, "Foo@+ f()", asFUNCTION(FooFactory), asCALL_CDECL);
```

Define your C++ smart pointers by qualifying `RefCountingObjectPtr<>`
and register each with `RegisterRefCountingObjectPtr()`.
and use them in your interfaces. 
These will become usable interchangeably from both C++ and script.

```cpp
typedef RefCountingObjectPtr<Foo> FooPtr;
FooPtr::RegisterRefCountingObjectPtr(engine, "FooPtr", "Foo");
```

Finally, use the smart pointers in your application interface.

```cpp
static FooPtr gf;

void SetFoo(FooPtr f) { gf = f; }
engine->RegisterGlobalFunction("void SetFoo(FooPtr@)", asFUNCTION(SetFoo), asCALL_CDECL);

FooPtr GetFoo() { return gf; }
engine->RegisterGlobalFunction("FooPtr@ GetFoo()", asFUNCTION(GetFoo), asCALL_CDECL);
```

In C++, use just the smart pointers and you'll be safe.

```cpp
FooPtr f1 = new Foo(); // refcount 1
SetFoo(f1);            // refcount 2
FooPtr f2 = GetFoo();  // refcount 3
f2 = nullptr;          // refcount 2
f1 = nullptr;          // refcount 1
SetFoo(nullptr);       // refcount 0 -> deleted.
```

In AngelScript, use the native handles.

```
Foo@ f1 = Foo();       // refcount 1
SetFoo(f1);            // refcount 2
Foo@ f2 = GetFoo();    // refcount 3
@f2 = null;            // refcount 2
@f1 = null;            // refcount 1
SetFoo(null);          // refcount 0 -> deleted.
```

## How it works

AngelScript automatically increases refcount when passing pointers to application
and expects the application to increase refcount when passing pointers to AngelScript.
See http://www.angelcode.com/angelscript/sdk/docs/manual/doc_obj_handle.html#doc_obj_handle_3

Across AngelScript documentation, the object constructor always pre-assigns refcount to 1,
but that would complicate using a smart pointer in C++. Thus, `RefCountingObject` leaves
the initial refcount at 0 and leaves it to either `RefCountingObjectPtr` (when constructing in C++)
or the user-defined factory function (when constructing in AngelScript) to increase it.
The factory function can either explicitly call `AddRef()`
or use the "auto handle" syntax `@+` which takes care of it automatically,
see http://www.angelcode.com/angelscript/sdk/docs/manual/doc_obj_handle.html#doc_obj_handle_4
  
