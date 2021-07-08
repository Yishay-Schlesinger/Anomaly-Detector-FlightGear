//208438119

#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "anomaly_detection_util.h"

using namespace std;

// ------------ DO NOT CHANGE -----------
class Circle{
public:
	Point center;
	float radius;
	Circle(Point c,float r):center(c),radius(r){}
};
// --------------------------------------

//declaration
Circle findMinCircle(Point** points,size_t size);
Circle findMinCircleAlgorithem(Point**& points,size_t size, vector<Point>& seperate_p);
Circle calCircleWith3Points(vector<Point>& seperate_p);
Circle calCircleWith2Points(vector<Point>& seperate_p);
bool isPointInCircle(Circle& circle,Point& p);
float distance(Point& p1,Point& p2);

#endif /* MINCIRCLE_H_ */