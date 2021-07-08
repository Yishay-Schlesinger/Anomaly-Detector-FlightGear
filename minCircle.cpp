//208438119
#include "minCircle.h"

// implement
Circle findMinCircle(Point** points,size_t size){
	vector<Point> seperate_p;
	return findMinCircleAlgorithem(points,size,seperate_p);
}

Circle findMinCircleAlgorithem(Point**& points,size_t size, vector<Point>& seperate_p){
	int size_of_seperate_p = seperate_p.size();
	if(size==0 || size_of_seperate_p==3){
		if (size_of_seperate_p == 3){  
			return calCircleWith3Points(seperate_p);
		}
		else if(size_of_seperate_p == 2 ){
			return calCircleWith2Points(seperate_p);
		}
		else if(size_of_seperate_p==1){
			return Circle(seperate_p[0],0);
		}else{
			return Circle(Point(0,0),0);
		}
	}
	Point p = *points[size-1];
	vector<Point> seperate_p_copy = seperate_p; 
	Circle optional_circle = findMinCircleAlgorithem(points,size-1,seperate_p_copy);
	if(isPointInCircle(optional_circle, p)){
		return optional_circle;
	}
	seperate_p.push_back(p);

	return findMinCircleAlgorithem(points,size-1,seperate_p);
}

Circle calCircleWith3Points(vector<Point>& p){
	float A = p[0].x * (p[1].y - p[2].y) - p[0].y * (p[1].x - p[2].x) + p[1].x * p[2].y - p[2].x * p[1].y;
	float B = (pow(p[0].x, 2) + pow(p[0].y, 2)) * (p[2].y - p[1].y) + (pow(p[1].x, 2) + pow(p[1].y, 2)) * (p[0].y - p[2].y) + (pow(p[2].x, 2) + pow(p[2].y, 2)) * (p[1].y - p[0].y);
	float C = (pow(p[0].x, 2) + pow(p[0].y, 2)) * (p[1].x - p[2].x) + (pow(p[1].x, 2) + pow(p[1].y, 2)) * (p[2].x - p[0].x) + (pow(p[2].x, 2) + pow(p[2].y, 2)) * (p[0].x - p[1].x);
	float D = (pow(p[0].x, 2) + pow(p[0].y, 2)) * (p[2].x*p[1].y - p[1].x*p[2].y) + (pow(p[1].x, 2) + pow(p[1].y, 2)) * (p[0].x*p[2].y - p[2].x*p[0].y) + (pow(p[2].x, 2) + pow(p[2].y, 2)) * (p[1].x*p[0].y - p[0].x*p[1].y);
	float p_x = -(B/(2*A));
	float p_y = -(C/(2*A));
	float r = sqrt((pow(B,2)+pow(C,2) -4*A*D)/(4*pow(A,2)));
	return {{p_x,p_y},r}; 
}

Circle calCircleWith2Points(vector<Point>& seperate_p){
	Point center{(seperate_p[0].x+seperate_p[1].x)/2,(seperate_p[0].y+seperate_p[1].y)/2};
	return Circle(center,distance(center,seperate_p[0]));
}

bool isPointInCircle(Circle& circle,Point& p){
	return (distance(circle.center,p)<=circle.radius);
}

float distance(Point& p1,Point& p2){
	return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}