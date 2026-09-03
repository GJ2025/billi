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
    tickTime what;
    bsn_action_group super;
    bsn_action_group big;
    bsn_action_group middle;
    bsn_action_group small;
    bsn_action_group total;
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

struct signal_info {
    double all_will_netin = 0.0;
    double all_price_netin = 0.0;

    double will_netin_change = 0.0;
    double price_netin_change = 0.0;
    size_t shrink_firm;
    size_t grow_firm;
    size_t shrink_loose;
    size_t grow_loose;
    int price_day;
    std::string display_file;
    DayOutputMetrics out;
    std::string trigger_reason;
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


inline void calculate_total(trade& total,
                     const trade& b_down, const trade& b_up, const trade& b_keep,
                     const trade& s_down, const trade& s_up, const trade& s_keep,
                     const trade& n_down, const trade& n_up, const trade& n_keep) 
{
    total = {}; 

    total += b_down;
    total += b_up;
    total += b_keep;

    total += s_down;
    total += s_up;
    total += s_keep;

    total += n_down;
    total += n_up;
    total += n_keep;
}

inline void this_bsn_add(const deal_bsn& deal, trade& this_trade) {
    this_trade =  deal.buy +  deal.sale + deal.neutral;
}

inline void get_slim_base(const DailyMetrics& metrics, RecordScale type,  bsn_action_group& h, deal_summary &deal_summary){
    
    const bsn_action_group& t = metrics.header.total;

    if (type == RecordScale::SUPER){
        h = metrics.header.super;
    }else if(type == RecordScale::BIG){
        h = metrics.header.big;
    }else if(type == RecordScale::MIDDLE){
        h = metrics.header.middle;
    }else if(type == RecordScale::SMALL){
        h = metrics.header.small;
    }else{
        h = metrics.header.total;
    }

    deal_summary.bsn.buy = h.buy.down + h.buy.keep + h.buy.up;
    deal_summary.bsn.neutral = h.neutral.down + h.neutral.keep + h.neutral.up;
    deal_summary.bsn.sale = h.sale.down + h.sale.keep + h.sale.up;

    deal_summary.price.down = h.buy.down + h.sale.down + h.neutral.down;
    deal_summary.price.up = h.buy.up + h.sale.up + h.neutral.up;
    deal_summary.price.keep = h.buy.keep + h.sale.keep + h.neutral.keep;

    calculate_total(deal_summary.total, t.buy.down, t.buy.up, t.buy.keep, t.sale.down, t.sale.up, t.sale.keep, t.neutral.down, t.neutral.up, t.neutral.keep);
    calculate_total(deal_summary.type_total, h.buy.down, h.buy.up, h.buy.keep, h.sale.down, h.sale.up, h.sale.keep, h.neutral.down, h.neutral.up, h.neutral.keep);


    return;
}

inline void  deal_volume_pro_distri(const range_info& rang_info, deal_probability_distribution& abc) {

    if (rang_info.super.summary.total.volume > 0) {
        abc.super   = (double)rang_info.super.summary.type_total.volume / rang_info.total.summary.total.volume;
        abc.big     = (double)rang_info.big.summary.type_total.volume / rang_info.total.summary.total.volume;
        abc.middle  = (double)rang_info.middle.summary.type_total.volume / rang_info.total.summary.total.volume;
        abc.small   = (double)rang_info.small.summary.type_total.volume / rang_info.total.summary.total.volume;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    oss <<  (abc.super * 100) <<
        "  " << (abc.big * 100) <<
        "  " << (abc.middle * 100) << 
        "  " << (abc.small * 100) ;
    abc.description = oss.str();

    return;
}

inline void  deal_money_pro_distri(const range_info& rang_info, deal_probability_distribution& abc) {

    if (rang_info.super.summary.total.money > 0) {
        abc.super   = (double)rang_info.super.summary.type_total.money / rang_info.total.summary.total.money;
        abc.big     = (double)rang_info.big.summary.type_total.money / rang_info.total.summary.total.money;
        abc.middle  = (double)rang_info.middle.summary.type_total.money / rang_info.total.summary.total.money;
        abc.small   = (double)rang_info.small.summary.type_total.money / rang_info.total.summary.total.money;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    oss <<  (abc.super * 100) <<
        "  " << (abc.big * 100) <<
        "  " << (abc.middle * 100) << 
        "  " << (abc.small * 100) ;
    abc.description = oss.str();

    return;
}

inline double get_first_record_net(const DailyMetrics& metrics){

    double total_money =  metrics.daily_first_record.volume * 100 * metrics.daily_first_record.price/WAN;

    if (metrics.daily_first_record.bs_type == "B"){

        return total_money;

    }else if (metrics.daily_first_record.bs_type == "S"){

        return 0 - total_money;

    }else{

        return 0;
    }

}

inline void get_range_info(range_info& rang_info, const DailyMetrics& metrics) {

    get_slim_base(metrics, RecordScale::SUPER, rang_info.super.info, rang_info.super.summary);
    get_slim_base(metrics, RecordScale::BIG,  rang_info.big.info, rang_info.big.summary);
    get_slim_base(metrics, RecordScale::MIDDLE,  rang_info.middle.info, rang_info.middle.summary);
    get_slim_base(metrics, RecordScale::SMALL,  rang_info.small.info, rang_info.small.summary);
    get_slim_base(metrics, RecordScale::TOTAL,  rang_info.total.info, rang_info.total.summary);


}

inline void get_daily_distributions(const DailyMetrics& metrics, DailyDistributions& result) {

    range_info rang_info;

    get_range_info(rang_info, metrics);

    deal_volume_pro_distri(rang_info, result.vol_dist);
    deal_money_pro_distri(rang_info, result.money_dist);
    return ;
}

inline double metrics_total_money(const DailyMetrics& metrics){

    bsn_action_group dump;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);

    return deal_summary_total.total.money;
}

inline double metrics_total_volume(const DailyMetrics& metrics){

    bsn_action_group dump;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);

    return deal_summary_total.total.volume;
}

inline double metrics_bsn_net(const DailyMetrics& metrics){

    trade buy = metrics.header.total.buy.down + metrics.header.total.buy.keep + metrics.header.total.buy.up;
    trade sale = metrics.header.total.sale.down + metrics.header.total.sale.keep + metrics.header.total.sale.up;

    trade net_in = buy - sale;

    return net_in.money;
}

inline double metrics_price_net(const bsn_action_group& total){
    
    trade up =  total.buy.up + total.sale.up + total.neutral.up;
    trade down = total.sale.down +total.buy.down +  total.neutral.down;
    
    trade net_in = up - down;


    return net_in.money;
} 

inline void set_metrics_record(DailyMetrics& metrics, TickRecord record, RecordType t){

    if (t == RecordType::FIRST){
        metrics.daily_first_record = record;
    }else if (t == RecordType::LAST){
         metrics.daily_last_record = record;
    }

}

inline bool last_record(TickRecord this_record) { return this_record.time == "15:00"; }
inline bool first_record(TickRecord this_record) { return this_record.time == "09:25"; }


void collect_bs_action(bsn_action_group& group, const std::string& bs_type, double money, size_t volume, double gap, size_t tick_count);
void update_stream(StreamRecord& stream, const TickRecord& record, const TickRecord& pre_record);

extern void update_metrics_header(tickTime what, record_stream& header, StreamRecord& stream);
extern bool record_change(TickRecord this_record, const TickRecord pre_record);
extern void stream_new(StreamRecord& stream, TickRecord record, double pre_price);
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


#endif // COLLECT_STREAM_H
