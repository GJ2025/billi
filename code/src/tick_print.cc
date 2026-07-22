#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>
#include <filesystem> 
#include <algorithm>  
#include <cmath>       
#include <cstdlib>   
#include <unistd.h> // 这是关键头文件
#include "common.h"
#include "tick_types.h"
#include "collect_stream.h"

void print_table_header() {
    std::cout << std::left << std::setw(12) << "Date" << " | "
              << std::right 
              << std::setw(12) << "Pre924" << " | "
              << std::setw(10) << "924"    << " | "
              << std::setw(10) << "925"    << " | "
              << std::setw(12) << "ChgPreDay" << " | "
              << std::setw(12) << "ChgPre924" << " | "
               << std::setw(6) << "b/s" << " | "
              << std::setw(12) << "Chg924"    << " | "
              << std::setw(12) << "PctChg" 
              << std::endl;
    // 打印分割线，让视觉效果更专业
    std::cout << std::string(88, '-') << std::endl;
}

void print_header_info(const DayOutputMetrics& out, const DayOutputMetrics& pre_out) {
    double price_change_pct = 0.0;
    double ratio_change_pre_day = 0.0;
    if (pre_out.pm_closing_price != 0.0) {
        price_change_pct = ((out.pm_closing_price - pre_out.pm_closing_price) / pre_out.pm_closing_price) * 100.0;
        ratio_change_pre_day = ((out.head_data.v_925.price - pre_out.pm_closing_price) / pre_out.pm_closing_price) * 100.0;

    }

    std::cout << std::left  << std::setw(12) << out.date_str << " | "
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(12) << out.head_data.pre_924.price << " | "
              << std::setw(10) << out.head_data.v_924.price   << " | "
              << std::setw(10) << out.head_data.v_925.price   << " | "
              << std::setw(11) << out.head_data.ratio_change_pre_924 << "% | "
              << std::setw(11) << ratio_change_pre_day << "% | "
              << std::setw(6) << out.head_data.v_925.bs_type << " | "
              << std::setw(11) << out.head_data.ratio_change_924 << "% | "
              << std::setw(11) << price_change_pct << "%"
              << std::endl;
}

void print_all() {
    // 总长度再次扩展 14 字符以兼容新的占比列
    std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl; 
    std::cout << std::left  << std::setw(11) << "Date" << " | "
              << std::right << std::setw(5)  << "Ticks" << " | "
              << std::setw(9)  << "Vol" << " | "               
              << std::setw(9)  << "AM_Vol" << " | "
              << std::setw(11) << "AM_Turnover" << " | "  
              << std::setw(11) << "AM_Turn%" << " | " // 新增表头：上午成交额占比
              << std::setw(10) << "Vol/Ticks" << " | "         
              << std::setw(11) << "Turnover" << " | "
              << std::setw(8)  << "AM_Close" << " | "  
              << std::setw(9)  << "Avg_Price" << " | " 
              << std::setw(7)  << "Close" << " | "  
              << std::setw(8)  << "AM_Chan%" << " | "               
              << std::setw(8)  << "Change%" << " | "     
              << std::setw(10) << "Net_In" << " | "     
              << std::setw(10) << "AM_Net_In" << " | "
              << std::setw(10) << "PM_Net_In" << " | " 
              << std::setw(9) << "Net_In%" << " | " 
              << std::setw(9) << "Net/change" << " | " 
              << std::setw(11) << "Hist_Cum" << " | "   
              << std::left  << std::setw(20) << "Signal" 
              << std::endl;
    std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl;
}

void print_headers(const ProgramOptions& opts) {
    if (opts.show_head){
        print_table_header();
    }  
    if (opts.show_all){
        print_will_price_header("ALL", data_row_table_cols);
    }   
    if (opts.show_will){
        print_will_price_header("WILL", will_table_cols);
    }   
    if (opts.show_price){
        print_will_price_header("PRICE ", price_table_cols);
    }   
    if (opts.show_merge){
        print_will_price_header("MERGE ", merge_table_cols) ;
    }   
    if (opts.show_super ){
        print_will_price_header("SUPER", will_price_table_cols);
    }
    
    if (opts.show_big){
        print_will_price_header("BIG", will_price_table_cols);
    }
    
    if (opts.show_middle){
        print_will_price_header("MIDDLE", will_price_table_cols);
    } 
}


void print_bodys(const ProgramOptions& opts, DayOutputMetrics& out, const DayOutputMetrics& prev_out, std::string divergence)  {
        if (opts.show_head){
            print_header_info(out, prev_out);
        }  

        if (opts.show_all){
            print_data(out, divergence,data_row_table_cols);
        }

        if (opts.show_will){
            print_will(out, will_table_cols);
        }  

        if (opts.show_price){
            print_price(out, price_table_cols);
        } 

        if (opts.show_merge){
            print_merge(out, merge_table_cols);
        } 

        if (opts.show_super){
            print_slim_price(out, out.stream_sum_info.super, out.deal_super_bsn, out.deal_super_price,will_price_table_cols);
        }

        if (opts.show_big){
            print_slim_price(out, out.stream_sum_info.big, out.deal_big_bsn,  out.deal_big_price,will_price_table_cols);
        } 

        if (opts.show_middle){
            print_slim_price(out, out.stream_sum_info.middle, out.deal_middle_bsn,  out.deal_middle_price, will_price_table_cols);
        } 
}
