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


extern tickTime sz_t[] ;
extern size_t get_sz_t_size();

tickTime get_current_tick_time();
std::vector<tickTime> generate_today_tick_times(const tickTime& current, size_t cnt = 0, int interval_minutes = 30);
extern tickTime current;


inline std::vector<tickTime> min_vector(SeqTime tseq){

    

    if (tseq.start_hour == 0 && tseq.start_min ==0 ){
        current = get_current_tick_time();
    }else{
        current.hour = tseq.start_hour;
        current.minute = tseq.start_min;
    }


    return generate_today_tick_times(current, tseq.cnt, tseq.intervel);
}

inline std::vector<tickTime> sz_time_vector(){

    
    std::vector<tickTime> result;


    size_t i = 0;
    for (i = 0; i < get_sz_t_size(); i++){
        result.push_back(sz_t[i]);
    }

    return result;

}



int show_time(SeqTime tseq) ;

int show_time_vector(std::vector<tickTime>& tick_times);
#endif // TIME_SEQ_H
