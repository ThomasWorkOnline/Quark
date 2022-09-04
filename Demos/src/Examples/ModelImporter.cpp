#include "ModelImporter.h"

ModelImporter::ModelImporter(const ApplicationOptions& options)
	: Application(options)
{
	Mesh mesh = Mesh::ReadOBJData("assets/meshes/arrow.obj");
}
