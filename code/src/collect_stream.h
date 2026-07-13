#ifndef COLLECT_STREAM_H
#define COLLECT_STREAM_H

#include <string>
#include <vector>
#include "tick_types.h"

// --- 基础结构 ---
struct by_price {
    double up = 0.0;
    double down = 0.0;
    double keep = 0.0;
};

// 规模下的行为组：先按规模分，内部再分买/卖/中性
struct bs_action_group {
    by_price buy;
    by_price sale;
    by_price neutral; 
};

// 顶层汇总：按规模区分
struct stream_sum {
    bs_action_group super;
    bs_action_group big;
    bs_action_group middle;
    bs_action_group small;
};

// 汇总用的辅助结构
struct classfy_bs_action {
    double buy = 0.0;
    double sale = 0.0;
    double middle = 0.0;
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

    by_price price_deal_super;
    by_price price_deal_big;
    by_price price_deal_middle;
    by_price price_deal_small;
    by_price price_deal_total;

    
};

// --- 函数声明 ---
void collect_bs_action(bs_action_group& group, const std::string& bs_type, double trade, double gap);
void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, TickRecord& record, TickRecord& pre_record);
void deal_classfy(DayOutputMetrics& out);
void print_will(DayOutputMetrics& out);
void print_super_price(DayOutputMetrics& out);
void print_super_price_header();
void print_price( DayOutputMetrics& out);
void print_merge( DayOutputMetrics& out);

#endif // COLLECT_STREAM_H