#include <iostream>
#include <iomanip>
#include "common.h"
#include "time_seq.h"
#include "tick_types.h"
#include "collect_stream.h"

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



void stream_new(StreamRecord& stream, const TickRecord record, double pre_price) {
    stream.records.clear();
    stream.records.push_back(record);
    stream.gap = record.price - pre_price;
}

void stream_add_record(StreamRecord& stream, const TickRecord record) {
    stream.records.push_back(record);
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

void update_metrics_header(record_stream& header, StreamRecord& stream) {
    double total_money = 0.0;
    size_t total_volume = 0;
    
    if (stream.records.empty()) {
        return;
    }

    for (const auto& r : stream.records){
        total_money += (r.volume * r.price * 100.0);
        total_volume += r.volume * 100;
    } 
    
    bsn_action_group* group = (total_money > 100 * WAN) ? &header.super :
                             (total_money > 30 * WAN) ? &header.big :
                             (total_money > 5 * WAN)  ? &header.middle : &header.small;
    
    collect_bs_action(*group, stream.records[0].bs_type, total_money, total_volume, stream.gap, stream.records.size());
    collect_bs_action(header.total, stream.records[0].bs_type, total_money, total_volume, stream.gap, stream.records.size());

    stream.records.clear();
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
    // 定义一个 Lambda 表达式用于处理单个 trade 的相减
    auto sub_trade = [](trade& t1, const trade& t2) {
        t1.money  -= t2.money;
        t1.volume -= t2.volume;
        t1.tick_count -= t2.tick_count;
    };

    // 定义一个 Lambda 表达式处理整个 deal_price
    auto sub_deal_price = [&](deal_price& d1, const deal_price& d2) {
        sub_trade(d1.up,    d2.up);
        sub_trade(d1.down,     d2.down);
        sub_trade(d1.keep,    d2.keep);
    };

    // 定义一个 Lambda 表达式处理整个 bsn_action_group
    auto sub_group = [&](bsn_action_group& g1, const bsn_action_group& g2) {
        sub_deal_price(g1.buy,     g2.buy);
        sub_deal_price(g1.sale,    g2.sale);
        sub_deal_price(g1.neutral, g2.neutral);
    };

    // 对所有分组执行相减
    sub_group(this_point.super,  that_point.super);
    sub_group(this_point.big,    that_point.big);
    sub_group(this_point.middle, that_point.middle);
    sub_group(this_point.small,  that_point.small);
    sub_group(this_point.total,  that_point.total);
}


void update_stream(StreamRecord& stream, const TickRecord& record, const TickRecord& pre_record) {
    if (stream.records.empty() || record_change(record, pre_record)) {

        stream_new(stream, record, pre_record.price);

    }else{

        stream_add_record(stream, record);
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

}

