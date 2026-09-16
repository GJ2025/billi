#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>
#include <filesystem> 
#include <algorithm>  
#include <cmath>       
#include <cstdlib>   
#include <unistd.h> 
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "common.h"
#include "collect_stream.h"
#include "sig.h"


template <typename F>
int generic_fixed_base_check(const std::vector<DayOutputMetrics>& out_vector, F should_break) {
    if (out_vector.size() < 2){
        return 0;
    } 

    int j = 0;
    for (size_t i = 1; i < out_vector.size(); ++i) {
        if (should_break(out_vector[0].metrics, out_vector[i].metrics)){
            break;
        } 
        ++j;
    }
    return j;
}

template <typename F>
int generic_adjacent_check(const std::vector<DayOutputMetrics>& out_vector, F should_break) {
    if (out_vector.size() < 2){
        return 0;
    } 

    int j = 0;
    for (size_t i = 0; i + 1 < out_vector.size(); ++i) {
        if (should_break(out_vector[i].metrics, out_vector[i + 1].metrics)){
            break;    
        } 
        ++j;
    }
    return j;
}


int metrics_down_check_price_adjacent(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return base.closing_price >= curr.closing_price; 
    };
    
    return generic_adjacent_check(out_vector, pred);
}

int metrics_up_check_price_adjacent(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return base.closing_price < curr.closing_price; 
    };
    
    return generic_adjacent_check(out_vector, pred);
}

int metrics_price_check_adjacent(const std::vector<DayOutputMetrics>& out_vector) {

    int down_day = metrics_down_check_price_adjacent(out_vector);
    int up_day = metrics_up_check_price_adjacent(out_vector);

    if (down_day != 0) {
        return 0 - down_day;
    }

    if (up_day != 0){
        return up_day;
    }

    return 0;
}

int metrics_down_check_volume_adjacent(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return metrics_total_volume(base) >= metrics_total_volume(curr); 
    };
    
    return generic_adjacent_check(out_vector, pred);
}

int metrics_up_check_volume_adjacent(const std::vector<DayOutputMetrics>& out_vector) {
    auto pred = [](const auto& base, const auto& curr) { 
        return metrics_total_volume(base) < metrics_total_volume(curr);
    };
    
    return generic_adjacent_check(out_vector, pred);
}

int metrics_volume_check_adjacent(const std::vector<DayOutputMetrics>& out_vector) {

    int down_day = metrics_down_check_volume_adjacent(out_vector);
    int up_day = metrics_up_check_volume_adjacent(out_vector);

    if (down_day != 0) {
        return 0 - down_day;
    }

    if (up_day != 0){
        return up_day;
    }

    return 0;
}



void metry_vector_summary(const std::vector<DayOutputMetrics>& out_vector, VectorStats& stats){

    stats.price_day_adjacent.push_back(metrics_price_check_adjacent(out_vector));
    stats.price_day_adjacent.push_back(metrics_price_check_adjacent(get_sub_vector(out_vector,1)));
    stats.price_day_adjacent.push_back(metrics_price_check_adjacent(get_sub_vector(out_vector,2)));
    stats.price_day_adjacent.push_back(metrics_price_check_adjacent(get_sub_vector(out_vector,3)));


    stats.volume_day_adjacent.push_back(metrics_volume_check_adjacent(out_vector));
    stats.volume_day_adjacent.push_back(metrics_volume_check_adjacent(get_sub_vector(out_vector,1)));
    stats.volume_day_adjacent.push_back(metrics_volume_check_adjacent(get_sub_vector(out_vector,2)));
    stats.volume_day_adjacent.push_back(metrics_volume_check_adjacent(get_sub_vector(out_vector,3)));


    stats.a.resize(10);

    metry_summary(out_vector[0], stats.a[0]);
    metry_summary(out_vector[1], stats.a[1]);

    stats.a[0].all_will_netin_pct = (stats.a[0].all_will_netin - stats.a[1].all_will_netin) / std::abs(stats.a[1].all_will_netin);
    stats.a[0].all_price_netin_pct = (stats.a[0].all_price_netin - stats.a[1].all_price_netin) / std::abs(stats.a[1].all_price_netin);


    stats.a[0].strip_will_netin_pct = (stats.a[0].strip_will_netin - stats.a[1].strip_will_netin) / std::abs(stats.a[1].strip_will_netin);
    stats.a[0].strip_price_netin_pct = (stats.a[0].strip_price_netin - stats.a[1].strip_price_netin) / std::abs(stats.a[1].strip_price_netin);

    return;
}
