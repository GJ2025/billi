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

namespace fs = std::filesystem;

struct TickData {
    std::string time;       
    double price;           
    int volume;             
    int type_count;         
    std::string bs_type;    
};

bool is_loading_data(const std::string& str) {
    if (str.empty()) return false;
    return std::isdigit(static_cast<unsigned char>(str[0]));
}

bool is_am_time(const std::string& tick_time) {
    std::string t = tick_time;
    t.erase(std::remove(t.begin(), t.end(), ':'), t.end());
    if (t.length() >= 4) {
        return t.substr(0, 4) <= "1130";
    }
    return false;
}

void print_header() {
    std::cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl; 
    std::cout << std::left  << std::setw(11) << "Date" << " | "
              << std::right << std::setw(5)  << "Ticks" << " | "
              << std::setw(9)  << "Vol" << " | "               
              << std::setw(9)  << "AM_Vol" << " | "
              << std::setw(10) << "Vol/Ticks" << " | "         
              << std::setw(11) << "Turnover" << " | "
              << std::setw(8)  << "AM_Close" << " | "  
              << std::setw(9)  << "Avg_Price" << " | " 
              << std::setw(7)  << "Close" << " | "            
              << std::setw(8)  << "Change%" << " | "         
              << std::setw(10) << "Net_In" << " | "     
              << std::setw(10) << "AM_Net_In" << " | "
              << std::setw(10) << "PM_Net_In" << " | " // 显式输出下午净流入
              << std::setw(11) << "Net_In/Turn" << " | " 
              << std::setw(11) << "Hist_Cum" << " | "   
              << std::left  << std::setw(20) << "Signal" 
              << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    std::cout << "----------------" << std::endl;
}

void print_data_row(const std::string& date_str, long long valid_records_count, 
                    double total_vol_wan, double am_vol_wan, double avg_vol_per_tick, double total_turnover_wan, 
                    double am_closing_price, double avg_price, double closing_price, const std::string& pct_str, 
                    double net_inflow_wan, double am_net_inflow_wan, double pm_net_inflow_wan,
                    double inflow_ratio, double historical_total_inflow, const std::string& divergence_str,
                    const std::string& row_color_start, const std::string& row_color_end) {
    
    std::stringstream inflow_ss, am_inflow_ss, pm_inflow_ss, ratio_ss, hist_ss, close_ss, am_close_ss, avg_price_ss;
    
    close_ss << std::fixed << std::setprecision(2) << closing_price;
    std::string close_str = close_ss.str();

    if (am_closing_price > 0.0) {
        am_close_ss << std::fixed << std::setprecision(2) << am_closing_price;
    } else {
        am_close_ss << " - ";
    }
    std::string am_close_str = am_close_ss.str();

    if (avg_price > 0.0) {
        avg_price_ss << std::fixed << std::setprecision(2) << avg_price;
    } else {
        avg_price_ss << " - ";
    }
    std::string avg_price_str = avg_price_ss.str();

    inflow_ss << std::fixed << std::setprecision(2) << (net_inflow_wan >= 0 ? "+" : "") << net_inflow_wan;
    std::string inflow_str = inflow_ss.str();

    am_inflow_ss << std::fixed << std::setprecision(2) << (am_net_inflow_wan >= 0 ? "+" : "") << am_net_inflow_wan;
    std::string am_inflow_str = am_inflow_ss.str();

    pm_inflow_ss << std::fixed << std::setprecision(2) << (pm_net_inflow_wan >= 0 ? "+" : "") << pm_net_inflow_wan;
    std::string pm_inflow_str = pm_inflow_ss.str();

    ratio_ss << std::fixed << std::setprecision(2) << (inflow_ratio >= 0 ? "+" : "") << inflow_ratio << "%";
    std::string ratio_str = ratio_ss.str();

    hist_ss << std::fixed << std::setprecision(2) << (historical_total_inflow >= 0 ? "+" : "") << historical_total_inflow;
    std::string hist_str = hist_ss.str();

    std::cout << std::left << std::setw(11) << date_str << " | "
              << std::right << std::setw(5) << valid_records_count << " | "
              << std::fixed << std::setprecision(2)
              << std::setw(9)  << total_vol_wan << " | " 
              << std::setw(9)  << am_vol_wan << " | "
              << std::setw(10) << std::fixed << std::setprecision(1) << avg_vol_per_tick << " | " 
              << std::fixed << std::setprecision(2)
              << std::setw(11) << total_turnover_wan << " | "
              << std::setw(8)  << am_close_str << " | "
              << std::setw(9)  << avg_price_str << " | "; 
              
    std::cout << row_color_start 
              << std::setw(7)  << close_str  << " | "
              << std::setw(8)  << pct_str    << " | "
              << std::setw(10) << inflow_str << " | "
              << std::setw(10) << am_inflow_str << " | "
              << std::setw(10) << pm_inflow_str << " | " 
              << std::setw(11) << ratio_str  << " | "
              << std::setw(11) << hist_str   << row_color_end << " | ";
    
    std::cout << std::left << std::setw(20) << divergence_str << std::endl;
}

