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

void check_sub_conditions(const std::string& file, const VectorStats& v_stats, std::vector<SubCondition>& sub_conditions){
    for (const auto& sc : sub_conditions) {
        if (sc.satisfied) {
            print_signal(file, v_stats, sc);
        }
    }
}

void signals_from_metrics(size_t size, const std::vector<std::string>& files_to_process, const std::vector<DayOutputMetrics>& out_vector) {
    if (size < 2 || size > files_to_process.size() || size > out_vector.size()) {
        return;
    }

    VectorStats v_stats;

    const auto& file = files_to_process[0]; 

    metry_vector_summary(out_vector, v_stats);

    TradeCategoryStats& a0 = v_stats.a0;
    TradeCategoryStats& a1 = v_stats.a1;

    bool all_netin = (a0.all_will_netin > 0 || a0.all_price_netin > 0);
    bool middle_netin = (a0.strip_will_netin > 0 || a0.strip_price_netin > 0);


    std::vector<SubCondition> sub_conditions = {
        {
            a0.all_will_netin > 0 && a0.all_price_netin > 0 && a0.pct_change_base_pre < 0.3 &&  v_stats.price_down_day_adjacent > 3,
            "will_up"
        },
        {
            all_netin && a0.pct_change_base_pre < 0.1,
            "abnormal_all"
        },
        {
            middle_netin && a0.pct_change_base_925 < 0.1,
            "abnormal_middle"
        },
        {
            a0.all_will_netin > 0 && a0.all_price_netin > 0  
            && a0.all_will_netin_pct > 0 && a0.all_price_netin_pct > 0 
            && v_stats.price_day_adjacent[0] >= -1 && v_stats.price_day_adjacent[0] <= 3,
            "SPEEDUP(" + pct_base_string(a0.buyup_pct) + "vs" + pct_base_string(a0.buyup_pct - a1.buyup_pct) + ")" 
        },
        {
            a0.pct_change_base_925 > 0 && middle_netin == false,
            "up_out_m" 
        },
        {
            a0.pct_change_base_pre > 0 && all_netin == false,
            "up_out_all" 
        }        
    };


    check_sub_conditions(file, v_stats, sub_conditions);
}