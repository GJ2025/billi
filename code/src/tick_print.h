#ifndef PRINT_TICK_H
#define PRINT_TICK_H
#include "tick_types.h"
#include "collect_stream.h"

void print_table_header(); 
void print_header_info(const DayOutputMetrics& out, const DayOutputMetrics& pre_out);
void print_all();
void show_margin_header(const std::string& margin);
void print_data_row(const DayOutputMetrics& out,  const std::string& divergence_str);
void print_headers(const ProgramOptions& opts);
void print_bodys(const ProgramOptions& opts, DayOutputMetrics& out, const DayOutputMetrics& prev_out, std::string divergence) ;


#endif // PRINT_TICK_H
