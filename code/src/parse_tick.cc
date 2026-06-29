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

// 原始定义的整个文件统计/TickData结构体（保留作扩展或外部兼容）
struct TickData {
    std::string time;       
    double price;           
    int volume;             
    int type_count;         
    std::string bs_type;    
};

// ================== 单行 Tick 记录结构体 ==================
struct TickRecord {
    std::string time = "";
    double price = 0.0;
    int volume = 0;
    int type_count = 0;
    std::string bs_type = "-";

    // 重载流输入运算符，一键解析整行（包括时间戳）
    friend std::istream& operator>>(std::istream& is, TickRecord& record) {
        if (is >> record.time >> record.price >> record.volume >> record.type_count) {
            // 如果行尾没有 B/S 属性，则默认为 "-"
            if (!(is >> record.bs_type)) {
                record.bs_type = "-";
            }
        }
        return is;
    }
};

// ================== 单日解析基础统计指标的结构体 ==================
struct DailyMetrics {
    long long valid_records_count = 0;
    double closing_price = 0.0;
    double am_closing_price = 0.0;
    long long am_vol = 0;
    long long pm_vol = 0;
    double total_turnover = 0.0;
    double am_inflow = 0.0;
    double am_outflow = 0.0;
    double pm_inflow = 0.0;
    double pm_outflow = 0.0;
};

// ================== 单日衍生/计算输出指标的结构体 ==================
struct DayOutputMetrics {
    double am_net_inflow_wan = 0.0;
    double pm_net_inflow_wan = 0.0;
    double net_inflow_wan = 0.0;
    double total_turnover_wan = 0.0;
    double total_vol_wan = 0.0;
    double am_vol_wan = 0.0;
    double avg_price = 0.0;
    double inflow_ratio = 0.0;
    double avg_vol_per_tick = 0.0;
};
// ====================================================================

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

