pushd ${0%/*}/../../Quark/

flags=-o
outputDir=./cache/spirv

if test -f "$outputDir"; then
   	echo "Do you want to delete the shader bin cache?"
	rm -r $outputDir
else
	mkdir -p $outputDir
fi

searchPath=./assets/shaders
shadersCompiled=0

echo Search path: $searchPath

for file in $(find $searchPath -name '*.vert' -or -name '*.frag'); do
	name="$(basename $file)"
	echo Compiling: $name to $outputDir/$name.spv
	glslc $file $flags $outputDir/$name.spv
	shadersCompiled=$(($shadersCompiled+1))
done

echo "\nShaders compiled:" $shadersCompiled
popd
