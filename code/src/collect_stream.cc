#include <iostream>
#include <iomanip>
#include "common.h"
#include "time_seq.h"
#include "tick_types.h"
#include "collect_stream.h"



bool record_change(TickRecord this_record, const TickRecord pre_record) {
    
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



void stream_new(StreamRecord& stream, TickRecord record, double pre_price) {
    stream.records.clear();
    stream.records.push_back(record);
    stream.gap = record.price - pre_price;
}

void stream_add_record(StreamRecord& stream, TickRecord record) {
    stream.records.push_back(record);
}


void collect_price_action(deal_price& rp, double trade, size_t volume, double gap) {
    if (gap < 0.0){
        rp.down.money += trade;
        rp.down.volume += volume;
    }else if (gap == 0.0){
        rp.keep.money += trade;
        rp.keep.volume += volume;
    }else{
        rp.up.money += trade;
        rp.up.volume += volume;
    } 
}

void collect_bs_action(bsn_action_group& group, const std::string& bs_type, double trade, size_t volume, double gap) {
    if (bs_type == "B"){
        collect_price_action(group.buy, trade, volume, gap);
    }else if (bs_type == "S"){
        collect_price_action(group.sale, trade, volume, gap);
    } else{
        collect_price_action(group.neutral, trade, volume, gap);
    } 
}

void update_metrics_header(record_stream& header, StreamRecord& stream) {
    double total_trade = 0.0;
    size_t total_volume = 0;
    
    if (stream.records.empty()) {
        return;
    }

    for (const auto& r : stream.records){
        total_trade += (r.volume * r.price * 100.0);
        total_volume += r.volume * 100;
    } 
    
    bsn_action_group* group = (total_trade > 100 * WAN) ? &header.super :
                             (total_trade > 30 * WAN) ? &header.big :
                             (total_trade > 5 * WAN)  ? &header.middle : &header.small;
    
    collect_bs_action(*group, stream.records[0].bs_type, total_trade, total_volume, stream.gap);

    stream.records.clear();
}


void update_stream(StreamRecord& stream, TickRecord& record, const TickRecord& pre_record) {
    
    if (stream.records.empty() || first_record(record) || record_change(record, pre_record)) {

        stream_new(stream, record, pre_record.price);

    }else{

        stream_add_record(stream, record);
    }

    return;
}

void deal_classfy(DailyMetrics& metrics) {

    auto fill_bsn = [&](deal_bsn& dest, const bsn_action_group& src) {
        dest.buy.money = sum_money(src.buy);
        dest.sale.money = sum_money(src.sale);
        dest.neutral.money = sum_money(src.neutral);

        dest.buy.volume = sum_volume(src.buy);
        dest.sale.volume = sum_volume(src.sale);
        dest.neutral.volume = sum_volume(src.neutral);
    };

    fill_bsn(metrics.deal_super_bsn, metrics.header.super);
    fill_bsn(metrics.deal_big_bsn, metrics.header.big);
    fill_bsn(metrics.deal_middle_bsn, metrics.header.middle);
    fill_bsn(metrics.deal_small_bsn, metrics.header.small);

    sum_bsn_buy(metrics.deal_super_bsn, metrics.deal_big_bsn, metrics.deal_middle_bsn, metrics.deal_small_bsn, metrics.deal_total_bsn);
    sum_bsn_sale(metrics.deal_super_bsn, metrics.deal_big_bsn, metrics.deal_middle_bsn, metrics.deal_small_bsn, metrics.deal_total_bsn);
    sum_bsn_neutral(metrics.deal_super_bsn, metrics.deal_big_bsn, metrics.deal_middle_bsn, metrics.deal_small_bsn, metrics.deal_total_bsn);

  
    auto fill_price = [](deal_price& dest, const bsn_action_group& src) {
        dest.up.money   = src.buy.up.money   + src.sale.up.money   + src.neutral.up.money;
        dest.down.money = src.buy.down.money + src.sale.down.money + src.neutral.down.money;
        dest.keep.money = src.buy.keep.money + src.sale.keep.money + src.neutral.keep.money;

        dest.up.volume   = src.buy.up.volume   + src.sale.up.volume   + src.neutral.up.volume;
        dest.down.volume = src.buy.down.volume + src.sale.down.volume + src.neutral.down.volume;
        dest.keep.volume = src.buy.keep.volume + src.sale.keep.volume + src.neutral.keep.volume;
    };

    fill_price(metrics.deal_super_price,  metrics.header.super);
    fill_price(metrics.deal_big_price,    metrics.header.big);
    fill_price(metrics.deal_middle_price, metrics.header.middle);
    fill_price(metrics.deal_small_price,  metrics.header.small);

    sum_price_up(metrics.deal_super_price, metrics.deal_big_price, metrics.deal_middle_price, metrics.deal_small_price, metrics.deal_total_price);
    sum_price_down(metrics.deal_super_price, metrics.deal_big_price, metrics.deal_middle_price, metrics.deal_small_price, metrics.deal_total_price);
    sum_price_keep(metrics.deal_super_price, metrics.deal_big_price, metrics.deal_middle_price, metrics.deal_small_price, metrics.deal_total_price);


    

}

