#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <vector>
#include <string>

struct tickTime {
    int hour = 0;
    int minute = 0;
};

// 获取当前时间
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

// 辅助函数：将 tickTime 转换为当天总分钟数，便于比较大小
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

std::vector<tickTime> generate_today_tick_times(const tickTime& current, int cnt = 0, int interval_minutes = 30) {
    std::vector<tickTime> result;

    int total_minutes = time_to_minutes(current);
    int m_0930 = 9 * 60 + 30;   // 570
    int m_1500 = 15 * 60;       // 900


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

std::vector<tickTime> min_vector(int cnt = 0, int interval_minutes = 30, int hour = 0, int min = 0){

    tickTime current;

    if (hour == 0 && min ==0 ){
        current = get_current_tick_time();
    }else{
        current.hour = hour;
        current.minute = min;
    }


    return generate_today_tick_times(current, cnt, interval_minutes);
}

int main(int argc, char* argv[]) {
    int interval = 30;
    int cnt = 0;
    int hour = 0;
    int min = 0;

    cnt = std::stoi(argv[1]);
    interval = std::stoi(argv[2]);

    if (argc == 5){
        hour = std::stoi(argv[3]);
        min = std::stoi(argv[4]);
    }




    std::vector<tickTime> tick_times = min_vector(cnt, interval, hour, min);

    std::cout << "今天过滤后的有效时间点 (共 " << tick_times.size() << " 个):\n";
    for (const auto& t : tick_times) {
        std::cout << std::setw(2) << std::setfill('0') << t.hour << ":" 
                  << std::setw(2) << std::setfill('0') << t.minute << "\n";
    }
              
    return 0;
}