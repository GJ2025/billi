#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include "time_seq.h"

// struct tickTime {
//     int hour = 0;
//     int minute = 0;
// };

tickTime get_current_tick_time() {
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    
    struct tm local_tm;
#if defined(_MSC_VER)
    localtime_s(&local_tm, &tt);
#else
    localtime_r(&tt, &local_tm);
#endif

    tickTime current_time;
    current_time.hour = local_tm.tm_hour; 
    current_time.minute = local_tm.tm_min;
    
    return current_time;
}

int time_to_minutes(const tickTime& t) {
    return t.hour * 60 + t.minute;
}

bool should_filter(int t_min) {
    int m_0930 = 9 * 60 + 30;   // 570
    int m_1130 = 11 * 60 + 30;  // 690
    int m_1300 = 13 * 60;       // 780
    int m_1500 = 15 * 60;       // 900


    if (t_min < m_0930){
        return true;
    }

    if (t_min > m_1130 && t_min < m_1300) {
        return true;
    }

    if (t_min > m_1500){
        return true;
    }
    
    return false;
}

std::vector<tickTime> generate_today_tick_times(const tickTime& current, size_t cnt , int interval_minutes) {
    std::vector<tickTime> result;

    int total_minutes = time_to_minutes(current);

    while (total_minutes > 0) {
        total_minutes -= interval_minutes;

        if (should_filter(total_minutes)) {
            continue; 
        }

        tickTime t;
        t.hour = total_minutes / 60;
        t.minute = total_minutes % 60;
        
        result.push_back(t);

        if (result.size() == cnt){
            std::cout << "size:" << result.size() << std::endl;
            break;
        }
    }

    return result;
}

int show_time(SeqTime tseq) {

    std::vector<tickTime> tick_times = min_vector(tseq);

    std::cout << "今天过滤后的有效时间点 (共 " << tick_times.size() << " 个):\n";
    for (const auto& t : tick_times) {
        std::cout << std::setw(2) << std::setfill('0') << t.hour << ":" 
                  << std::setw(2) << std::setfill('0') << t.minute << "\n";
    }
              
    return 0;
}

int show_time_vector(std::vector<tickTime>& tick_times){
    for (const auto& t : tick_times) {
        std::cout << std::setw(2) << std::setfill('0') << t.hour << ":" 
                  << std::setw(2) << std::setfill('0') << t.minute << "\n";
    }
              
    return 0;
}

// int show_time(size_t cnt = 0, int interval = 30, int hour = 0 ,int min = 0) {

//     std::vector<tickTime> tick_times = min_vector(cnt, interval, hour, min);

//     std::cout << "今天过滤后的有效时间点 (共 " << tick_times.size() << " 个):\n";
//     for (const auto& t : tick_times) {
//         std::cout << std::setw(2) << std::setfill('0') << t.hour << ":" 
//                   << std::setw(2) << std::setfill('0') << t.minute << "\n";
//     }
              
//     return 0;
// }


// int main(int argc, char* argv[]) {
//     int interval = 30;
//     int cnt = 0;
//     int hour = 0;
//     int min = 0;

//     cnt = std::stoi(argv[1]);
//     interval = std::stoi(argv[2]);

//     if (argc == 5){
//         hour = std::stoi(argv[3]);
//         min = std::stoi(argv[4]);
//     }


//     std::vector<tickTime> tick_times = min_vector(cnt, interval, hour, min);

//     std::cout << "今天过滤后的有效时间点 (共 " << tick_times.size() << " 个):\n";
//     for (const auto& t : tick_times) {
//         std::cout << std::setw(2) << std::setfill('0') << t.hour << ":" 
//                   << std::setw(2) << std::setfill('0') << t.minute << "\n";
//     }
              
//     return 0;
// }