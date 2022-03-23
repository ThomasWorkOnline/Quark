# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Designed for learning purposes.
Currently only builds for Windows.
Official support for Linux and macOS is on it's way!

***

# Installation & Setup

## On Windows

1. After cloning this repository, run the script `GenVisualStudioSolution20XX_Win.bat` located under `Scripts`.<br />
2. Open the Visual Studio solution and hit build! Build for your desired configuration<br />
	*(I encourage using release builds with optimizations for distribution and debug while testing)*.<br />
4. Run `GenInclude_Win.bat`. This will generate the `include` directory alongside the `lib` and `vendor` folders.<br />
*NOTE: `GenInclude_Win.bat` will only copy files, so make sure the application is compiled for your desired platform.*<br />

4. Link the appropriate libraries for your application (on Visual Studio, you will need to link the Debug and Release binaries seperately depending on your configuration).
5. Quark uses [glm](https://github.com/g-truc/glm). You will need to link glm as a dependency to your project.<br />
6. Quark also uses [entt](https://github.com/skypjack/entt). You will need to include both of these templated header libraries for Quark to link.<br />
	*(all dependencies are located inside the `vendor` folder when the project has been built)*.<br />
7. The following examples will guide you in learning the Quark API.<br />

# How to use

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

Allocating an instance of your application can potentially be very memory heavy.
Therefore, instantiating your application on the stack is not adviced.
I suggest you dynamically allocate your app to prevent high stack usage which can lead to janky behavior.

```c++
int main()
{
	auto app = Quark::CreateScope<YourApplication>();
	app->Run();
}
```

<ins>**3. Implementing some functionality**</ins>
```c++
class YourApplication : public Quark::Application
{
public:
	// Called each frame
	// Elapsed time is in seconds
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
The event handler must have the following signature:<br />
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

# Dependencies
glm: https://github.com/g-truc/glm<br />
entt: https://github.com/skypjack/entt<br />

Feel free to explore!
