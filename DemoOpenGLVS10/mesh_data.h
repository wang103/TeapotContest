#ifndef _MESH_DATA_H
#define _MESH_DATA_H

#include <vector>
#include "gfx\vec3.h"

using namespace gfx;
using namespace std;

class MeshData
{
private:
	vector<Vec3f> PositionList;				// positions of all vertices
	vector<Vec3i> TriIndices;				// indices of vertex for all faces
	vector<Vec3f> FaceNormalList;			// normals of all faces
	vector<Vec3f> VertexUnitNormalList;		// normalized normals of all vertices

	void computeFaceNormals();
	void computeUnitVertexNormals();

public:
	MeshData(char *file_name);
	void draw();
};

#endif
