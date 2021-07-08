//208438119

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector : public SimpleAnomalyDetector
{
	float another_min_correlation = 0.5;
public:
	virtual void update_cf_vector(correlatedFeatures current_cf, float *x, float *y, int size);
	virtual bool is_annomaly(float p_x, float p_y, correlatedFeatures current_cf);
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
