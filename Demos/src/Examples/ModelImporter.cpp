#include "ModelImporter.h"

ModelImporter::ModelImporter(const ApplicationOptions& options)
	: Application(options)
{
	Mesh mesh = Mesh::ReadOBJData("assets/Models/arrow.obj");
}
