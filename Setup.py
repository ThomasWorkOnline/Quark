import os
import subprocess

print("Installing submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])
