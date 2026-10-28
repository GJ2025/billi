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
#include "opts.h"
#include "tick_types.h"
#include "process_files.h"
#include "time_seq.h"
#include "show.h"
#include "collect_stream.h"
#include "statics.h"

bool check_sub_conditions(const std::string& file, const VectorStats& v_stats, std::vector<SubCondition>& sub_conditions){
    bool printed = false;

    for (const auto& sc : sub_conditions) {
        if (sc.satisfied) {
            print_signal(file, v_stats, sc);
            printed = true;
        }
    }

    return printed;
}

std::string top10show(topN& N){
    std::ostringstream oss;

    oss << "top10:" << N.net << ":b" << N.b << ":s" << N.s;
    return oss.str();
}

void signals_from_metrics(const std::string& file, size_t size, const std::vector<DayOutputMetrics>& out_vector, bool print_anyway, int topN) {
    if (size < 2 ||  size > out_vector.size()) {
        return;
    }

    bool  hit = false;
    VectorStats v_stats;
    metry_vector_summary(out_vector, v_stats, topN);
    TradeCategoryStats& a0 = v_stats.a[0];

    std::vector<SubCondition> sub_conditions = {
        {
            v_stats.money_in_and_price_up_nt >=2 ,
            "M_IN_NT_UP"
        },
        {
            a0.all_netin && a0.pct_change_base_pre < 0.1 && a0.middle_netin && a0.pct_change_base_925 < 0.1 &&  v_stats.price_day_adjacent[0] < -3,
            "will_up"
        },
        {
            a0.all_netin && a0.pct_change_base_pre < 0.1,
            "abnormal_all"
        },
        {
            a0.middle_netin && a0.pct_change_base_925 < 0.1,
            "abnormal_middle"
        },
        {
            a0.all_netin && a0.pct_change_base_pre < 0.1 && a0.middle_netin && a0.pct_change_base_925 < 0.1,
            "abnormal_FULL"
        },
        {
            v_stats.N.net > 0 &&  v_stats.N.b > v_stats.N.s+1 && a0.pct_change_base_pre < -1.0 &&  a0.pct_change_base_925 < -1.0,
            top10show(v_stats.N)
        },             
    };


    hit = check_sub_conditions(file, v_stats, sub_conditions);

    if (hit == false && print_anyway == true){
        SubCondition dump;
        print_signal(file, v_stats, dump);
    }

    return;
}