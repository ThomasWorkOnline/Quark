# From The Cherno: https://github.com/TheCherno/Hazel/tree/master/scripts

import os
import platform

import Utils

from pathlib import Path
from io import BytesIO
from urllib.request import urlopen

class VulkanConfiguration:
	requiredVulkanVersion = "1.3."
	installVulkanVersion = "1.3.216.0"
	vulkanDirectory = "./Quark/vendor/VulkanSDK"

	@classmethod
	def Validate(cls):
		if (not cls.__CheckVulkanSDK()):
			print("Vulkan SDK not installed correctly.")
			return False
            
		if (not cls.__CheckVulkanSDKDebugLibs()):
			print("No Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
			print("Debug configuration disabled.")
			return False

		print("Vulkan validation successful!\n")
		return True

	@classmethod
	def __CheckVulkanSDK(cls):
		vulkanSDK = os.environ.get("VULKAN_SDK")
		if (vulkanSDK is None):
			print("You don't have the Vulkan SDK installed!")
			cls.__InstallVulkanSDK()
			return False
		else:
			print(f"Located Vulkan SDK at {vulkanSDK}")

		if (cls.requiredVulkanVersion not in vulkanSDK):
			print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.requiredVulkanVersion})")
			cls.__InstallVulkanSDK()
			return False
    
		return True

	@classmethod
	def __InstallVulkanSDK(cls):
		permissionGranted = False
		while not permissionGranted:
			reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
			if reply == 'n':
				return
			permissionGranted = (reply == 'y')
		
		vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/windows/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
		vulkanPath = f"{cls.vulkanDirectory}/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"

		print("Downloading {0:s} to {1:s}".format(vulkanInstallURL, vulkanPath))
		Utils.DownloadFile(vulkanInstallURL, vulkanPath)

		print("Running Vulkan SDK installer...")
		os.startfile(os.path.abspath(vulkanPath))

		print("Re-run this script after installation!")
		quit()

	@classmethod
	def __CheckVulkanSDKDebugLibs(cls):
		vulkanSDK = os.environ.get("VULKAN_SDK")
		shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")
		
		return shadercdLib.exists()

if __name__ == "__main__":
    VulkanConfiguration.Validate()
