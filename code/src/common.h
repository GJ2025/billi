#ifndef COMMON_H
#define COMMON_H

#define WAN 10000
#define BAI 100

struct tickTime {
    int hour = 0;
    int minute = 0;
};


inline double pct(double target, double base){
    return ((target - base) / base) * 100.0;
}


inline bool is_am_time(const tickTime& t) {
    if (t.hour < 12){
        return true;
    }else{
        return false;
    }
}

inline bool after_15(const tickTime& t) {
    if (t.hour == 15 && t.minute !=0){
        return true;
    }else{
        return false;
    }
}

inline bool is_am_end(const tickTime& t) {
    if (t.hour == 11 && (t.minute == 30 || t.minute == 29)){
        return true;
    }else{
        return false;
    }
}


#endif // COMMON_H
