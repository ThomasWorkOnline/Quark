# Thanks to The Cherno for the Python scripts here:
# https://github.com/TheCherno/Hazel/tree/master/scripts

import os
import platform
import subprocess

from VulkanConfiguraton import VulkanConfiguration as VulkanRequirements

VulkanRequirements.Validate()

if platform.system() == "Windows":
	dirname = os.path.dirname(__file__)
	os.chdir(dirname) # Run subprocess as if we are in the scripts folder

	print("Generating Visual Studio solution using Premake...")
	subprocess.call([os.path.abspath("./Windows/GenVisualStudio2022.bat")])
