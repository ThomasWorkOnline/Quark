# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Designed for learning purposes.
Currently only supports Windows. Compatibility for Mac and Linux is not guaranteed.
Official support for Linux and macOS is on it's way!

# Installation & Setup

1. Run the batch script inside Quark `Gen Include - Win.bat`.
2. Copy the generated folder in your project.
3. Link the appropriate libraries for your application (on Visual Studio, you will need to link the Debug and Release binaries seperately depending on your configuration).
4. Quark internally uses glm, but exposes it. You will need to install glm as a dependency to your project.
5. Quark also uses entt (ECS). You will need to include this templated header only library.
6. #include <QK/Quark.h>
7. Create a class extending Quark::Application

Example from Sandbox:
```c++
#include <QK/Quark.h>

class SandboxGame : public Quark::Application
{
	...
}
```
	
Create your class instance in main:
```c++
int main()
{
	auto app = new Sandbox();
	app->Run();
	delete app;
	
	return 0;
}
```
`Quark::Application::Run()` is only accessible from main. Make sure to invoke it there.

Feel free to explore!
