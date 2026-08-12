#ifndef COMMON_H
#define COMMON_H

#include <iostream>

#define WAN 10000
#define BAI 100

struct tickTime {
    int hour = 0;
    int minute = 0;
};


inline double pct(double target, double base){
    return ((target - base) / base) * 100.0;
}

#include <cmath>

inline double pct_base(double target, double base) {
    if (base == 0.0) return 0.0; // 防止除以零
    double pct = (target / base) * 100.0;
    // 先乘以 100 四舍五入，再除以 100
    return std::round(pct * 100.0) / 100.0;
}


inline int check_time(const tickTime& self, const tickTime& that) {
    if (self.hour != that.hour) {
        return self.hour - that.hour; // 小时不同直接比小时
    }
    return self.minute - that.minute; // 小时相同时比分钟
}

inline bool is_am_time(const tickTime& t) {

    tickTime that_time{11, 30};

    if (check_time(t, that_time) <= 0){
        return true;
    }

    return false;

}

inline bool after_15(const tickTime& t) {

    tickTime that_time{15, 00};

    if (check_time(t, that_time) > 0){
        return true;
    }else{
        return false;
    }
}

inline bool is_am_end(const tickTime& now,const tickTime& pre) {

    tickTime that_time{11, 30};

    if (check_time(now, that_time) > 0 &&  check_time(pre, that_time) <= 0){
        return true;
    }else{
        return false;
    }
}

inline bool is_tick_time_end(const tickTime& now,const tickTime& pre, tickTime this_time) {


    if (check_time(now, this_time) > 0 &&  check_time(pre, this_time) <= 0){
        return true;
    }else{
        return false;
    }
}

inline std::string format_tick_times(const tickTime& t) {
    std::ostringstream oss;
    
    oss << std::setw(2) << std::setfill('0') << t.hour << ":" 
        << std::setw(2) << std::setfill('0') << t.minute ;
    
    return oss.str();
}




#endif // COMMON_H
