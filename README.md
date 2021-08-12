# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Designed for learning purposes.
Currently only supports Windows. Compatibility for Mac and Linux is not guaranteed.
Official support for Linux and macOS is on it's way!

# Installation & Setup

1. Download and build Quark as a static lib (if using Visual Studio, simply clone the solution and build for both Debug and Release). This might take a while.
2. Optionally, you can download the compiled libraries from the project. Under `Quark/quark-v.x.x`
3. Run the batch script inside Quark `Gen Include - Win.bat`. Make sure both configurations of the libraries are pre-compiled for the script to copy files properly.
4. Copy the generated folder in your project.
5. Link the appropriate libraries for your application (on Visual Studio, you will need to link the Debug and Release binaries seperately depending on your configuration).
6. Quark internally uses glm, but exposes it. You will need to install glm as a dependency to your project.
7. Quark also uses entt (ECS). You will need to include this templated header only library.
8. #include <QK/Quark.h>
9. Create a class extending Quark::Application

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
