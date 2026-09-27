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

void check_sub_conditions(const std::string& file, const VectorStats& v_stats, std::vector<SubCondition>& sub_conditions, bool print_anyway){
    bool printed = false;

    for (const auto& sc : sub_conditions) {
        if (sc.satisfied) {
            print_signal(file, v_stats, sc);
            printed = true;
        }
    }

    if (printed == false && print_anyway){
        SubCondition dump;
        print_signal(file, v_stats, dump);
    }

    return;
}

void signals_from_metrics(size_t size, const std::vector<std::string>& files_to_process, const std::vector<DayOutputMetrics>& out_vector, bool print_anyway) {
    if (size < 2 || size > files_to_process.size() || size > out_vector.size()) {
        return;
    }

    VectorStats v_stats;
    const auto& file = files_to_process[0]; 
    metry_vector_summary(out_vector, v_stats);
    TradeCategoryStats& a0 = v_stats.a[0];

    std::vector<SubCondition> sub_conditions = {
        {
            v_stats.money_in_and_price_up_nt >=2 ,
            "M_IN_NT_UP"
        },
        {
            a0.all_will_netin > 0 && a0.all_price_netin > 0 && a0.pct_change_base_pre < 0.3 &&  v_stats.price_day_adjacent[0] < -3,
            "will_up"
        },
        {
            a0.all_netin && a0.pct_change_base_pre < 0.1,
            "abnormal_all"
        },       
    };


    check_sub_conditions(file, v_stats, sub_conditions, print_anyway);

    return;
}