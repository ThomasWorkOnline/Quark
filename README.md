# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Designed for learning purposes.
Currently only builds for Windows.
Official support for Linux and macOS is on it's way!

***

# Installation & Setup

## On Windows

1. After cloning this repository, run the script `GenVisualStudioSolution_Win.bat` located in the project root.
2. Once the script is completed, open the Visual Studio solution and hit build!
3. After a successful build, run `GenInclude_Win.bat`. This will generate the `include` directory alongside the `lib` folder.<br />
*NOTE: `GenInclude_Win.bat` will only copy files, so make sure the application is compiled for your desired platform (I encourage using release builds).*<br />

4. Link the appropriate libraries for your application (on Visual Studio, you will need to link the Debug and Release binaries seperately depending on your configuration).
5. Quark internally uses glm and also exposes it. You will need to install glm as a dependency to your project.
6. Quark also uses entt (ECS). You will need to include this templated header only library as well.
7. `#include <Quark.h>`
8. The following examples will guide you in learning the Quark API.

# How to use

<ins>**1. Creating a Quark application**</ins>
```c++
#include <Quark.h>

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
The function given must have the following signature:<br />
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
	{
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
