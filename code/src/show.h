#ifndef SHOW_H
#define SHOW_H
#include "opts.h"
#include "collect_stream.h"

template<typename T>
void print_next(const T& val, int& index, const std::vector<Col>& cols);

template<typename T>
void print_next_pos(const T& val, int& index, const std::vector<Col>& cols);

void print_decorative_line(int total_width, const std::string& left_title, const std::string& right_title) ;


void print__headers(const std::string& title, const std::vector<Col>& cols) ;

void print_slim_price(const std::string& date_str, const DailyMetrics& metrics, const DailyMetrics& pre_metrics, RecordScale t, const std::vector<Col>& cols) ;

void print_slim_price_ratio(const std::string& date_str, const DailyMetrics& metrics, const DailyMetrics& pre_metrics, RecordScale t, const std::vector<Col>& cols);



void print_quiet_buying_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) ;

void print_will(const std::string& date_str, const DailyMetrics& pre_metrics, const DailyMetrics& metrics, const std::vector<Col>& cols) ;

void print_price(const std::string& date_str, const DailyMetrics& pre_metrics, const DailyMetrics& metrics, const std::vector<Col>& cols);

void print_tseq_price(DailyMetrics& metrics);

void print_signal(const std::string& file, const VectorStats& v_stats, SubCondition sc);

void print_all_data(const std::string& date_str,  
                    const DailyMetrics& am_metrics, 
                    const DailyMetrics& metrics, 
                    const DailyMetrics& pre_metrics);

void print_header_info(const DayOutputMetrics& out, const DayOutputMetrics& pre_out);
std::string get_and_print_signals(const DayOutputMetrics& out) ;

void print_all() ;

void print_headers(const ProgramOptions& opts);

void print_bodys(const ProgramOptions& opts,  
                const std::string& date_str, 
                const DailyMetrics& am_metrics, 
                const DailyMetrics& metrics, 
                const DailyMetrics& pre_metrics);

void print_tseq_sz(const std::string& date, DailyMetrics& metrics, std::vector<DailyMetrics>& all_metrics);

#endif // SHOW_H