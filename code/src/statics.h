#ifndef STATICS_H
#define STATICS_H

#include "opts.h"
#include "collect_stream.h"

int metrics_grow_loose(const std::vector<DayOutputMetrics>& out_vector);
int metrics_shrink_loose(const std::vector<DayOutputMetrics>& out_vector);
int metrics_grow_firm(const std::vector<DayOutputMetrics>& out_vector);
int metrics_shrink_firm(const std::vector<DayOutputMetrics>& out_vector);
void metry_vector_summary(const std::vector<DayOutputMetrics>& out_vector, VectorStats& stats);


#endif // STATICS_H