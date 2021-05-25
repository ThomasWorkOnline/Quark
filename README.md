# Quark

Quark is a 3D game engine in early development inspired by Hazel Game Engine.
Currently supports Windows. Support for Mac and Linux might not work as expected.
Official support for Linux and macOS is on it's way!

Quark is an API available to develop 2D and 3D applications.

# Installation & Setup

1. On Windows, link quark.lib to your project.
2. #include "Quark.h".
3. Create a sub-class from parent 'Quark::Application'

Example from Sandbox:
```c++
class SandboxGame : public Quark::Application
{
	...
}
```
4. Implement the function Quark::CreateApplication().
	If not defined, it will not compile!
	This is what internally creates your Quark application.
	
Example from Sandbox:
```c++
Quark::Application* Quark::CreateApplication()
{
	return new SandboxGame();
}
```
The setup is complete!
