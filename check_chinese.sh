#!/bin/bash

# 为了保证代码的多编码兼容性，src文件夹内的内容(代码部分)统一使用英文，因此需要检查代码中是否包含中文注释，要翻译为英文

# 遍历当前目录下的所有文件
find ./src -type f | while read file
do
    # 使用 grep 命令检测文件内容是否包含中文字符
    if grep -q -P "[\x{4e00}-\x{9fa5}]" "$file"
    then
        # 如果包含中文字符，输出文件名
        echo "$file"
    fi
done
