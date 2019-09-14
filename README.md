**CppRevEng** is a project aimed at making C++ reverse engineering and DLL injection easier. It's main focus is to provide an easier interface to use [Microsoft's Detours API](https://github.com/microsoft/Detours), which is used to alter the code of a running program.

This library (which is actually just two header files) was created for injecting and detouring Visual C++ x86 programs. If you want to use this library, just add the `inc` folder into your include path, and include the file `<CppRevEng.h>` . Even though one of its main focus is detouring, this does not include the `Detours` API; you must download it and include it in your project for `CppRevEng` to work correctly.

## Projects that use CppRevEng

 - [Spore ModAPI](https://github.com/Emd4600/Spore-ModAPI) (uses slightly modified version)
 - [Spore ModCreatorKit](https://github.com/Emd4600/Spore-ModCreatorKit) (uses slightly modified version)

## Addresses

The first thing `CppRevEng` helps you with is with organizing addresses. For small projects this is generally unnecessary, but if
you want to do a big project (like an API to mod a game) you will want to keep some order.

The most simple macro is `Address()`, which just takes an address you have found reverse engineering (with IDA, Ghidra or similars)
and converts it into the correct address of the current executable running. `Address(0x4056d0)`, as simple as that.
* Be warned, programs like Cheat Engine or OllyDBG treat base addresses differently, so you might need to add `0x400000` to the 
addresses you get from those programs.

And then comes organization: it is recommended to store addresses in namespaces. Imagine you have reverse engineered a class, `cViewer`,
with two methods: `LoadTransformations` and `SetBackgroundColor`. You can then store their addresses in a list:

```cpp
class cViewer { ... };

namespace Addresses(cViewer) {
	DeclareAddress(LoadTransformations, 0x7C5650);
	DeclareAddress(SetBackgroundColor, 0x7C42C0);
}
```

Now you can use `GetAddress()` to get those addresses, e.g. `GetAddress(cViewer, LoadTransformations)`.

In your `dllmain.cpp`, you should add he following line just after the includes so that it can compile correctly:

```cpp
BaseAddressDeclare;
```

Then in your `DllMain` function, you should call the `InitCppRevEng()` function in order to correctly initialize the base address variable.

## Method Redirecting

When doing DLL injection, you will probably need to call a function of the executable, which is at a certain address. Normally, this would be done by making a function pointer type, casting the function address to that type and then call it. Since this is very counter-intuitive, `CppRevEng` includes several macros to help with that.

Our preferred solution is declaring classes and methods in header files, as if it were an API. Then, do the implementation on a .cpp file (you can also do it in the .h, inlined). In order to generate the implementation for these kind of functions, we have the `Redirect...Method` macros. The macros are designed to work combined with the address lists we mentioned before.

In general, all the macros take:
 - The address namespace name (or class name if it's a member method).
 - The function name.
 - The return type.
 - The parameters.
 - The parameter names (they can be generic, don't need to match the header file).

There are three main macros:
 - `RedirectStaticMethod`: used for **static** and namespace functions.
 - `RedirectMethod`: used for non-virtual, **non-static** class methods.
 - `RedirectVirtualMethod`: used for **virtual**, non-static class methods. This one takes an extra parameter, the virtual class.

These macros have multiple variants (which can be combined):
 - `..._noargs`: if the method does not take any parameters.
 - `..._structret`: if the method returns a struct by value (exactly, if sizeof() > 8)
 - `..._const`: if the method is declared as `const` (cannot be used in static functions).

Some examples:

```cpp
namespace Spore {
    void DestroyObject(int index);
}
RedirectStaticMethod(Spore, DestroyObject, void, Args(int index), Args(index));

class  PhysicsEngine {
    static float GetGravity();
}
RedirectStaticMethod_noargs(PhysicsEngine, GetGravity, float);

namespace Math {
    Vector3 Normalize(const Vector3& v);
    Vector3 GetZ();
}
RedirectStaticMethod_structret(Math, Normalize, Vector3, Args(const Vector3& v), Args(v));
RedirectStaticMethod_structret(Math, GetZ, Vector3);

class Object {
    uint32_t GetId();
    uint32_t GetId() const;
}
RedirectMethod_noargs(Object, GetId, uint32_t);
RedirectMethod_noargs_const(Object, GetId, uint32_t);

class Window {
    void AddChild(int index);
    virtual bool Paint(DrawSettings& drawer)
}
RedirectMethod(Window, AddChild, void, Args(int index), Args(index));
RedirectVirtualMethod(Window, Paint, bool, Args(DrawSettings& drawer), Args(drawer));

struct Vector3 {
    float Scale(float factor);
}
RedirectMethod_const(Vector3, Scale, float, Args(float factor), Args(factor));
```

## Detouring

When using DLL injection, detouring is the most important technique: it allows you to hook the executable methods and override them with your own functionality. Generally, detouring a program is quite complicated. Not because it's hard, but because it requires many repeated code that could be avoided: that's what `CppRevEng` does.

This header file defines 3 macros:
 - `static_detour`: for static functions.
 - `member_detour`: for non-static, non-virtual methods.
 - `virtual_detour`: for non-static, virtual methods.

These macros are struct declarations, so you must follow them with a {};  - don't forget the final semicolon!

All macros take at least two parameters: 
 - `name`: A unique name to identify the detour object. You will use it to attach the detour.
 - `declaration`: The type and parameter types of the function. Examples: `void(int)`, `bool()`, `const float&(const vector<int>, int**)`

### Detouring static functions

Imagine you want to detour a static function that returns a float, and takes a const char* as parameter. You would do it like this:
```cpp
static_detour(MyDetour1, float(const char*)) {
	float detoured(const char* pString) {
		if (pString[0] == '_') return -1.0f;
		else {
			// We call the original function; as you can see you can use any parameter you want
			return original_function(pString) + original_function("object");
		}
	}
};
```

### Detouring a member method

Imagine you want to detour a method that is mebmer of a certain class. It returns void, and does not take any parameter. You would do it like this:
```cpp
// A class declaration, the method belongs to this class
class ClassManager {
protected:
	vector<void*> mObjects;
}

member_detour(MyDetour2, ClassManager, void()) {
	void detoured() {
		MessageBox("Objects are being deleted");
		// You can access the public and protected fields:
		mObjects.clear();
	}
};
```

### Detouring a virtual member method

Imagine you want to detour a virtual method that is member of a certain class. It returns void, and takes 2 float parameters. You would do it like this:
```cpp
// A class declaration, the method belongs to this class
// It's important to know in which class the method was declared. In this case, let's imagine it was declared in ICombatant
class cCreature : public IObject, public ICombatant {
protected:
	float mHealthPoints;
}

virtual_detour(MyDetour3, cCreature, ICombatant, void(float, float)) {
	void detoured(float strength, float distance) {
		// I want to be immortal!
		if (mHealthPoints < 10) mHealthPoints += 1000.0f;
		original_function(this, strength, distance);
	}
};
```

### Attaching/detaching detours

Detours must be attached/detached in the DllMain function. This header provides two functions:
- `PrepareDetours(HANDLE)`: must be called before using any detours.
- `CommitDetours()`: must be used after using detours

An example DllMain would look like this:
```cpp
switch (dwReason) {
	case DLL_PROCESS_ATTACH:
	PrepareDetours(hHandle);
	MyDetour1.attach(Address(0x405810));
	// You can also use address lists
	MyDetour2.attach(GetAddress(ClassManager, Create));
	CommitDetours();
	break;

case DLL_PROCESS_DETACH:
	PrepareDetours(hHandle);
	MyDetour1.detach();
	MyDetour2.detach();
	CommitDetours();
	break;
}
```

### Separating header/cpp files

If you want, you can declare the detour in a header file, and specify the code in a cpp file. It would look like this:
```cpp
// In the header file:
static_detour(MyDetour1, float(const char*)) {};

// In the cpp file:
// Use the macro DETOUR to access the method
float MyDetour1::DETOUR(const char* pString) {
	if (pString[0] == '_') return -1.0f;
	else {
		// We call the original function; as you can see you can use any parameter you want
		return original_function(pString) + original_function("object");
	}
}
```
