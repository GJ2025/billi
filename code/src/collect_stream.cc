#include "tick_types.h"
#include "collect_stream.h"
#include <iostream>
#include <iomanip>
#include "common.h"

// --- 辅助逻辑 ---
bool record_change(TickRecord this_record, TickRecord pre_record) {
    if (this_record.bs_type != pre_record.bs_type) return true;
    if (this_record.bs_type == "S") return this_record.price > pre_record.price;
    if (this_record.bs_type == "B") return this_record.price < pre_record.price;
    return true;
}

bool last_record(TickRecord this_record) { return this_record.time == "15:00"; }
bool first_record(TickRecord this_record) { return this_record.time == "09:25"; }

void stream_new(StreamRecord& stream, TickRecord record, double pre_price) {
    stream.records.clear();
    stream.records.push_back(record);
    stream.gap = record.price - pre_price;
}

// --- 核心逻辑 ---
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

void collect_bs_action(bs_action_group& group, const std::string& bs_type, double trade, size_t volume, double gap) {
    if (bs_type == "B"){
        collect_price_action(group.buy, trade, volume, gap);
    }else if (bs_type == "S"){
        collect_price_action(group.sale, trade, volume, gap);
    } else{
        collect_price_action(group.neutral, trade, volume, gap);
    } 
}

void summary_stream(struct stream_sum& sum, StreamRecord& stream) {
    double total_trade = 0.0;
    size_t total_volume = 0;
    
    if (stream.records.empty()) {
        return;
    }

    for (const auto& r : stream.records){
        total_trade += (r.volume * r.price * 100.0);
        total_volume += r.volume * 100;
    } 
    
    bs_action_group* group = (total_trade > 100 * WAN) ? &sum.super :
                             (total_trade > 30 * WAN) ? &sum.big :
                             (total_trade > 5 * WAN)  ? &sum.middle : &sum.small;
    
    collect_bs_action(*group, stream.records[0].bs_type, total_trade, total_volume, stream.gap);
}

void update_stream_and_metrics(DailyMetrics& metrics, StreamRecord& stream, 
                               TickRecord& record, TickRecord& pre_record) {
    if (first_record(record)) {
        pre_record = record;
        metrics.first_record = record;
        stream_new(stream, record, record.price);
    }

    if (record_change(record, pre_record)) {
        summary_stream(metrics.stream_sum_info, stream);
        stream_new(stream, record, pre_record.price);
    } else if (!first_record(record)) {
        stream.records.push_back(record);
    }


    if (last_record(record)) {
        metrics.last_record = record;
        summary_stream(metrics.stream_sum_info, stream);
    }

    pre_record = record;
}

void deal_classfy(DayOutputMetrics& out) {

    auto fill_bsn = [&](deal_bsn& dest, const bs_action_group& src) {
        dest.buy.money = sum_money(src.buy);
        dest.sale.money = sum_money(src.sale);
        dest.neutral.money = sum_money(src.neutral);

        dest.buy.volume = sum_volume(src.buy);
        dest.sale.volume = sum_volume(src.sale);
        dest.neutral.volume = sum_volume(src.neutral);
    };

    fill_bsn(out.deal_super_bsn, out.stream_sum_info.super);
    fill_bsn(out.deal_big_bsn, out.stream_sum_info.big);
    fill_bsn(out.deal_middle_bsn, out.stream_sum_info.middle);
    fill_bsn(out.deal_small_bsn, out.stream_sum_info.small);

    sum_bsn_buy(out.deal_super_bsn, out.deal_big_bsn, out.deal_middle_bsn, out.deal_small_bsn, out.deal_total_bsn);
    sum_bsn_sale(out.deal_super_bsn, out.deal_big_bsn, out.deal_middle_bsn, out.deal_small_bsn, out.deal_total_bsn);
    sum_bsn_neutral(out.deal_super_bsn, out.deal_big_bsn, out.deal_middle_bsn, out.deal_small_bsn, out.deal_total_bsn);

  
    auto fill_price = [](deal_price& dest, const bs_action_group& src) {
        dest.up.money   = src.buy.up.money   + src.sale.up.money   + src.neutral.up.money;
        dest.down.money = src.buy.down.money + src.sale.down.money + src.neutral.down.money;
        dest.keep.money = src.buy.keep.money + src.sale.keep.money + src.neutral.keep.money;

        dest.up.volume   = src.buy.up.volume   + src.sale.up.volume   + src.neutral.up.volume;
        dest.down.volume = src.buy.down.volume + src.sale.down.volume + src.neutral.down.volume;
        dest.keep.volume = src.buy.keep.volume + src.sale.keep.volume + src.neutral.keep.volume;
    };

    fill_price(out.deal_super_price,  out.stream_sum_info.super);
    fill_price(out.deal_big_price,    out.stream_sum_info.big);
    fill_price(out.deal_middle_price, out.stream_sum_info.middle);
    fill_price(out.deal_small_price,  out.stream_sum_info.small);

    sum_price_up(out.deal_super_price, out.deal_big_price, out.deal_middle_price,out. deal_small_price, out.deal_total_price);
    sum_price_down(out.deal_super_price, out.deal_big_price, out.deal_middle_price,out. deal_small_price, out.deal_total_price);
    sum_price_keep(out.deal_super_price, out.deal_big_price, out.deal_middle_price,out. deal_small_price, out.deal_total_price);
}

