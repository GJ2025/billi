#!/bin/bash
# 假设 $d 已经定义
target=$1
output="$s/${target}.txt"
limit=$2

# --- 新增：检查目标目录是否存在 ---
if [ -z "$target" ]; then
    echo "错误: 未指定目标文件夹。"
    exit 1
fi

target_dir="$d/$target/"

if [ ! -d "$target_dir" ]; then
    echo "错误: 目录 $target_dir 不存在。"
    exit 1
fi
# ---------------------------------

# 如果 limit 存在，则设置参数字符串为 "-l $limit"，否则为空
limit_opt=""
if [ -n "$limit" ]; then
    limit_opt="-l $limit"
fi

# 在调用时使用变量 target_dir
./bin/parse_tick -d "$target_dir" -a $limit_opt > "$output"
./bin/parse_tick -d "$target_dir" -m $limit_opt >> "$output"
./bin/parse_tick -d "$target_dir" -w $limit_opt >> "$output"
./bin/parse_tick -d "$target_dir" -p $limit_opt >> "$output"
./bin/parse_tick -d "$target_dir" -s $limit_opt >> "$output"
./bin/parse_tick -d "$target_dir" -b $limit_opt >> "$output"

echo "Done: $output"


