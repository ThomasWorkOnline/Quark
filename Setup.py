# Thanks to The Cherno for the Python scripts here:
# https://github.com/TheCherno/Hazel/tree/master/scripts

import os
import platform
import subprocess

from Scripts.VulkanConfiguration import VulkanConfiguration as VulkanRequirements

dirname = os.path.dirname(__file__)
os.chdir(dirname) # Run subprocess as if we are in the root folder

VulkanRequirements.Validate()

if platform.system() == "Windows":
	print("Generating Visual Studio solution using Premake...")
	subprocess.call([os.path.abspath("./Scripts/Windows/GenVisualStudio2022.bat")])
