# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Designed for learning purposes.
Currently only supports Windows. Compatibility for Mac and Linux is not guaranteed.
Official support for Linux and macOS is on it's way!

# Installation & Setup

1. Download and build Quark as a static lib (if using Visual Studio, simply clone the solution and build for both Debug and Release). This might take a while. Optionally, you can download the compiled libraries from the project under `Quark/pre-compiled libs`.
2. Run the batch script inside Quark `Gen Include - Win.bat`. If you built it yourself, make sure both configurations of the libraries are compiled for the script to copy files properly.
3. Copy the generated include folder and binaries in your project.
4. Link the appropriate libraries for your application (on Visual Studio, you will need to link the Debug and Release binaries seperately depending on your configuration).
5. Quark internally uses glm, but exposes it. You will need to install glm as a dependency to your project.
6. Quark also uses entt (ECS). You will need to include this templated header only library.
7. #include <QK/Quark.h>
8. Create a class extending Quark::Application

Example:
```c++
#include <QK/Quark.h>

class YourGame : public Quark::Application
{
	...
}
```
	
Create your class instance in main:
```c++
int main()
{
	auto app = new YourGame();
	app->Run();
	delete app;
	
	return 0;
}
```
`Quark::Application::Run()` is only accessible from main. Make sure to invoke it there.

# Dependencies
glm: https://github.com/g-truc/glm
entt: https://github.com/skypjack/entt

Feel free to explore!
