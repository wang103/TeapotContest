#include "mesh_data.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include "general_utils.h"

#define LINE_MAX 255		// maximum characters allowed in one line in mesh file

/**
 *	Constructor for mesh object which is stored in file 'file_name'.
 *	Assume that the file content is in the correct format.
 */
MeshData::MeshData(char *file_name) {
	FILE *fp = fopen(file_name, "rt");
	char line[LINE_MAX];
	char delims[] = " ";
	char *type = NULL;
	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	if (fp != NULL) {
		while (fgets(line, LINE_MAX, fp) != NULL) {
			if (strlen(line) == 0)
				continue;

			// data is stored in 'line' now
			type = strtok(line, delims);
			if (strcmp(type, "#") == 0 || strcmp(type, "g") == 0) {	// just comments
				continue;
			}

			var1 = strtok(NULL, delims);
			var2 = strtok(NULL, delims);
			var3 = strtok(NULL, delims);

			if (strcmp(type, "v") == 0) {			// this line represents a vertex
				Vec3f vertex(atof(var1), atof(var2), atof(var3));
				PositionList.push_back(vertex);
			}
			else if (strcmp(type, "f") == 0) {	// this line represents a face
				Vec3i face(atoi(var1), atoi(var2), atoi(var3));
				TriIndices.push_back(face);
			}
		}

		// end of file reached
		fclose(fp);

		computeFaceNormals();
		computeUnitVertexNormals();
	}
	else {
		perror("error opening mesh file");
		exit(1);
	}
}

/**
 *	Draw the mesh with OpenGL API.
 */
void MeshData::draw()
{
	glBegin(GL_TRIANGLES);
	for (unsigned int triIndex = 0; triIndex < TriIndices.size(); triIndex ++)
	{
		Vec3i tri = TriIndices[triIndex];
		tri[0] -= 1;
		tri[1] -= 1;
		tri[2] -= 1;

		glNormal3fv(VertexUnitNormalList[tri[0]]);
		glVertex3fv(PositionList[tri[0]]);
		
		glNormal3fv(VertexUnitNormalList[tri[1]]);
		glVertex3fv(PositionList[tri[1]]);
		
		glNormal3fv(VertexUnitNormalList[tri[2]]);
		glVertex3fv(PositionList[tri[2]]);
	}
	glEnd();
}

/**
 *	Compute the normals for all faces of this mesh object.
 *	The result is stored in 'FaceNormalList'.
 *	Assume that 'TriIndices' and 'PositionList' have been populated.
 */
void MeshData::computeFaceNormals()
{
	for (unsigned int triIndex = 0; triIndex < TriIndices.size(); triIndex ++)
	{
		Vec3i tri = TriIndices[triIndex];
		tri[0] -= 1;
		tri[1] -= 1;
		tri[2] -= 1;

		Vec3f p1 = PositionList[tri[0]];
		Vec3f p2 = PositionList[tri[1]];
		Vec3f p3 = PositionList[tri[2]];

		float v1[3] = {p1[0] - p3[0], p1[1] - p3[1], p1[2] - p3[2]};
		float v2[3] = {p2[0] - p3[0], p2[1] - p3[1], p2[2] - p3[2]};
		float result[3];

		calculateCrossProduct(v1, v2, result);

		FaceNormalList.push_back(result);
	}
}

/**
 *	Compute the unit normals for all vertices of this mesh object.
 *	The result is stored in 'VertexUnitNormalList'.
 *	Assume that 'TriIndices', 'PositionList', and 'FaceNormalList' have been populated.
 */
void MeshData::computeUnitVertexNormals()
{
	// initialize the list
	int numVertice = PositionList.size();
	for (int count = 0; count < numVertice; count ++)
	{
		VertexUnitNormalList.push_back(Vec3f(0, 0, 0));
	}

	for (unsigned int triIndex = 0; triIndex < TriIndices.size(); triIndex ++)
	{
		Vec3f currentFaceNormal = FaceNormalList[triIndex];

		Vec3i tri = TriIndices[triIndex];
		tri[0] -= 1;
		tri[1] -= 1;
		tri[2] -= 1;

		// add the normal of this surface to all three vertices
		addTwoVectors(VertexUnitNormalList[tri[0]], currentFaceNormal);
		addTwoVectors(VertexUnitNormalList[tri[1]], currentFaceNormal);
		addTwoVectors(VertexUnitNormalList[tri[2]], currentFaceNormal);
	}

	// normalize all the vertex normal vectors
	for (unsigned int index = 0; index < VertexUnitNormalList.size(); index ++)
	{
		normalize(VertexUnitNormalList[index]);
	}
}
