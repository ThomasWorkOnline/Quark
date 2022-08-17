import os
import platform
import subprocess

from Scripts.VulkanConfiguraton import VulkanConfiguration as VulkanRequirements

VulkanRequirements.Validate()

if platform.system() == "Windows":
	dirname = os.path.dirname(__file__)
	os.chdir(dirname) # Run subprocess as if we are in the solution root

	print("Generating Visual Studio solution using Premake...")
	subprocess.call([os.path.abspath("./Scripts/Windows/GenVisualStudio2022.bat")])
