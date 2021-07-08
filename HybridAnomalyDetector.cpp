//208438119
#include "HybridAnomalyDetector.h"

void HybridAnomalyDetector::update_cf_vector(correlatedFeatures current_cf, float *x, float *y, int size)
{
	float high_limit_correlation = min_correlation;
	float low_limit_correlation = another_min_correlation;
	if (current_cf.corrlation > low_limit_correlation && current_cf.corrlation <= high_limit_correlation)
	{
		Point** p = convert_arrays_to_points(x,y,size);
		Circle min_circle = findMinCircle(p, size);
		for(int i=0;i<size;i++){
			delete p[i];
		}
		delete p;
		current_cf.center = min_circle.center;
		current_cf.threshold = min_circle.radius* 1.1;
		this->cf.push_back(current_cf);
	}
	else
	{
		return SimpleAnomalyDetector::update_cf_vector(current_cf, x, y, size);
	}
}

bool HybridAnomalyDetector::is_annomaly(float p_x, float p_y, correlatedFeatures current_cf)
{
	if (current_cf.corrlation <= SimpleAnomalyDetector::min_correlation)
	{
		Circle c = {current_cf.center, current_cf.threshold};
		Point p = {p_x, p_y};
		return !isPointInCircle(c, p);
	}
	else
	{
		return SimpleAnomalyDetector::is_annomaly(p_x, p_y, current_cf);
	}
}

HybridAnomalyDetector::HybridAnomalyDetector()
{
}

HybridAnomalyDetector::~HybridAnomalyDetector()
{
	// TODO Auto-generated destructor stub
}