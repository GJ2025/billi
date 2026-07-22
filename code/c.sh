#!/bin/bash

# 获取传入的第一个参数作为目标目录，如果未传参数，则默认为当前目录 "."
TARGET_DIR="${1:-.}"

# 去除可能存在的末尾斜杠
TARGET_DIR="${TARGET_DIR%/}"

# 检查目录是否存在
if [ ! -d "$TARGET_DIR" ]; then
    echo "错误: 目录 $TARGET_DIR 不存在！"
    exit 1
fi

# 遍历传入目标目录下的所有文件
for file in "$TARGET_DIR"/*; do
    # 确保是普通文件
    if [ -f "$file" ]; then
        
        # ==================== 修复：使用 case 结构，完美兼容 sh 和 bash ====================
        case "$file" in
            *.txt)
                # 是 txt 文件，正常继续往下走
                ;;
            *)
                # 不是 txt 文件，跳过
                #echo "跳过非文本文件: $file"
                continue
                ;;
        esac
        # ===================================================================================

        # 检测文件编码检测结果是否包含 UTF-8
        if ! file "$file" | grep -q "UTF-8"; then
            echo "正在转换: $file"
            iconv -f GBK -t UTF-8 "$file" | tr '\r' '\n' > "${file}.tmp" && mv "${file}.tmp" "$file"
        #else
        #    echo "跳过已是 UTF-8 的文件: $file"
        fi
    fi
done

#echo "=========================================================================================================================="
echo "\r\n"