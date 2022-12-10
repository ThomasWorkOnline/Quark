#include "ModelImporter.h"

ModelImporter::ModelImporter(const ApplicationSpecification& spec)
	: Application(spec)
{
	StaticMesh mesh = StaticMesh::ReadOBJData("assets/Models/arrow.obj");
}
