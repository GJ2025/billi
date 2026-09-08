#ifndef COLLECT_STREAM_H
#define COLLECT_STREAM_H

#include <string>
#include <vector>
#include <iomanip>
#include "common.h"
#include "tick_types.h"
#include "tables.h"

enum class RecordType {
    FIRST,       
    LAST
};

enum class RecordScale {
    SUPER,       
    BIG,
    MIDDLE,
    SMALL,
    TOTAL
};

struct trade {
    double money = 0.0;
    size_t volume = 0;
    size_t tick_count = 0;

    trade& operator+=(const trade& rhs) {
        money += rhs.money;
        volume += rhs.volume;
        tick_count += rhs.tick_count;
        return *this;
    }

    trade& operator-=(const trade& rhs) {
        money -= rhs.money;
        volume -= rhs.volume;
        tick_count -= rhs.tick_count;
        return *this;
    }
};


inline trade operator+(trade lhs, const trade& rhs) {
    lhs += rhs; 
    return lhs; 
}

inline trade operator-(trade lhs, const trade& rhs) {
    lhs -= rhs;
    return lhs;
}

struct deal_bsn {
    trade buy;
    trade sale;
    trade neutral;
};

struct deal_price {
    trade up;
    trade down;
    trade keep;
};

struct bsn_action_group {
    deal_price buy;
    deal_price sale;
    deal_price neutral; 
};

struct deal_summary {
    deal_bsn bsn; 
    deal_price price ;
    trade total;
    trade type_total;
};

struct bs_info{
    bsn_action_group info;
    deal_summary summary;
};

struct range_info{
    bs_info super;
    bs_info big;
    bs_info middle;
    bs_info small;
    bs_info total;
};

struct record_stream {
    tickTime time;
    bsn_action_group super;
    bsn_action_group big;
    bsn_action_group middle;
    bsn_action_group small;
    bsn_action_group total;
};

struct Burst_st {
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
    size_t ticks_count = 0;
    double closing_price = 0.0;
    double pre_closing_price = 0.0;
    double this_1457_pirce = 0.0;
    double avg_price = 0.0;
    double all_money = 0.0;
    size_t all_volume = 0.0;

    deal_bsn am_bsn;
    deal_bsn pm_bsn;

    HeadTickData head_data;
    bool head_calculated = false;
    TickRecord daily_first_record;
    TickRecord daily_last_record;
    record_stream header;
    record_stream start_point;
    record_stream end_point;
};


struct DailyMetrics_range_st {
    std::vector<tickTime> tick_times;
    std::vector<DailyMetrics> all_metrics;
    DailyMetrics metrics;
};

struct SubCondition {
    bool satisfied;         
    std::string description; 
};

struct TradeCategoryStats {

    double all_will_netin = 0.0;
    double all_price_netin = 0.0;
    double strip_will_netin = 0.0; 
    double strip_price_netin = 0.0;

    double buyup_pct = 0.0;
    double buydown_pct = 0.0;

    double saleup_pct = 0.0;

    double pct_change_base_pre = 0.0;
    double pct_change_base_925 = 0.0;

    double all_will_netin_pct = 0.0;
    double all_price_netin_pct = 0.0;

    double strip_will_netin_pct = 0.0;
    double strip_price_netin_pct = 0.0; 

};

struct VectorStats {
    int price_down_day_pre_max = 0;
    int price_up_day_pre_max = 0;
    int price_day_pre_max = 0;


    int price_down_day_adjacent = 0;
    int price_up_day_adjacent = 0;
    
    std::vector<int> price_day_adjacent;

    int volume_shrink_firm = 0;
    int volume_grow_firm = 0;

    int volume_shrink_loose = 0;
    int volume_grow_loose = 0;

    TradeCategoryStats a0;
    TradeCategoryStats a1;
};


struct DayOutputMetrics {
    
    std::string date_str = "";

    double am_pct_change = 0.0;
    double pct_change_base_pre = 0.0;
    double pct_change_base_925 = 0.0;
    double start_change = 0.0;
    double avg_pct_change = 0.0;
    double historical_total_inflow = 0.0;
    
    DailyMetrics middle_metrics;
    DailyMetrics metrics;
    DailyMetrics am_metrics;

};


struct file2out_st{
    std::vector<std::string> files_to_process;
    std::vector<DayOutputMetrics> out_vector;
};

struct deal_probability_distribution {
    double super = 0.0;
    double big = 0.0;
    double middle = 0.0;
    double small = 0.0;
    std::string description;
};

struct DailyDistributions {
    deal_probability_distribution vol_dist;
    deal_probability_distribution money_dist;
};


inline bool last_record(TickRecord this_record) { return this_record.time == "15:00"; }
inline bool first_record(TickRecord this_record) { return this_record.time == "09:25"; }


void collect_bs_action(bsn_action_group& group, const std::string& bs_type, double money, size_t volume, double gap, size_t tick_count);
extern void burst_new(Burst_st& burst, TickRecord record, double pre_price);
extern void update_burst(Burst_st& burst, const TickRecord& record, const TickRecord& pre_record);

extern void update_metrics_stream(tickTime time, record_stream& header, Burst_st& burst);
extern bool record_change(TickRecord this_record, const TickRecord pre_record);
extern void get_record_stream_point(record_stream& this_point, TickRecord r, double pre_price);
extern void sub_record_stream_point(record_stream& this_point, record_stream& that_point);
void metry_summary(const DayOutputMetrics& out, TradeCategoryStats& stats); 

int metrics_up_check_price_pre_max(const std::vector<DayOutputMetrics>& out_vector);
int metrics_down_check_price_pre_max(const std::vector<DayOutputMetrics>& out_vector);
int metrics_price_check_pre_max(const std::vector<DayOutputMetrics>& out_vector);

int metrics_up_check_price_adjacent(const std::vector<DayOutputMetrics>& out_vector);
int metrics_down_check_price_adjacent(const std::vector<DayOutputMetrics>& out_vector);
int metrics_price_check_adjacent(const std::vector<DayOutputMetrics>& out_vector);

int metrics_grow_loose(const std::vector<DayOutputMetrics>& out_vector);
int metrics_shrink_loose(const std::vector<DayOutputMetrics>& out_vector);
int metrics_grow_firm(const std::vector<DayOutputMetrics>& out_vector);
int metrics_shrink_firm(const std::vector<DayOutputMetrics>& out_vector);
void metry_vector_summary(const std::vector<DayOutputMetrics>& out_vector, VectorStats& stats);
void get_slim_base(const DailyMetrics& metrics, RecordScale type,  bsn_action_group& h, deal_summary &deal_summary);
void this_bsn_add(const deal_bsn& deal, trade& this_trade);
void set_metrics_record(DailyMetrics& metrics, TickRecord record, RecordType t);
double metrics_total_volume(const DailyMetrics& metrics);
double metrics_price_net(const bsn_action_group& total);
double metrics_bsn_net(const DailyMetrics& metrics);
double metrics_total_money(const DailyMetrics& metrics);
void get_daily_distributions(const DailyMetrics& metrics, DailyDistributions& result);
double get_first_record_net(const DailyMetrics& metrics);


#endif // COLLECT_STREAM_H
