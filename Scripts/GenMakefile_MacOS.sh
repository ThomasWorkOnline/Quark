pushd ${0%/*}/../
./vendor/bin/premake5/Macos/premake5 gmake2
popd
