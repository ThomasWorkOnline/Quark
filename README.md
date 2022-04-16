# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Designed for learning purposes.
Official support for macOS and Linux is on it's way!

***

# Installation & Setup

## On Windows
1. Run `git clone --resursive https://github.com/ThomasWorkOnline/Quark.git` in your desired environment folder.
2. After cloning this repository, run the script `GenVisualStudioSolution20xx_Win.bat` located under `Scripts/Windows`.
3. Open the Visual Studio solution and hit build! Build for your desired configuration(s).
4. Run `GenInclude_Win.bat`. This will generate the `include` directory alongside the `lib` folder.<br />
NOTE: *`GenInclude_Win.bat` will only copy files, so make sure the application is compiled for your desired platform.*

5. Link the appropriate libraries for your application (on Visual Studio, you will need to link the Debug, Release and Dist binaries seperately depending on your configuration).<br />

## On MacOS
1. Run `git clone --resursive https://github.com/ThomasWorkOnline/Quark.git` in your desired environment folder.
2. After cloning this repository, run the script `GenMakefile_MacOS.sh` located under `Scripts/Macos`.
3. Navigate to the project root directory and type `make config=<config>`.
### Different configs:
	make config=debug
	make config=release
	make config=dist

4. Run `GenInclude_MacOS.sh`. This will generate the `include` directory alongside the `lib` folder.<br />
NOTE: *`GenInclude_MacOS.sh` will only copy files, so make sure the application is compiled for your desired platform.*

5. Link the appropriate libraries for your application (remember to link the debug and release configs appropriately).

## On Linux
Coming soon!

## Dependencies
- [glm](https://github.com/g-truc/glm)
- [entt](https://github.com/skypjack/entt)
- [spdlog](https://github.com/gabime/spdlog/tree/083ea59fbd06add6f973636acca93ae0dfc0b73f)

*(all dependencies are located inside the `Quark/vendor` folder)*.
All you need to do is to include the designated 'Quark/premake5.lua' build step to your premake5 script.

## Build Configurations
### Debug
This config includes no optimizations and enables asserts aswell as logging. It is well suited for testing.
### Release
This config includes optimizations and strips assertions and log traces. Performance metrics are present. I would not suggest you run tests using this config. Use this config to run benchmarks and test your app's performance.
### Dist
This config includes full optimizations, strips assertions and log traces. Warnings and errors are always displayed. Use this config to ship.
	
*I encourage using dist builds with optimizations for distribution and debug while testing.
Release mode can be useful when you need to test performance.*

# How to use

The following examples will guide you in learning the Quark API.

<ins>**1. Creating a Quark application**</ins>
```c++
// Single header include
#include <Quark.h>

// Create a class extending `Quark::Application`
class YourApplication : public Quark::Application
{
	...
}
```
	
<ins>**2. Launching your app**</ins>

Quark already provides the default entry point for launching your program.
Redefining `main()` will cause a duplicate symbol linking error.

Use `Quark::Main()` method as shown in this snippet below:
Note: *On Windows, the app with launch without the console using the builtin WinMain() entry point (see EntryPoint.h for details)*

```c++
namespace Quark {

	int Main(int argc, char** argv)
	{
		auto app = Quark::CreateScope<YourApplication>();
		app->Run();
	}
}
```

Make sure to define `Main()` inside the Quark namespace.

Allocating an instance of your application can potentially be very memory heavy.
Therefore, instantiating your application on the stack is not adviced.
I suggest you dynamically allocate your app to prevent high stack usage which can lead to bugs and crashes.

<ins>**3. Implementing some functionality**</ins>
```c++
class YourApplication : public Quark::Application
{
public:
	// Called each frame
	// Elapsed time holds the time delta in seconds between frames
	void OnUpdate(Quark::Timestep elapsedTime) override
	{
		// Run your app logic and rendering here:
		...
	}
}
```

<ins>**4. Handling events**</ins>

Events provide a way to hook into certain application actions such as:
mouse and keyboard inputs, window moved, resized, minimized, maximized and much more.
The following examples will guide you through using Quark events:

```c++
class YourApplication : public Quark::Application
{
public:
	void OnEvent(Quark::Event& e) override
	{
		// Create an dispatcher object with the given event reference
		Quark::EventDispatcher dispatcher(e);
	
		...
	}
}
```

Event dispatchers are used to associate an event type with a given function.
The event handler must have the following signature:
```c++
bool Function(EventType&);
```

If the function you are linking to is a class member function, you'll have to use the `ATTACH_EVENT_FN` macro to pass the implicit `this` parameter as an argument.
For instance, let's write a function that handles keyboard input.

```c++
class YourApplication : public Quark::Application
{
public:
	void OnEvent(Quark::Event& e) override
	{
		// Create an dispatcher object with the given event reference
		Quark::EventDispatcher dispatcher(e);
	
		// Route all `KeyPressedEvent` to `YourApplication::OnKeyPressed`
		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(YourApplication::OnKeyPressed));
	}
	
private:
	// Our custom KeyPressedEvent handler
	bool OnKeyPressed(KeyPressedEvent& e)
	{       //               ^
		// Note that the event has been cast to the right type
		std::cout << e.GetKeyCode() << std::endl;
	
		// Returns if the event has been handled
		// A handled event will not be propagated to other handlers
		return false;
	}
}
```

By returning false, we do not prevent the event propagation. Returning true can be useful when you want to prevent the app from propagating the event any further.
For instance, in a FPS, you would not wan't your player controller to shoot when left-clicking in your inventory. Furthermore, your player controller should not be aware that a click event has been fired.

# Dependencies
glm: https://github.com/g-truc/glm<br />
entt: https://github.com/skypjack/entt<br />
spdlog: https://github.com/gabime/spdlog/tree/083ea59fbd06add6f973636acca93ae0dfc0b73f<br />

Feel free to explore!
