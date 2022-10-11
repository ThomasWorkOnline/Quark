#include "ModelImporter.h"

ModelImporter::ModelImporter(const ApplicationSpecification& spec)
	: Application(spec)
{
	Mesh mesh = Mesh::ReadOBJData("assets/Models/arrow.obj");
}
