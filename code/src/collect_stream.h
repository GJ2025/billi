#ifndef COLLECT_STREAM_H
#define COLLECT_STREAM_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "tick_types.h"

struct range_price {
    double up = 0.0;
    double down = 0.0;
    double keep = 0.0;
};

struct deal_range{
    range_price super;
    range_price big;
    range_price middle;
    range_price small; 
};

struct stream_sum {

    deal_range classfy_b;
    deal_range classfy_s;
    deal_range classfy_m;
};

struct StreamRecord {
    std::vector<TickRecord> records;
    double gap = 0.0;
};

struct HeadTickData {
    TickRecord v_924;
    TickRecord pre_924;
    TickRecord v_925;
    double ratio_change_924 = 0.0;
    double ratio_change_pre_924 = 0.0;
};

struct DailyMetrics {
    long long valid_records_count = 0;
    double closing_price = 0.0;
    double am_closing_price = 0.0;
    long long am_vol = 0;
    long long pm_vol = 0;
    double total_turnover = 0.0;
    double am_turnover = 0.0; 
    double am_inflow = 0.0;
    double am_outflow = 0.0;
    double pm_inflow = 0.0;
    double pm_outflow = 0.0;

    HeadTickData head_data;
    bool head_calculated = false;
    TickRecord first_record;
    TickRecord last_record;
    stream_sum stream_sum_info;
};

struct classfy_bs_action{
    double buy = 0.0;
    double sale = 0.0;
    double middle = 0.0;
};

struct DayOutputMetrics {
    long long ticks_count = 0;  
    double pre_closing_price = 0.0;        
    double closing_price = 0.0;       
    double am_closing_price = 0.0;     
    double am_net_inflow_wan = 0.0;
    double pm_net_inflow_wan = 0.0;
    double net_inflow_wan = 0.0;
    double total_turnover_wan = 0.0;
    double am_turnover_wan = 0.0;     
    double am_turnover_ratio = 0.0;
    double total_vol_wan = 0.0;
    double am_vol_wan = 0.0;
    double avg_price = 0.0;
    double inflow_ratio = 0.0;
    double avg_vol_per_tick = 0.0;
    double net_per_change = 0.0;
    double pct_change = 0.0;
    double am_pct_change = 0.0;
    double start_change = 0.0;

    std::string date_str = "";
    double historical_total_inflow = 0.0;

    TickRecord first_record;
    TickRecord last_record;

    HeadTickData head_data;
    stream_sum stream_sum_info;

    classfy_bs_action deal_super;
    classfy_bs_action deal_big;
    classfy_bs_action deal_middle;
    classfy_bs_action deal_small;

    classfy_bs_action deal_total;
    
};


void collect_classfy_action(deal_range& deal, double trade, double gap);
void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, 
                               TickRecord& record, TickRecord& pre_record);

void deal_classfy(DayOutputMetrics& out);

void print_classfy( DayOutputMetrics& out);
#endif // COLLECT_STREAM_H
