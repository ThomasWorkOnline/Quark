# Entropy

Entropy is a 3D game engine in early development inspired by Hazel Game Engine.
Currently supports Windows, support for Mac and Linux should work as expected though not thoroughly tested.
Official support for Linux and macOS is on it's way!

# Installation & Setup

1. On Windows, link entropy.lib to your project. On Unix systems, use libentropy.a.
2. #include "Entropy.h".
3. Create a sub-class from parent 'Entropy::Application'

Example from Sandbox:
```
class SandboxGame : public Entropy::Application
{
	...
}
```
4. Implement the function Entropy::CreateApplication().
	If not defined externally, it will not compile! This is what gives Entropy ownership of your application.
	
Example from Sandbox:
```
Entropy::Application* Entropy::CreateApplication()
{
	return new SandboxGame();
}
```
5. OnCreate(), OnUpdate() and OnApplicationEvent() are methods to be overriden available for your application.
