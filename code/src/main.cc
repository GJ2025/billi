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
#include <unistd.h> 
#include "common.h"
#include "tick_types.h"
#include "collect_stream.h"
#include "tick_print.h"
#include "process_files.h"

namespace fs = std::filesystem;

bool is_loading_data(const std::string& str) {
    if (str.empty()) return false;
    return std::isdigit(static_cast<unsigned char>(str[0]));
}


std::string extract_company_id(const std::string& filename) {
    std::string pure_name = fs::path(filename).stem().string(); 
    
    size_t underscore_pos = pure_name.find('_');
    if (underscore_pos != std::string::npos) {
        std::string part1 = pure_name.substr(0, underscore_pos);
        std::string part2 = pure_name.substr(underscore_pos + 1);
        
        if (part1.find('-') != std::string::npos) return part2;
        if (part2.find('-') != std::string::npos) return part1;
        
        return (part1.length() < part2.length()) ? part1 : part2;
    }
    
    return pure_name;
}

bool check_company_id_match(const std::string& file_path, const std::string& target_company_id) {
    std::string current_company_id = extract_company_id(file_path);
    if (current_company_id != target_company_id) {
        std::cout << "[Skip File] Mismatched Company (" 
                  << current_company_id << " != " << target_company_id 
                  << "): " << fs::path(file_path).filename().string() << std::endl;
        return false;
    }
    return true;
}


std::string get_divergence_string(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) {
    std::vector<std::string> signals;

    if (out.pct_change > 0 && out.net_inflow_wan < 0) {
        signals.push_back("[UP_OUT]");
    } else if (out.pct_change < 0 && out.net_inflow_wan > 0) {
        signals.push_back("[DN_IN]");
    }

    if (prev_out.avg_price > 0.0) {
        if (out.avg_price > prev_out.avg_price && out.net_inflow_wan < 0) {
            signals.push_back("[AVUP_OUT]");
        } else if (out.avg_price < prev_out.avg_price && out.net_inflow_wan > 0) {
            signals.push_back("[AVDN_IN]");
        }
    }

    if (signals.empty()) {
        return "      -      ";
    }

    std::string result;
    for (size_t i = 0; i < signals.size(); ++i) {
        result += signals[i];
        if (i < signals.size() - 1) result += " ";
    }
    return result;
}

bool is_filled_tick(const DayOutputMetrics& out){
    bool had_one = (out.metrics.ticks_count > 0 && out.metrics.closing_price > 0.0);
    return had_one;
}

std::string get_and_print_signals(DayOutputMetrics& out, const DayOutputMetrics& prev_out) {

    std::string divergence_str = get_divergence_string(out, prev_out); 
    return divergence_str;
}

void update_head_tick_data(HeadTickData& head_data, const TickRecord& record) {
    if (record.time.find("09:24") == 0) {
        head_data.pre_924 = head_data.v_924;
        head_data.v_924 = record;
    } else if (record.time.find("09:25") == 0) {
        head_data.v_925 = record;
    }
}

void calculate_head_tick_changes(HeadTickData& head_data) {
    auto calc_pct = [](double target, double base) -> double {
        if (base <= 0.0){
            return 0.0;
        }    
        
        return ((target - base) / base) * 100.0;
    };

    if (!head_data.v_924.time.empty()) {
        head_data.ratio_change_924 = calc_pct(head_data.v_925.price, head_data.v_924.price);
    }
    
    if (!head_data.pre_924.time.empty()) {
        head_data.ratio_change_pre_924 = calc_pct(head_data.v_925.price, head_data.pre_924.price);
    }
}

void process_head_data(DailyMetrics& metrics, const TickRecord& record) {
    if (metrics.head_data.v_925.time.empty()) {
        if (record.time.find("09:24") == 0 || record.time.find("09:25") == 0) {
            update_head_tick_data(metrics.head_data, record);
        }
    }
    
    if (!metrics.head_data.v_925.time.empty() && !metrics.head_calculated) {
        calculate_head_tick_changes(metrics.head_data);
        metrics.head_calculated = true; 
    }
}

void update_metrics_by_record(DailyMetrics& metrics, TickRecord& record){
    metrics.ticks_count++;
    metrics.closing_price = record.price;

    long long current_vol = record.volume * 100; 
    double current_amount = record.price * current_vol;

  if (is_am_time(record.t)) {

        if (record.bs_type == "B"){
            metrics.am_inflow += current_amount;        
        }else if (record.bs_type == "S"){
            metrics.am_outflow += current_amount;    
        } 
    }else {

        if (record.bs_type == "B"){
            metrics.pm_inflow += current_amount;        
        }else if (record.bs_type == "S"){
            metrics.pm_outflow += current_amount;    
        } 
    }



    return;
}

void parse_tick_file(std::ifstream& infile, DailyMetrics& metrics, DailyMetrics& am_metrics) {
    std::string line;
    TickRecord pre_record;
    StreamRecord stream;

    std::getline(infile, line); 
    std::getline(infile, line);
    while (std::getline(infile, line)) {
        if (line.empty()){
            continue;
        } 

        std::stringstream ss(line);
        TickRecord record;
        
        
        if (ss >> record) {
            if (!is_loading_data(record.time)){
                continue;        
            } 

            process_head_data(metrics, record);

            if (record.deal_count == 0){
                continue;
            }  
            if (after_15(record.t)){
                continue;
            } 

            if (first_record(record)) {
                pre_record = record;
            }

            update_stream_and_metrics(metrics, stream, record, pre_record);
            update_metrics_by_record(metrics, record);

            if (is_am_end(record.t, pre_record.t)){
                am_metrics = metrics;
                // double total_buy_money = 0.0;

                //  deal_classfy(metrics);

                // total_buy_money =  metrics.deal_super_bsn.buy.money +  metrics.deal_big_bsn.buy.money +  metrics.deal_middle_bsn.buy.money+  metrics.deal_small_bsn.buy.money;
                // std::cout << line <<" (this_time, pre_time)--> (" 
                //              << record.time << ", " <<  pre_record.time 
                //              <<")  (metrics.am_inflow, total_buy_money)--->("
                //              << metrics.am_inflow << ", " << total_buy_money 
                //              <<")"<<std::endl;
            }

        }else{
                std::cout << "failed=========== " << record.time << std::endl;
                exit(0);
        }

        pre_record = record;
    }
    infile.close();
}

