#include <iostream>
#include <iomanip>
#include "common.h"
#include "tick_types.h"
#include "collect_stream.h"

void print_will( DayOutputMetrics& out) {

    double jing_super = out.deal_super_bsn.buy - out.deal_super_bsn.sale;
    double jing_big = out.deal_big_bsn.buy - out.deal_big_bsn.sale;
    double jing_middle = out.deal_middle_bsn.buy - out.deal_middle_bsn.sale;
    double jing_small = out.deal_small_bsn.buy - out.deal_small_bsn.sale;
    double jing_total = out.deal_total_bsn.buy - out.deal_total_bsn.sale;

    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)
                << "buy "
                << std::setw(12)  << out.deal_super_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_super_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_super/WAN  << std::noshowpos << " | "

                << std::setw(12)  << out.deal_big_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_big_bsn.sale/WAN << " | "
                << std::showpos  << std::setw(9)  << jing_big/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_middle_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_middle_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_middle/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_small_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_small_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_small/WAN << std::noshowpos << " | "

                << std::setw(9)  << out.deal_total_bsn.buy/WAN << " | "
                << std::setw(9)  << out.deal_total_bsn.sale/WAN << " | "
                << std::showpos << std::setw(9)  << jing_total/WAN << std::noshowpos << " | "

                << std::setw(12)  << (out.deal_total_bsn.buy+out.deal_total_bsn.sale+out.deal_total_bsn.neutral)/WAN << " | "  
                << std::setw(12)  << out.total_vol_wan << " | " 

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}

void print_merge( DayOutputMetrics& out) {


    std::cout << std::left << std::setw(11) << out.date_str << " | "
                << std::fixed << std::setprecision(2)

                << "buy "
                << std::setw(12)  << out.deal_total_bsn.buy/WAN << " | "
                << std::setw(12)  << out.deal_total_bsn.sale/WAN << " | "
                << std::setw(12)  << out.deal_total_bsn.neutral/WAN << " | "
                << "up "
                << std::setw(12)  << out.deal_total_price.up/WAN << " | "
                << std::setw(12)  << out.deal_total_price.down/WAN << " | "
                << std::setw(12)  << out.deal_total_price.keep/WAN << " | "

                << std::setw(16)  << std::showpos <<(out.deal_total_bsn.buy - out.deal_total_bsn.sale)/WAN << " | " << std::noshowpos
                << std::setw(16)  << std::showpos <<(out.deal_total_price.up - out.deal_total_price.down)/WAN << " | " << std::noshowpos

  
                << std::setw(12)  << (out.deal_total_price.down+out.deal_total_price.up+out.deal_total_price.keep)/WAN << " | "  
                << std::setw(12)  << out.total_vol_wan << " | " 

                << std::setw(5)  << out.pre_closing_price << " | " 
                << std::setw(9)  << std::showpos << out.start_change << " | " << std::noshowpos
                << std::setw(9)  << std::showpos << out.pct_change << " | "  << std::noshowpos
                << std::setw(5)  << out.closing_price << " | " 
                << std::endl;

}

