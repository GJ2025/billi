#include <iostream>
#include <iomanip>
#include "common.h"
#include "tick_types.h"
#include "collect_stream.h"


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

