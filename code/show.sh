#!/bin/bash
target=$1
output="$s/${target}.txt"
limit=$2

# 如果 limit 存在，则设置参数字符串为 "-l $limit"，否则为空
limit_opt=""
if [ -n "$limit" ]; then
    limit_opt="-l $limit"
fi

# 在调用时直接嵌入 $limit_opt
./bin/parse_tick -d "$d/$target/" -a $limit_opt > "$output"
./bin/parse_tick -d "$d/$target/" -m $limit_opt >> "$output"
./bin/parse_tick -d "$d/$target/" -w $limit_opt >> "$output"
./bin/parse_tick -d "$d/$target/" -p $limit_opt >> "$output"
./bin/parse_tick -d "$d/$target/" -s $limit_opt >> "$output"
./bin/parse_tick -d "$d/$target/" -b $limit_opt >> "$output"

echo "Done: $output"
