#!/bin/bash

# 指定要遍历的文件夹路径
folder_path="./build"

# 检查文件夹是否存在
if [ ! -d "$folder_path" ]; then
    echo 'No build folder.'
    exit 1
fi

# 获取文件夹中的所有文件名
files=()

# 遍历文件夹中的文件
for file in "$folder_path"/*; do
    if [ -f "$file" ]; then
        files+=("$(basename "$file")")
    fi
done

# 检查是否有文件
if [ ${#files[@]} -eq 0 ]; then
    echo 'No build files found.'
    exit 1
fi

# 按字典序排序文件名
sorted_files=($(printf "%s\n" "${files[@]}" | sort -r))

# 获取最小的文件名
max_file="${sorted_files[0]}"

# 输出结果
qemu-system-x86_64 \
    -drive "file=$folder_path/$max_file,format=raw,index=0,media=disk" \
    -bios /usr/share/ovmf/x64/OVMF.4m.fd \
    -cpu kvm64,+smep,+smap \
    -smp sockets=1,dies=1,cores=4,threads=2 \
    -m 4G \
    --machine q35
