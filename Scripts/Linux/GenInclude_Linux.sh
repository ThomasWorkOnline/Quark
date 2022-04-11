version=1.0
dest=quark-v${version}

pushd ${0%/*}/../../

# Copy Quark header files
mkdir -p ./${dest}/include/Quark/
cp -r ./Quark/src/Quark/* ./${dest}/include/Quark/
cp -r ./Quark/src/Quark.h ./${dest}/include/

# Copy header libraries
mkdir -p ./${dest}/vendor/entt/
mkdir -p ./${dest}/vendor/glm/
mkdir -p ./${dest}/vendor/spdlog/
cp -r ./Quark/vendor/entt/* ./${dest}/vendor/entt/
cp -r ./Quark/vendor/glm/* ./${dest}/vendor/glm/
cp -r ./Quark/vendor/spdlog/* ./${dest}/vendor/spdlog/

# Copy binaries
mkdir -p ./${dest}/lib/linux-x64-Debug/
mkdir -p ./${dest}/lib/linux-x64-Release/
mkdir -p ./${dest}/lib/linux-x64-Dist/
cp ./bin/linux-Debug-x86_64/Quark/*.a ./${dest}/lib/linux-x64-Debug/
cp ./bin/linux-Release-x86_64/Quark/*.a ./${dest}/lib/linux-x64-Release/
cp ./bin/linux-Dist-x86_64/Quark/*.a ./${dest}/lib/linux-x64-Dist/

popd