bool process_single_file(const std::string& filename, DayOutputMetrics& out) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    DailyMetrics metrics;
    DailyMetrics am_metrics;

    parse_tick_file(infile, metrics, am_metrics);

    if (metrics.ticks_count == 0) {
        return false;
    }

    if (out.total_vol_wan > 0.0) {
        out.avg_price = out.total_turnover_wan / out.total_vol_wan;
    }

    if (out.total_turnover_wan > 0.0) {
        out.inflow_ratio = (out.net_inflow_wan / out.total_turnover_wan) * BAI;
    }

    out.avg_vol_per_tick = (out.total_vol_wan * WAN) / metrics.ticks_count; 

    
    std::string pure_name = fs::path(filename).filename().string();
    out.date_str = (pure_name.length() >= 10) ? pure_name.substr(0, 10) : pure_name;

    out.metrics = metrics;
    out.am_metrics = am_metrics;

    return true;
}

void process_out(DayOutputMetrics& out, DayOutputMetrics& prev_out){

        out.historical_total_inflow = prev_out.historical_total_inflow + out.net_inflow_wan;
        if (is_filled_tick(prev_out)) {
            out.pct_change =pct(out.metrics.closing_price, prev_out.metrics.closing_price);
            out.am_pct_change = pct(out.am_metrics.closing_price, prev_out.metrics.closing_price);
            out.start_change = pct(out.metrics.first_record.price, prev_out.metrics.closing_price);
            out.pre_closing_price = prev_out.metrics.closing_price;
        }

        if (std::abs(out.pct_change) <= 1.0) {
            out.net_per_change = 0.0;
        } else {
            out.net_per_change = out.inflow_ratio / out.pct_change;
        }

        return;
}

void make_test(DayOutputMetrics& out){

    int i = 0;
    bool should_exist = false;
    const std::vector<Col>& cols = test_table_cols;

    if (out.metrics.am_inflow != out.am_metrics.deal_total_bsn.buy.money){
        print__headers("TEST", test_table_cols);
        print_next(out.date_str, i, cols);
        print_next(out.metrics.ticks_count, i, cols);

        print_next(out.metrics.am_inflow/WAN, i, cols);
        print_next(out.am_metrics.deal_total_bsn.buy.money/WAN, i, cols);

        print_next(out.metrics.am_outflow/WAN, i, cols);
        print_next(out.am_metrics.deal_total_bsn.sale.money/WAN, i, cols);

        print_next(out.metrics.pm_inflow/WAN, i, cols);
        print_next((out.metrics.deal_total_bsn.buy.money - out.am_metrics.deal_total_bsn.buy.money)/WAN, i, cols);

        print_next(out.metrics.pm_outflow/WAN, i, cols);
        print_next((out.metrics.deal_total_bsn.sale.money - out.am_metrics.deal_total_bsn.sale.money)/WAN, i, cols);
        std::cout << std::endl;
        
        should_exist = true;
        
        print__headers("TEST", test_table_cols);
    }


    if (should_exist){
        exit(0);
    }


    return;
}

int main(int argc, char* argv[]) {
    ProgramOptions opts;
    std::vector<std::string> files_to_process;
    int opt;

    while ((opt = getopt(argc, argv, "hd:parwsmbl:M")) != -1) {
        switch (opt) {
            case 'h': opts.show_head = true; break;
            case 'd': opts.dir_path = optarg; break;
            case 'r': opts.show_income_ratio = true; break;
            case 'a': opts.show_all = true; break;
            case 'w': opts.show_will = true; break;
            case 'p': opts.show_price = true; break;
            case 'm': opts.show_merge = true; break;
            case 's': opts.show_super = true; break;
            case 'b': opts.show_big = true; break;
            case 'M': opts.show_middle = true; break;
            case 'l': 
                opts.show_limit = std::stoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-h] [-d path] [-p] [-a] [-r] [-w] [-s] [-m]" << std::endl;
                return 1;
        }
    }

    int init_status = initialize_and_get_files(opts.dir_path, files_to_process);
    if (init_status > 0) return init_status;
    if (init_status < 0) return 0;

    print_headers(opts);

    DayOutputMetrics prev_out;
    std::string divergengce;
    std::string target_company_id = extract_company_id(files_to_process[0]);

    size_t total_files = files_to_process.size();
    size_t start_index = (total_files > opts.show_limit) ? (total_files - opts.show_limit) : 0;
    size_t current_idx = 0;

    for (const auto& file : files_to_process) {

        if (!check_company_id_match(file, target_company_id)) {
            continue;
        }

        if (current_idx++ < start_index){
            continue;
        }

        DayOutputMetrics out;

        
        if (!process_single_file(file, out)) {
            continue;
        }

        process_out(out, prev_out);

        deal_classfy(out.metrics);
        deal_classfy(out.am_metrics);

        make_test(out);

        divergengce = get_and_print_signals(out, prev_out);

        print_bodys(opts, out, prev_out, divergengce);

        if (out.metrics.ticks_count > 0) {
            prev_out = out;
        }
    }

    print_headers(opts);

    return 0;
}