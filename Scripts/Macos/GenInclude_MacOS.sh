pushd ${0%/*}/../../

# Copy Quark header files
mkdir -p ./include/Quark/
cp -r ./Quark/src/Quark/* ./include/Quark/
cp -r ./Quark/src/Quark.h ./include/

# Copy binaries
mkdir -p ./lib/macosx-x64-Debug/
mkdir -p ./lib/macosx-x64-Release/
mkdir -p ./lib/macosx-x64-Dist/
cp ./bin/macosx-Debug-x86_64/Quark/*.a ./lib/macosx-x64-Debug/
cp ./bin/macosx-Release-x86_64/Quark/*.a ./lib/macosx-x64-Release/
cp ./bin/macosx-Dist-x86_64/Quark/*.a ./lib/macosx-x64-Dist/

popd
