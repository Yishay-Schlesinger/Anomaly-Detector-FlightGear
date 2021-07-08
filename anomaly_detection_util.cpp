//208438119

#include <math.h>
#include "anomaly_detection_util.h"
#include <vector>

using namespace std;

float sum(float *x, int size)
{
	float sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += x[i];
	}
	return sum;
}

//get array and return new array of square values of the recived one(Notice:should be deleted)
float *arraySquared(float *x, int size)
{
	float *y = new float[size];
	for (int i = 0; i < size; i++)
	{
		y[i] = x[i] * x[i];
	}
	return y;
}

float avg(float *x, int size)
{
	float avg = sum(x, size) / size;
	return avg;
}

// returns the variance of X and Y
float var(float *x, int size)
{
	float arrayAvg = avg(x, size);
	float *arraySqr = arraySquared(x, size);
	float rtn = avg(arraySqr, size) - pow(arrayAvg, 2);
	delete arraySqr;
	return rtn;
}

// returns the covariance of X and Y
float cov(float *x, float *y, int size)
{
	float sum = 0;
	float xAvg = avg(x, size);
	float yAvg = avg(y, size);
	for (int i = 0; i < size; i++)
	{
		sum += (x[i] - xAvg) * (y[i] - yAvg);
	}
	return sum / size;
}

// returns the Pearson correlation coefficient of X and Y
float pearson(float *x, float *y, int size)
{
	float covXY = cov(x, y, size);
	float divideBy = sqrt(var(x, size)) * sqrt(var(y, size));
	float rtn = covXY / divideBy;
	if (rtn < 0)
	{
		rtn = -rtn;
	}
	return rtn;
}

// performs a linear regression and returns the line equation
Line linear_reg(Point **points, int size)
{
	vector<float> x(size);
	vector<float> y(size);
	for (int i = 0; i < size; i++)
	{
		x[i] = points[i]->x;
		y[i] = points[i]->y;
	}
	return linear_reg(x.data(), y.data(), size);
}


Point** convert_arrays_to_points(float* x, float*y, int size){
	Point** ps=new Point*[size];
	for(size_t i=0;i<size;i++){
		ps[i]=new Point(x[i],y[i]);
	}
	return ps;
}

Line linear_reg(float *x, float *y, int size)
{
	float a = cov(x, y, size) / var(x, size);
	float b = avg(y, size) - a * avg(x, size);
	return Line(a, b);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size)
{
	return dev(p, linear_reg(points, size));
}

// returns the deviation between point p and the line
float dev(Point p, Line l)
{
	float fx = l.f(p.x);
	float y = p.y;
	float rtn = fx - y;
	if (rtn < 0)
	{
		rtn = -rtn;
	}
	return rtn;
}