void get_and_print_signals(const std::string& date_str, long long valid_records_count, 
                           double total_vol_wan, double am_vol_wan, double avg_vol_per_tick, double total_turnover_wan, 
                           double am_closing_price, double avg_price, double prev_avg_price, double closing_price, double prev_closing_price, 
                           double net_inflow_wan, double am_net_inflow_wan, double pm_net_inflow_wan, double inflow_ratio, double historical_total_inflow) {
    
    bool has_prev = (prev_closing_price > 0.0);
    double pct_change = 0.0;
    std::string pct_str = "0.00%";
    
    std::string row_color_start = "";
    std::string row_color_end = "";

    if (has_prev) {
        pct_change = ((closing_price - prev_closing_price) / prev_closing_price) * 100.0;
        
        std::stringstream pct_ss;
        pct_ss << std::fixed << std::setprecision(2) << (pct_change >= 0 ? "+" : "") << pct_change << "%";
        pct_str = pct_ss.str();

        if (closing_price > prev_closing_price) {
            row_color_start = "\033[31m"; 
            row_color_end = "\033[0m";
        } else if (closing_price < prev_closing_price) {
            row_color_start = "\033[32m"; 
            row_color_end = "\033[0m";
        }
    }

    // 组合信号判定
    std::string divergence_str = ""; 
    if (has_prev) {
        if (pct_change > 0 && net_inflow_wan < 0) {
            divergence_str += "\033[33m[UP/NET_OUT]\033[0m"; 
        } else if (pct_change < 0 && net_inflow_wan > 0) {
            divergence_str += "\033[35m[DN/NET_IN ]\033[0m"; 
        }

        if (prev_avg_price > 0.0) {
            if (avg_price > prev_avg_price && net_inflow_wan < 0) {
                if (!divergence_str.empty()) divergence_str += " ";
                divergence_str += "\033[1;31m[AV_UP/OUT]\033[0m"; 
            } else if (avg_price < prev_avg_price && net_inflow_wan > 0) {
                if (!divergence_str.empty()) divergence_str += " ";
                divergence_str += "\033[1;32m[AV_DN/IN ]\033[0m"; 
            }
        }
    }

    if (divergence_str.empty()) {
        divergence_str = "      -      ";
    }

    print_data_row(date_str, valid_records_count, total_vol_wan, am_vol_wan, avg_vol_per_tick, 
                   total_turnover_wan, am_closing_price, avg_price, closing_price, pct_str, 
                   net_inflow_wan, am_net_inflow_wan, pm_net_inflow_wan,
                   inflow_ratio, historical_total_inflow, divergence_str, row_color_start, row_color_end);
}

// ==================== 细化 parse_tick_file 里的时段流向 ====================
void parse_tick_file(std::ifstream& infile, long long& valid_records_count, double& closing_price, double& am_closing_price,
                     long long& am_vol, long long& pm_vol, double& total_turnover, 
                     double& am_inflow, double& am_outflow, double& pm_inflow, double& pm_outflow) {
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string first_token;
        ss >> first_token;

        if (!is_loading_data(first_token)) continue;

        double price;
        int volume, type_count;
        std::string bs_type;

        if (ss >> price >> volume >> type_count) {
            if (!(ss >> bs_type)) bs_type = "-"; 

            // ==================== 🛠️ BUG 修复核心位置 ====================
            // 拦截集合竞价虚拟快照：如果笔数(type_count)为 0，说明是 09:15-09:24 的非真实交易流
            // 必须直接跳过，否则会让全天成交额无端膨胀几倍甚至十几倍
            if (type_count == 0) continue; 
            // ==========================================================

            valid_records_count++;
            closing_price = price; // 最后一个有效价格即为 Close

            long long current_vol = static_cast<long long>(volume) * 100; // 现手 * 100 换算为真实股数
            double current_amount = price * current_vol;
            total_turnover += current_amount; 

            // 根据时间严格做 if-else 分流
            if (is_am_time(first_token)) {
                am_closing_price = price; 
                am_vol += current_vol;
                if (bs_type == "B") am_inflow += current_amount;
                else if (bs_type == "S") am_outflow += current_amount;
            } else {
                // 下午时段
                pm_vol += current_vol;
                if (bs_type == "B") pm_inflow += current_amount;
                else if (bs_type == "S") pm_outflow += current_amount;
            }
        }
    }
    infile.close();
}

