#ifndef VOLUME_H
#define VOLUME_H

#include <glm/glm.hpp>
#include <iostream>

#include "Polygon.h"

class Volume
{
private:
public:
      Volume();
      Volume(glm::vec3 _BBL, glm::vec3 _TTR);
      ~Volume();

      void print();

      glm::vec3 BBL;
      glm::vec3 TTR;

      float sz;
      float hsz;

      Volume getChild(int c);
      bool containsVertex(const glm::vec3);
      bool containsPolygon(const Polygon p);
      int countContainedVertices(const Polygon p);
      bool intersectPolygon(const Polygon p);



	bool GetIntersect( float d1, float d2, glm::vec3 x_0, glm::vec3 x_1, glm::vec3 hit );
	bool InBox( glm::vec3 hit, int axis );
	bool lineNodeIntersection(glm::vec3 x_0, glm::vec3 x_1);
};

#endif
