# Entropy

Entropy is a 3D game engine in early development inspired by Hazel Game Engine.
Currently supports Windows, support for Mac and Linux should work as expected though not thoroughly tested.
Official support for Linux and macOS is on it's way!

# Installation

1. Link the appropriate library 'libentropy.a'/'entropy.lib' to your project;
2. Include 'Entropy.h';
3. Create a sub-class from parent 'Entropy::Application';
	```
	class SandboxGame : public Entropy::Application
	{
		...
	}
	```
4. Implement Entropy::CreateApplication();
	```Entropy::Application* Entropy::CreateApplication()
	{
		return new SandboxGame();
	}
	```
	