void process_single_file(const std::string& filename, double prev_closing_price, double prev_avg_price,
                         double& current_closing_price, double& current_avg_price, double& historical_total_inflow) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        current_closing_price = 0.0;
        current_avg_price = 0.0;
        return;
    }

    double total_turnover = 0.0;
    double am_inflow = 0.0, am_outflow = 0.0;
    double pm_inflow = 0.0, pm_outflow = 0.0;
    long long am_vol = 0, pm_vol = 0, valid_records_count = 0;
    double closing_price = 0.0; 
    double am_closing_price = 0.0; 

    // 调用解析函数
    parse_tick_file(infile, valid_records_count, closing_price, am_closing_price, am_vol, pm_vol,
                    total_turnover, am_inflow, am_outflow, pm_inflow, pm_outflow);

    if (valid_records_count == 0) {
        current_closing_price = 0.0;
        current_avg_price = 0.0;
        return;
    }

    // 各项指标精准求和与换算
    double am_net_inflow_wan = (am_inflow - am_outflow) / 10000.0;
    double pm_net_inflow_wan = (pm_inflow - pm_outflow) / 10000.0; // 纯粹从下午 Tick 块中计算得到
    double net_inflow_wan = am_net_inflow_wan + pm_net_inflow_wan;  // 总流入由上下午无缝捏合
    
    double total_turnover_wan = total_turnover / 10000.0;
    long long total_vol = am_vol + pm_vol;
    double total_vol_wan = total_vol / 10000.0;  
    double am_vol_wan = am_vol / 10000.0;

    double avg_price = 0.0;
    if (total_vol_wan > 0.0) {
        avg_price = total_turnover_wan / total_vol_wan;
    }

    double inflow_ratio = 0.0;
    if (total_turnover_wan > 0.0) {
        inflow_ratio = (net_inflow_wan / total_turnover_wan) * 100.0;
    }

    historical_total_inflow += net_inflow_wan;
    double avg_vol_per_tick = (total_vol_wan * 10000.0) / valid_records_count; 
    
    std::string pure_name = fs::path(filename).filename().string();
    std::string date_str = (pure_name.length() >= 10) ? pure_name.substr(0, 10) : pure_name;

    get_and_print_signals(date_str, valid_records_count, total_vol_wan, am_vol_wan, avg_vol_per_tick, 
                          total_turnover_wan, am_closing_price, avg_price, prev_avg_price, closing_price, prev_closing_price, 
                          net_inflow_wan, am_net_inflow_wan, pm_net_inflow_wan, inflow_ratio, historical_total_inflow);

    current_closing_price = closing_price;
    current_avg_price = avg_price;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Directory_Path>" << std::endl;
        return 1;
    }

    std::string dir_path = argv[1];

    std::cout << "======================= Pre-processing =======================" << std::endl;
    
    const char* c_env = std::getenv("c");
    std::string script_dir = c_env ? c_env : "."; 
    std::string script_path = (fs::path(script_dir) / "c.sh").string();

    if (!fs::exists(script_path)) {
        std::cerr << "Warning: Pre-processing script not found at: " << script_path  
                  << "\nSkipping pre-processing stage..." << std::endl;
    } else {
        std::string shell_cmd = "sh " + script_path + " " + dir_path;
        std::cout << "Executing: " << shell_cmd << std::endl;
        
        int ret = std::system(shell_cmd.c_str());
        if (ret != 0) {
            std::cerr << "Error: Pre-processing script exited with code " << ret << std::endl;
        } else {
            std::cout << "Pre-processing completed successfully.\n" << std::endl;
        }
    }

    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path: " << dir_path << std::endl;
        return 1;
    }

    std::vector<std::string> files_to_process;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files_to_process.push_back(entry.path().string());
        }
    }
    std::sort(files_to_process.begin(), files_to_process.end());

    if (files_to_process.empty()) {
        std::cout << "Info: No valid .txt data files found." << std::endl;
        return 0;
    }

    print_header();

    double prev_closing_price = 0.0;
    double prev_avg_price = 0.0; 
    double historical_total_inflow = 0.0; 
    
    for (const auto& file : files_to_process) {
        double current_closing_price = 0.0;
        double current_avg_price = 0.0;
        
        process_single_file(file, prev_closing_price, prev_avg_price, current_closing_price, current_avg_price, historical_total_inflow);
        
        if (current_closing_price > 0.0) {
            prev_closing_price = current_closing_price;
            prev_avg_price = current_avg_price;
        }
    }

    print_header();
    return 0;
}