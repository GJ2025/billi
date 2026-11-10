#ifndef SIG_H
#define SIG_H

#include <iostream>
#include <filesystem>
#include <span>
#include <vector>          // 1. 缺少 vector 头文件
#include <string>          // 2. 缺少 string 头文件
#include "collect_stream.h"

int signals_from_metrics(size_t size, const std::vector<DayOutputMetrics>& out_vector, VectorStats& v_stats) ;

#endif // SIG_H
