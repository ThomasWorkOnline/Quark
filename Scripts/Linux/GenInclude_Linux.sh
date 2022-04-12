pushd ${0%/*}/../../

# Copy Quark header files
mkdir -p ./include/Quark/
cp -r ./Quark/src/Quark/* ./include/Quark/
cp -r ./Quark/src/Quark.h ./include/

# Copy binaries
mkdir -p ./lib/linux-x64-Debug/
mkdir -p ./lib/linux-x64-Release/
mkdir -p ./lib/linux-x64-Dist/
cp ./bin/linux-Debug-x86_64/Quark/*.a ./lib/linux-x64-Debug/
cp ./bin/linux-Release-x86_64/Quark/*.a ./lib/linux-x64-Release/
cp ./bin/linux-Dist-x86_64/Quark/*.a ./lib/linux-x64-Dist/

popd
