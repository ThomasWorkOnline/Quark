# Entropy

Entropy is a 3D game engine in early development.
Currently supports Windows only.
Support for Linux and macOS is on the way!

# Installation

1. Link 'libentropy.a'/'entropy.lib' to your project;
2. Include 'Entropy.h';
3. Create a sub-class from parent 'Entropy::Application';
4. Implement Entropy::CreateApplication();

	Entropy::Application* Entropy::CreateApplication()
	{
		return new SandboxGame();
	}
	
