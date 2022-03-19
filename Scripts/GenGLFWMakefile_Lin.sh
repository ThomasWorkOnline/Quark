chmod +x InstallGLFWDependencies_Lin.sh
./InstallGLFWDependencies_Lin.sh

cmake -S ../Quark/vendor/glfw -B ../Quark/vendor/glfw/build
