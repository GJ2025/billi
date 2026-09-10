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

void print_slim_price(const DayOutputMetrics& out,const DayOutputMetrics& prev_out, RecordScale t, const std::vector<Col>& cols);

void print_slim_price_ratio(const DayOutputMetrics& out,const DayOutputMetrics& prev_out, RecordScale t, const std::vector<Col>& cols);



void print_quiet_buying_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out) ;

void print_will(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols);

void print_price(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const DailyMetrics& metrics, const std::vector<Col>& cols);

void print_tseq_price(DailyMetrics& metrics);

void print_signal(const std::string& file, const VectorStats& v_stats, SubCondition sc);

void print_all_data(const DayOutputMetrics& out, const DayOutputMetrics& prev_out, const std::string& divergence_str);

void print_header_info(const DayOutputMetrics& out, const DayOutputMetrics& pre_out);

void print_all() ;

void print_headers(const ProgramOptions& opts);
void print_bodys(const ProgramOptions& opts, const DayOutputMetrics& out, const DayOutputMetrics& prev_out, std::string divergence) ;
void print_tseq_sz(const std::string& date, DailyMetrics& metrics, std::vector<DailyMetrics>& all_metrics);

#endif // SHOW_H