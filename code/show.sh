#!/bin/bash
# 用法: ./run_parse.sh <文件夹名>
# 示例: ./run_parse.sh chuanheng

target=$1
output="$s/${target}.txt"

# 依次执行，第一次覆盖，后续追加
./bin/parse_tick -d "$d/$target/" -a >  "$output"
./bin/parse_tick -d "$d/$target/" -m >> "$output"
./bin/parse_tick -d "$d/$target/" -w >> "$output"
./bin/parse_tick -d "$d/$target/" -p >> "$output"
./bin/parse_tick -d "$d/$target/" -s >> "$output"
./bin/parse_tick -d "$d/$target/" -b >> "$output"

echo "Done: $output"

