#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>
#include <filesystem> // C++17 目录遍历库
#include <algorithm>  // 用于排序
#include <cmath>       

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

// ==================== 封装的表头打印函数 ====================
void print_header() {
    std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << std::left  << std::setw(23) << "File_Name" << " | "
              << std::right << std::setw(6)  << "Ticks" << " | "
              << std::setw(10) << "Vol(10k)" << " | "               
              << std::setw(14) << "Vol/Ticks(10K)" << " | "         
              << std::setw(13) << "Turnover(10k)" << " | "
              << std::setw(8)  << "Close" << " | "           
              << std::setw(9)  << "Change%" << " | "         
              << std::setw(12) << "Net_In(10k)" << " | "     
              << std::setw(15) << "Net_In/Turnover" << " | " 
              << std::setw(14) << "Hist_Cum(10k)" << " | "   
              << std::left  << std::setw(12) << "Signal"
              << std::endl;
    std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
}

// ==================== 封装的数据行格式化输出函数 ====================
void print_data_row(const std::string& pure_name, long long valid_records_count, 
                    double total_vol_wan, double avg_vol_per_tick, double total_turnover_wan, 
                    double closing_price, const std::string& pct_str, double net_inflow_wan, 
                    double inflow_ratio, double historical_total_inflow, const std::string& divergence_str,
                    const std::string& row_color_start, const std::string& row_color_end) {
    
    std::stringstream inflow_ss, ratio_ss, hist_ss, close_ss;
    
    close_ss << std::fixed << std::setprecision(2) << closing_price;
    std::string close_str = close_ss.str();

    inflow_ss << std::fixed << std::setprecision(2) << (net_inflow_wan >= 0 ? "+" : "") << net_inflow_wan;
    std::string inflow_str = inflow_ss.str();

    ratio_ss << std::fixed << std::setprecision(2) << (inflow_ratio >= 0 ? "+" : "") << inflow_ratio << "%";
    std::string ratio_str = ratio_ss.str();

    hist_ss << std::fixed << std::setprecision(2) << (historical_total_inflow >= 0 ? "+" : "") << historical_total_inflow;
    std::string hist_str = hist_ss.str();

    // 1. 基础成交量价数据（不着色）
    std::cout << std::left << std::setw(23) << pure_name << " | "
              << std::right << std::setw(6) << valid_records_count << " | "
              << std::setw(10) << std::fixed << std::setprecision(2) << total_vol_wan << " | " 
              << std::setw(14) << std::fixed << std::setprecision(1) << avg_vol_per_tick << " | " 
              << std::setw(13) << total_turnover_wan << " | ";
              
    // 2. 核心联动区块着色开始：包含 Close | Change% | Net_In | Net_In/Turnover | Hist_Cum
    std::cout << row_color_start 
              << std::setw(8)  << close_str  << " | "
              << std::setw(9)  << pct_str    << " | "
              << std::setw(12) << inflow_str << " | "
              << std::setw(15) << ratio_str  << " | "
              << std::setw(14) << hist_str   << row_color_end << " | ";
    
    // 3. 终极判定信号（维持自身的高亮逻辑，不受行涨跌颜色干扰）
    std::cout << std::left << divergence_str << std::endl;
}

