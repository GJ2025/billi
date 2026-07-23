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

bool is_am_time(const tickTime& t) {
    if (t.hour< 12){
        return true;
    }else{
        return false;
    }
}

bool after_15(const tickTime& t) {
    if (t.hour == 15 && t.minute !=0){
        return true;
    }else{
        return false;
    }
}

bool is_1130(const tickTime& t) {
    if (t.hour == 11 && t.minute == 30){
        return true;
    }else{
        return false;
    }
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
    bool had_one = (out.ticks_count > 0 && out.pm_closing_price > 0.0);
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
    metrics.valid_records_count++;
        

    long long current_vol = record.volume * 100; 
    double current_amount = record.price * current_vol;
    metrics.pm_closing_price = record.price;

    if (is_am_time(record.t)) {
        metrics.am_closing_price = record.price; 
        metrics.am_vol += current_vol;
        metrics.am_turnover += current_amount; 

        if (record.bs_type == "B"){
            metrics.am_inflow += current_amount;        
        }else if (record.bs_type == "S"){
            metrics.am_outflow += current_amount;    
        } 
    } else {
        metrics.pm_vol += current_vol;
        metrics.pm_turnover += current_amount; 
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

            update_stream_and_metrics(metrics, stream, record, pre_record);
            update_metrics_by_record(metrics, record);

            if (is_1130(record.t)){
                am_metrics = metrics;
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

    if (metrics.valid_records_count == 0) {
        return false;
    }

    out.ticks_count = metrics.valid_records_count;
    out.pm_closing_price = metrics.pm_closing_price;
    out.am_closing_price = metrics.am_closing_price;

    out.am_net_inflow_wan = (metrics.am_inflow - metrics.am_outflow) / WAN;
    out.pm_net_inflow_wan = (metrics.pm_inflow - metrics.pm_outflow) / WAN; 
    out.net_inflow_wan = out.am_net_inflow_wan + out.pm_net_inflow_wan;  
    
    out.total_turnover_wan = (metrics.pm_turnover+ metrics.am_turnover)/ WAN;
    out.am_turnover_wan = metrics.am_turnover / WAN; 
    
    if ((metrics.pm_turnover+ metrics.am_turnover) > 0.0) {
        out.am_turnover_ratio = (metrics.am_turnover / (metrics.pm_turnover+ metrics.am_turnover)) * BAI;
    } else {
        out.am_turnover_ratio = 0.0;
    }

    out.total_vol_wan = (metrics.am_vol + metrics.pm_vol) / WAN;  
    out.am_vol_wan = metrics.am_vol / WAN;

    if (out.total_vol_wan > 0.0) {
        out.avg_price = out.total_turnover_wan / out.total_vol_wan;
    }

    if (out.total_turnover_wan > 0.0) {
        out.inflow_ratio = (out.net_inflow_wan / out.total_turnover_wan) * BAI;
    }

    out.avg_vol_per_tick = (out.total_vol_wan * 10000.0) / metrics.valid_records_count; 

    
    std::string pure_name = fs::path(filename).filename().string();
    out.date_str = (pure_name.length() >= 10) ? pure_name.substr(0, 10) : pure_name;

    out.head_data = metrics.head_data;
    out.stream_sum_info = metrics.stream_sum_info;
    out.first_record = metrics.first_record;
    out.last_record = metrics.last_record;

    return true;
}

void process_out(DayOutputMetrics& out, DayOutputMetrics& prev_out){

        out.historical_total_inflow = prev_out.historical_total_inflow + out.net_inflow_wan;
        if (is_filled_tick(prev_out)) {
            out.pct_change =pct(out.pm_closing_price, prev_out.pm_closing_price);
            out.am_pct_change = pct(out.am_closing_price, prev_out.pm_closing_price);
            out.start_change = pct(out.first_record.price, prev_out.pm_closing_price);
            out.pre_closing_price = prev_out.pm_closing_price;
        }

        if (std::abs(out.pct_change) <= 1.0) {
            out.net_per_change = 0.0;
        } else {
            out.net_per_change = out.inflow_ratio / out.pct_change;
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

    DayOutputMetrics out;
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

        
        if (!process_single_file(file, out)) {
            continue;
        }

        process_out(out, prev_out);

        deal_classfy(out);
        divergengce = get_and_print_signals(out, prev_out);

        print_bodys(opts, out, prev_out, divergengce);

        if (out.ticks_count > 0) {
            prev_out = out;
        }
    }

    print_headers(opts);

    return 0;
}