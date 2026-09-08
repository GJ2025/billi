#include <iostream>
#include <iomanip>
#include "common.h"
#include "time_seq.h"
#include "tick_types.h"
#include "collect_stream.h"


void calculate_total(trade& total,
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

void this_bsn_add(const deal_bsn& deal, trade& this_trade) {
    this_trade =  deal.buy +  deal.sale + deal.neutral;
}

void get_slim_base(const DailyMetrics& metrics, RecordScale type,  bsn_action_group& h, deal_summary &deal_summary){
    
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

void  deal_volume_pro_distri(const range_info& rang_info, deal_probability_distribution& abc) {

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

void  deal_money_pro_distri(const range_info& rang_info, deal_probability_distribution& abc) {

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

double get_first_record_net(const DailyMetrics& metrics){

    double total_money =  metrics.daily_first_record.volume * 100 * metrics.daily_first_record.price/WAN;

    if (metrics.daily_first_record.bs_type == "B"){

        return total_money;

    }else if (metrics.daily_first_record.bs_type == "S"){

        return 0 - total_money;

    }else{

        return 0;
    }

}

void get_range_info(range_info& rang_info, const DailyMetrics& metrics) {

    get_slim_base(metrics, RecordScale::SUPER, rang_info.super.info, rang_info.super.summary);
    get_slim_base(metrics, RecordScale::BIG,  rang_info.big.info, rang_info.big.summary);
    get_slim_base(metrics, RecordScale::MIDDLE,  rang_info.middle.info, rang_info.middle.summary);
    get_slim_base(metrics, RecordScale::SMALL,  rang_info.small.info, rang_info.small.summary);
    get_slim_base(metrics, RecordScale::TOTAL,  rang_info.total.info, rang_info.total.summary);


}

void get_daily_distributions(const DailyMetrics& metrics, DailyDistributions& result) {

    range_info rang_info;

    get_range_info(rang_info, metrics);

    deal_volume_pro_distri(rang_info, result.vol_dist);
    deal_money_pro_distri(rang_info, result.money_dist);
    return ;
}


bool record_change(const TickRecord this_record, const TickRecord pre_record) {
    
    if (this_record.bs_type != pre_record.bs_type){
        return true;
    } 

    if (this_record.bs_type == "S"){
        return this_record.price > pre_record.price;
    } 

    if (this_record.bs_type == "B"){
        return this_record.price < pre_record.price;
    }
    
    return true;
}

void burst_new(Burst_st& burst, const TickRecord record, double pre_price) {
    burst.records.clear();
    burst.records.push_back(record);
    burst.gap = record.price - pre_price;
}

void burst_add_record(Burst_st& burst, const TickRecord record) {
    burst.records.push_back(record);
}

double metrics_total_money(const DailyMetrics& metrics){

    bsn_action_group dump;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);

    return deal_summary_total.total.money;
}

double metrics_total_volume(const DailyMetrics& metrics){

    bsn_action_group dump;
    deal_summary deal_summary_total;

    get_slim_base(metrics, RecordScale::TOTAL, dump, deal_summary_total);

    return deal_summary_total.total.volume;
}

double metrics_bsn_net(const DailyMetrics& metrics){

    trade buy = metrics.header.total.buy.down + metrics.header.total.buy.keep + metrics.header.total.buy.up;
    trade sale = metrics.header.total.sale.down + metrics.header.total.sale.keep + metrics.header.total.sale.up;

    trade net_in = buy - sale;

    return net_in.money;
}

double metrics_price_net(const bsn_action_group& total){
    
    trade up =  total.buy.up + total.sale.up + total.neutral.up;
    trade down = total.sale.down +total.buy.down +  total.neutral.down;
    
    trade net_in = up - down;


    return net_in.money;
} 

void set_metrics_record(DailyMetrics& metrics, TickRecord record, RecordType t){

    if (t == RecordType::FIRST){
        metrics.daily_first_record = record;
    }else if (t == RecordType::LAST){
         metrics.daily_last_record = record;
    }
}

void collect_price_action(deal_price& rp, double trade, size_t volume, double gap, size_t tick_count) {
    if (gap < 0.0){
        rp.down.money += trade;
        rp.down.volume += volume;
        rp.down.tick_count += tick_count;
    }else if (gap == 0.0){
        rp.keep.money += trade;
        rp.keep.volume += volume;
        rp.keep.tick_count += tick_count;
    }else{
        rp.up.money += trade;
        rp.up.volume += volume;
        rp.up.tick_count += tick_count;
    } 
}

void collect_bs_action(bsn_action_group& group, const std::string& bs_type, double money, size_t volume, double gap, size_t tick_count) {
    if (bs_type == "B"){
        collect_price_action(group.buy, money, volume, gap, tick_count);
    }else if (bs_type == "S"){
        collect_price_action(group.sale, money, volume, gap, tick_count);
    } else{
        collect_price_action(group.neutral, money, volume, gap, tick_count);
    } 
}

void update_metrics_stream(tickTime time, record_stream& header, Burst_st& burst) {
    double total_money = 0.0;
    size_t total_volume = 0;
    
    if (burst.records.empty()) {
        return;
    }

    for (const auto& r : burst.records){
        total_money += (r.volume * r.price * 100.0);
        total_volume += r.volume * 100;
    } 
    
    bsn_action_group* group = (total_money > 100 * WAN) ? &header.super :
                             (total_money > 30 * WAN) ? &header.big :
                             (total_money > 5 * WAN)  ? &header.middle : &header.small;
    
    collect_bs_action(*group, burst.records[0].bs_type, total_money, total_volume, burst.gap, burst.records.size());
    collect_bs_action(header.total, burst.records[0].bs_type, total_money, total_volume, burst.gap, burst.records.size());

    header.time = time;

    burst.records.clear();
}

void get_record_stream_point(record_stream& this_point, TickRecord r, double pre_price) {
    double total_money = 0.0;
    size_t total_volume = 0;


    total_money += (r.volume * r.price * 100.0);
    total_volume += r.volume * 100;
    
    bsn_action_group* group = (total_money > 100 * WAN) ? &this_point.super :
                             (total_money > 30 * WAN) ? &this_point.big :
                             (total_money > 5 * WAN)  ? &this_point.middle : &this_point.small;
    
    collect_bs_action(*group, r.bs_type, total_money, total_volume, r.price - pre_price, 1);
    collect_bs_action(this_point.total, r.bs_type, total_money, total_volume, r.price - pre_price, 1);
}

void sub_record_stream_point(record_stream& this_point, record_stream& that_point) {
    
    auto sub_trade = [](trade& t1, const trade& t2) {
        t1.money  -= t2.money;
        t1.volume -= t2.volume;
        t1.tick_count -= t2.tick_count;
    };

    
    auto sub_deal_price = [&](deal_price& d1, const deal_price& d2) {
        sub_trade(d1.up,    d2.up);
        sub_trade(d1.down,     d2.down);
        sub_trade(d1.keep,    d2.keep);
    };

    
    auto sub_group = [&](bsn_action_group& g1, const bsn_action_group& g2) {
        sub_deal_price(g1.buy,     g2.buy);
        sub_deal_price(g1.sale,    g2.sale);
        sub_deal_price(g1.neutral, g2.neutral);
    };

    
    sub_group(this_point.super,  that_point.super);
    sub_group(this_point.big,    that_point.big);
    sub_group(this_point.middle, that_point.middle);
    sub_group(this_point.small,  that_point.small);
    sub_group(this_point.total,  that_point.total);
}


void update_burst(Burst_st& burst, const TickRecord& record, const TickRecord& pre_record) {
    if (burst.records.empty() || record_change(record, pre_record)) {

        burst_new(burst, record, pre_record.price);

    }else{

        burst_add_record(burst, record);
    }

    return;
}


void metry_summary(const DayOutputMetrics& out, TradeCategoryStats& stats){

    bsn_action_group dump;
    deal_summary summary;
    const bsn_action_group& bs = out.metrics.header.total;

    get_slim_base(out.metrics, RecordScale::TOTAL, dump, summary);

    stats.all_will_netin = metrics_bsn_net(out.metrics);
    stats.all_price_netin = metrics_price_net(out.metrics.header.total);

    stats.strip_will_netin = metrics_bsn_net(out.middle_metrics);
    stats.strip_price_netin = metrics_price_net(out.middle_metrics.header.total);


    stats.buyup_pct = pct_base(bs.buy.up.money,   summary.total.money);
    stats.buydown_pct = pct_base(bs.buy.down.money,   summary.total.money);

    stats.saleup_pct = pct_base(bs.sale.up.money,   summary.total.money);

    stats.pct_change_base_925 = out.pct_change_base_925;
    stats.pct_change_base_pre = out.pct_change_base_pre;

    return;
}

void metry_vector_summary(const std::vector<DayOutputMetrics>& out_vector, VectorStats& stats){

    stats.price_down_day_pre_max = metrics_down_check_price_pre_max(out_vector);
    stats.price_up_day_pre_max = metrics_up_check_price_pre_max(out_vector);
    stats.price_day_pre_max = metrics_price_check_pre_max(out_vector);

    stats.price_down_day_adjacent = metrics_down_check_price_adjacent(out_vector);
    stats.price_up_day_adjacent = metrics_up_check_price_adjacent(out_vector);
    stats.price_day_adjacent.push_back(metrics_price_check_adjacent(out_vector));
    stats.price_day_adjacent.push_back(metrics_price_check_adjacent(get_sub_vector(out_vector,1)));


    stats.volume_shrink_firm = metrics_shrink_firm(out_vector);
    stats.volume_grow_firm = metrics_grow_firm(out_vector);
    stats.volume_shrink_loose = metrics_shrink_loose(out_vector);
    stats.volume_grow_loose = metrics_grow_loose(out_vector);


    metry_summary(out_vector[0], stats.a0);
    metry_summary(out_vector[1], stats.a1);

    stats.a0.all_will_netin_pct = (stats.a0.all_will_netin - stats.a1.all_will_netin) / std::abs(stats.a1.all_will_netin);
    stats.a0.all_price_netin_pct = (stats.a0.all_price_netin - stats.a1.all_price_netin) / std::abs(stats.a1.all_price_netin);


    stats.a0.strip_will_netin_pct = (stats.a0.strip_will_netin - stats.a1.strip_will_netin) / std::abs(stats.a1.strip_will_netin);
    stats.a0.strip_price_netin_pct = (stats.a0.strip_price_netin - stats.a1.strip_price_netin) / std::abs(stats.a1.strip_price_netin);

    return;
}



