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

<ins>**1. Creating a Quark application**</ins>
```c++
#include <Quark.h>

class YourApplication : public Quark::Application
{
	...
}
```
	
<ins>**2. Launching your app**</ins>
```c++
int main()
{
	auto app = new YourApplication();
	app->Run();
	delete app;
}
```

<ins>**3. Implementing some functionality**</ins>
```c++
class YourApplication : public Quark::Application
{
public:
	// Called each frame
	// Elapsed time is in seconds
	void OnUpdate(float elapsedTime) override
	{
		// Run your app logic and rendering here:
		...
	}
}
```

<ins>**4. Handling events**</ins>

Events provide a way to hook into certain application events such as:
mouse and keyboard inputs, window move, resize, minimized, maximized and much more.
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
