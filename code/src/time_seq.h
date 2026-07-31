#ifndef TIME_SEQ_H
#define TIME_SEQ_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include "common.h"

struct SeqTime{
    int start_hour = 0;
    int start_min = 0;
    int cnt = 0;
    int intervel =0;
};

tickTime get_current_tick_time();
std::vector<tickTime> generate_today_tick_times(const tickTime& current, size_t cnt = 0, int interval_minutes = 30);

inline std::vector<tickTime> min_vector(SeqTime tseq){

    tickTime current;

    if (tseq.start_hour == 0 && tseq.start_min ==0 ){
        current = get_current_tick_time();
    }else{
        current.hour = tseq.start_hour;
        current.minute = tseq.start_min;
    }


    return generate_today_tick_times(current, tseq.cnt, tseq.intervel);
}



int show_time(SeqTime tseq) ;

int show_time_vector(std::vector<tickTime>& tick_times);
#endif // TIME_SEQ_H
