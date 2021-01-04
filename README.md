# Entropy

Entropy is a bare bones 3D game engine in development.
Support for Windows, Linux and macOS is on the way!

# Installation

1. Link 'libentropy.a'/'entropy.lib' to your project;
2. Include 'Entropy.h';
3. Create a sub-class from parent 'Entropy::Application';
4. Implement CreateApplication();

	Entropy::Application* Entropy::CreateApplication()
	{
		return new SandboxGame();
	}