// ==================== 核心封装：指标衍生、信号计算与输出入口 ====================
void get_and_print_signals(const std::string& pure_name, long long valid_records_count, 
                           double total_vol_wan, double avg_vol_per_tick, double total_turnover_wan, 
                           double closing_price, double prev_closing_price, double net_inflow_wan, 
                           double inflow_ratio, double historical_total_inflow) {
    
    bool has_prev = (prev_closing_price > 0.0);
    double pct_change = 0.0;
    std::string pct_str = "0.00%";
    
    // 行颜色控制变量
    std::string row_color_start = "";
    std::string row_color_end = "";

    if (has_prev) {
        pct_change = ((closing_price - prev_closing_price) / prev_closing_price) * 100.0;
        
        std::stringstream pct_ss;
        pct_ss << std::fixed << std::setprecision(2) << (pct_change >= 0 ? "+" : "") << pct_change << "%";
        pct_str = pct_ss.str();

        // 核心改动：根据当天涨跌，为整段区间赋予全局颜色
        if (closing_price > prev_closing_price) {
            row_color_start = "\033[31m"; // 涨 -> 红色
            row_color_end = "\033[0m";
        } else if (closing_price < prev_closing_price) {
            row_color_start = "\033[32m"; // 跌 -> 绿色
            row_color_end = "\033[0m";
        }
    }

    // 自动判定背离信号标签
    std::string divergence_str = "      -      "; 
    if (has_prev) {
        if (pct_change > 0 && net_inflow_wan < 0) {
            divergence_str = "\033[33m[ 涨/净流出 ]\033[0m"; 
        } else if (pct_change < 0 && net_inflow_wan > 0) {
            divergence_str = "\033[35m[ 跌/净流入 ]\033[0m"; 
        }
    }

    // 送入底层对齐输出
    print_data_row(pure_name, valid_records_count, total_vol_wan, avg_vol_per_tick, 
                   total_turnover_wan, closing_price, pct_str, net_inflow_wan, inflow_ratio, 
                   historical_total_inflow, divergence_str, row_color_start, row_color_end);
}

// ==================== 封装的文件核心原始数据解析函数 ====================
void parse_tick_file(std::ifstream& infile, long long& valid_records_count, double& closing_price,
                     long long& total_vol, double& total_turnover, 
                     double& total_inflow, double& total_outflow) {
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
            valid_records_count++;
            closing_price = price; 

            long long current_vol = static_cast<long long>(volume) * 100; 
            total_vol += current_vol;

            double current_amount = price * current_vol;
            total_turnover += current_amount; 

            if (bs_type == "B") total_inflow += current_amount;  
            else if (bs_type == "S") total_outflow += current_amount; 
        }
    }
    infile.close();
}

// ==================== 封装的单文件流程控制函数 ====================
double process_single_file(const std::string& filename, double prev_closing_price, double& historical_total_inflow) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "错误：无法打开文件 " << filename << std::endl;
        return 0.0;
    }

    double total_inflow = 0.0, total_outflow = 0.0, total_turnover = 0.0; 
    long long total_vol = 0, valid_records_count = 0;
    double closing_price = 0.0; 

    parse_tick_file(infile, valid_records_count, closing_price, total_vol, 
                    total_turnover, total_inflow, total_outflow);

    if (valid_records_count == 0) return 0.0;

    double net_inflow_wan = (total_inflow - total_outflow) / 10000.0;
    double total_turnover_wan = total_turnover / 10000.0;
    double total_vol_wan = total_vol / 10000.0;  

    double inflow_ratio = 0.0;
    if (total_turnover_wan > 0.0) {
        inflow_ratio = (net_inflow_wan / total_turnover_wan) * 100.0;
    }

    historical_total_inflow += net_inflow_wan;

    double avg_vol_per_tick = (total_vol_wan * 10000.0) / valid_records_count; 
    std::string pure_name = fs::path(filename).filename().string();

    get_and_print_signals(pure_name, valid_records_count, total_vol_wan, avg_vol_per_tick, 
                          total_turnover_wan, closing_price, prev_closing_price, net_inflow_wan, 
                          inflow_ratio, historical_total_inflow);

    return closing_price; 
}

// ==================== 主入口函数 ====================
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "用法提示: " << argv[0] << " <目录路径>" << std::endl;
        return 1;
    }

    std::string dir_path = argv[1];
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "错误：路径不存在或无效: " << dir_path << std::endl;
        return 1;
    }

    std::cout << "======================= 目录批处理资金统计 =======================" << std::endl;
    std::cout << "正在扫描目录: " << dir_path << std::endl;

    std::vector<std::string> files_to_process;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files_to_process.push_back(entry.path().string());
        }
    }
    std::sort(files_to_process.begin(), files_to_process.end());

    if (files_to_process.empty()) {
        std::cout << "提示：未找到 .txt 格式数据文件。" << std::endl;
        return 0;
    }

    print_header();

    double prev_closing_price = 0.0;
    double historical_total_inflow = 0.0; 
    
    for (const auto& file : files_to_process) {
        prev_closing_price = process_single_file(file, prev_closing_price, historical_total_inflow);
    }

    print_header();
    std::cout << "=======================================================================================================================================================" << std::endl;

    return 0;
}