void run_preprocessing(const std::string& dir_path) {
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
              << std::setw(10) << "PM_Net_In" << " | " 
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

// ================== 重构：直接传递结构体引用，精简参数列表 ==================
void get_and_print_signals(const std::string& date_str, 
                           const DailyMetrics& metrics, 
                           const DayOutputMetrics& out,
                           double prev_closing_price, 
                           double prev_avg_price, 
                           double historical_total_inflow) {
    
    bool has_prev = (prev_closing_price > 0.0);
    double pct_change = 0.0;
    std::string pct_str = "0.00%";
    
    std::string row_color_start = "";
    std::string row_color_end = "";

    if (has_prev) {
        pct_change = ((metrics.closing_price - prev_closing_price) / prev_closing_price) * 100.0;
        
        std::stringstream pct_ss;
        pct_ss << std::fixed << std::setprecision(2) << (pct_change >= 0 ? "+" : "") << pct_change << "%";
        pct_str = pct_ss.str();

        if (metrics.closing_price > prev_closing_price) {
            row_color_start = "\033[31m"; 
            row_color_end = "\033[0m";
        } else if (metrics.closing_price < prev_closing_price) {
            row_color_start = "\033[32m"; 
            row_color_end = "\033[0m";
        }
    }

    // 组合信号判定
    std::string divergence_str = ""; 
    if (has_prev) {
        if (pct_change > 0 && out.net_inflow_wan < 0) {
            divergence_str += "\033[33m[UP/NET_OUT]\033[0m"; 
        } else if (pct_change < 0 && out.net_inflow_wan > 0) {
            divergence_str += "\033[35m[DN/NET_IN ]\033[0m"; 
        }

        if (prev_avg_price > 0.0) {
            if (out.avg_price > prev_avg_price && out.net_inflow_wan < 0) {
                if (!divergence_str.empty()) divergence_str += " ";
                divergence_str += "\033[1;31m[AV_UP/OUT]\033[0m"; 
            } else if (out.avg_price < prev_avg_price && out.net_inflow_wan > 0) {
                if (!divergence_str.empty()) divergence_str += " ";
                divergence_str += "\033[1;32m[AV_DN/IN ]\033[0m"; 
            }
        }
    }

    if (divergence_str.empty()) {
        divergence_str = "      -      ";
    }

    print_data_row(date_str, metrics.valid_records_count, out.total_vol_wan, out.am_vol_wan, out.avg_vol_per_tick, 
                   out.total_turnover_wan, metrics.am_closing_price, out.avg_price, metrics.closing_price, pct_str, 
                   out.net_inflow_wan, out.am_net_inflow_wan, out.pm_net_inflow_wan,
                   out.inflow_ratio, historical_total_inflow, divergence_str, row_color_start, row_color_end);
}

void parse_tick_file(std::ifstream& infile, DailyMetrics& metrics) {
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        TickRecord record;
        
        if (ss >> record) {
            if (!is_loading_data(record.time)) continue;
            if (record.type_count == 0) continue; 

            metrics.valid_records_count++;
            metrics.closing_price = record.price; 

            long long current_vol = static_cast<long long>(record.volume) * 100; 
            double current_amount = record.price * current_vol;
            metrics.total_turnover += current_amount; 

            if (is_am_time(record.time)) {
                metrics.am_closing_price = record.price; 
                metrics.am_vol += current_vol;
                if (record.bs_type == "B") metrics.am_inflow += current_amount;
                else if (record.bs_type == "S") metrics.am_outflow += current_amount;
            } else {
                metrics.pm_vol += current_vol;
                if (record.bs_type == "B") metrics.pm_inflow += current_amount;
                else if (record.bs_type == "S") metrics.pm_outflow += current_amount;
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

    DailyMetrics metrics; 
    parse_tick_file(infile, metrics);

    if (metrics.valid_records_count == 0) {
        current_closing_price = 0.0;
        current_avg_price = 0.0;
        return;
    }

    DayOutputMetrics out;
    out.am_net_inflow_wan = (metrics.am_inflow - metrics.am_outflow) / 10000.0;
    out.pm_net_inflow_wan = (metrics.pm_inflow - metrics.pm_outflow) / 10000.0; 
    out.net_inflow_wan = out.am_net_inflow_wan + out.pm_net_inflow_wan;  
    
    out.total_turnover_wan = metrics.total_turnover / 10000.0;
    long long total_vol = metrics.am_vol + metrics.pm_vol;
    out.total_vol_wan = total_vol / 10000.0;  
    out.am_vol_wan = metrics.am_vol / 10000.0;

    if (out.total_vol_wan > 0.0) {
        out.avg_price = out.total_turnover_wan / out.total_vol_wan;
    }

    if (out.total_turnover_wan > 0.0) {
        out.inflow_ratio = (out.net_inflow_wan / out.total_turnover_wan) * 100.0;
    }

    historical_total_inflow += out.net_inflow_wan;
    out.avg_vol_per_tick = (out.total_vol_wan * 10000.0) / metrics.valid_records_count; 
    
    std::string pure_name = fs::path(filename).filename().string();
    std::string date_str = (pure_name.length() >= 10) ? pure_name.substr(0, 10) : pure_name;

    // ================== 完美的流线型调用 ==================
    get_and_print_signals(date_str, metrics, out, prev_closing_price, prev_avg_price, historical_total_inflow);

    current_closing_price = metrics.closing_price;
    current_avg_price = out.avg_price;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Directory_Path>" << std::endl;
        return 1;
    }

    std::string dir_path = argv[1];
    run_preprocessing(dir_path);

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
    
    std::string target_company_id = extract_company_id(files_to_process[0]);

    for (const auto& file : files_to_process) {
        std::string current_company_id = extract_company_id(file);
        if (current_company_id != target_company_id) {
            std::cout << "\033[33m[Skip File]\033[0m Mismatched Company (" 
                      << current_company_id << " != " << target_company_id 
                      << "): " << fs::path(file).filename().string() << std::endl;
            continue;
        }

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