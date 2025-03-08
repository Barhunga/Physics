#include "SoftBody.h"
#include "Sphere.h"
#include "Spring.h"

void SoftBody::Build(PhysicsScene* scene, glm::vec2 position, float spacing, float springForce, float damping, std::vector<std::string>& strings)
{
	int numRows = strings.size();
	int numColumns = strings[0].length();

	// error checking for bad data
	for (int i = 1; i < numRows; i++)
	{
		if (strings[i].length() != numColumns) return;
	}

	// traverse across the array and add balls where the ascii art says they should be
	Sphere** spheres = new Sphere*[numRows * numColumns];

	for (int i = 0; i < numColumns; i++)
	{
		for (int j = 0; j < numRows; j++)
		{
			// traverse down the columns
			if (strings[j][i] == '0')
			{
				spheres[i * numRows + j] = new Sphere(position + glm::vec2(i, -j) * spacing, 
					glm::vec2(0, 0), 1.0f, 2.0f, 1, glm::vec4(1, 0, 0, 1));
				scene->addActor(spheres[i * numRows + j]);
			}
			else
			{
				spheres[i * numRows + j] = nullptr;
			}
		}
	}

	// second pass - add springs in
	for (int i = 1; i < numColumns; i++)
	{
		for (int j = 1; j < numRows; j++)
		{
			Sphere* s11 = spheres[i * numRows + j];
			Sphere* s01 = spheres[(i - 1) * numRows + j];
			Sphere* s10 = spheres[i * numRows + j - 1];
			Sphere* s00 = spheres[(i - 1) * numRows + j - 1];
			// make springs to cardinal neighbours
			if (s11 && s01)
				scene->addActor(new Spring(s11, s01, springForce, damping, spacing));
			if (s11 && s10)
				scene->addActor(new Spring(s11, s10, springForce, damping, spacing));
			if (s10 && s00)
				scene->addActor(new Spring(s10, s00, springForce, damping, spacing));
			if (s01 && s00)
				scene->addActor(new Spring(s01, s00, springForce, damping, spacing));
			// add a spring between s11 and s00, and another one between s01 and s10...


			bool endOfJ = j == numRows - 1;
			bool endOfI = i == numColumns - 1;
			Sphere* s22 = (!endOfI && !endOfJ) ? spheres[(i + 1) * numRows + (j + 1)] : nullptr;
			Sphere* s02 = !endOfJ ? spheres[(i - 1) * numRows + (j + 1)] : nullptr;
			Sphere* s20 = !endOfI ? spheres[(i + 1) * numRows + j - 1] : nullptr;

			// add springs between s00 and s02, s22 and s20, and so on...

		}
	}
}
