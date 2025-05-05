#include <cstddef>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdatomic.h>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <climits>
#include <libgen.h>

const std::string isoName = "LanternOS_2025.5-1_x86_64.img";

int stepNum = 0, stepTotal = 6;

const size_t MAX_FILE_NUM = 1024;

size_t calculate_folder_size_MB(const std::filesystem::path& folder_path) {
    size_t total_size = 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path)) {
        if (entry.is_regular_file()) {
            total_size += entry.file_size();
        }
    }

    size_t result = static_cast<uint64_t>(total_size / static_cast<double>(1024 * 1024)) + 1;

    return result;
}

int main(int argc, char *argv[]) {
    const char *cfullPath; { // 一种优雅不失美观又好用的变量初始化方法
        char fullPath_FILE[PATH_MAX];

        if(realpath(argv[0], fullPath_FILE) == NULL) {
            std::cerr << "Failed to get realpath, stop making." << std::endl;
            return 1;
        }

        cfullPath = dirname(fullPath_FILE);
    }

    const std::string fullPath = cfullPath;
    const std::string &rootPath = fullPath; // 项目根路径
    const std::string isoPath = fullPath + "/build/" + isoName; // ISO输出路径
    const std::string srcPath = fullPath + "/src"; // 资源路径
    const std::string includePath = srcPath + "/include"; // include路径
    const std::string isoModulePath = fullPath + "/x86_64"; // ISO打包时取的文件夹
    const std::string kernelPath = isoModulePath + "/EFI/BOOT/BOOTX64.EFI"; // 内核路径
    const std::string mountPath = fullPath + "/x86_64_mnt"; // 挂载点目录

    bool bad_exit = false;

    std::string Links[MAX_FILE_NUM * 2]; // 链接出的.o文件
    size_t Links_size = 0;

    try {
        printf("[%d/%d] 清理构建文件...\n", stepNum++, stepTotal);
        std::remove(isoPath.c_str());

        printf("[%d/%d] 编译makeLists中的文件...\n", stepNum++, stepTotal);
        std::ifstream makeLists("makeLists.txt", std::ios::in);
        if(!makeLists) {
            std::cerr << "Failed to read makeLists.txt, stop making." << std::endl;
            bad_exit = true;
            throw "Error";
        }
        std::string cFiles[MAX_FILE_NUM];
        size_t cFiles_size = 0;
        for(; getline(makeLists, cFiles[cFiles_size]); cFiles[cFiles_size] = rootPath + cFiles[cFiles_size], ++cFiles_size);
        makeLists.close();
        for(int i = 0; i < cFiles_size; ++i) {
            Links[Links_size + i] = cFiles[i];
            Links[Links_size + i].replace(Links[Links_size + i].find_last_of("."), Links[Links_size + i].size() - Links[Links_size + i].find_last_of("."), ".o");
            printf("  | 正在编译 %s 到 %s ...\n", cFiles[i].c_str(), Links[Links_size + i].c_str());
            system(("x86_64-w64-mingw32-gcc -c -I" + includePath + "/ -I/usr/include/efi/ -lefi -lgnuefi -fno-stack-protector -fno-builtin -O2 " + cFiles[i] + " -o " + Links[Links_size + i]).c_str());
        }
        Links_size += cFiles_size;

        printf("[%d/%d] 验证文件...\n", stepNum++, stepTotal);
        for(int i = 0; i < Links_size; ++i) {
            if(access(Links[i].c_str(), F_OK) != 0) {
                std::cerr << "  | 文件不存在: " << Links[i] << std::endl;
                bad_exit = true;
                throw "Error";
            }
            printf("  | 文件存在: %s\n", Links[i].c_str());
        }

        printf("[%d/%d] 链接内核...\n", stepNum++, stepTotal);
        std::string linkCommand = "x86_64-w64-mingw32-gcc -L/usr/x86_64-w64-mingw32/lib/ -L/usr/lib/ -L/usr/lib32/ -L/usr/lib64/ -nostdlib -e kernel_main -lgnuefi -lefi -Wl,--no-dynamic-linker,--entry=kernel_main,--subsystem,10 -O2 -o " + kernelPath;
        for(int i = 0; i < Links_size; ++i) {
            linkCommand += " \'" + Links[i] + '\'';
        }
        system(linkCommand.c_str());
        if(access(kernelPath.c_str(), F_OK) != 0) {
            std::cerr << "  | 内核未生成。" << std::endl;
            bad_exit = true;
            throw "Error";
        }

        printf("[%d/%d] 生成IMG镜像...\n", stepNum++, stepTotal);
        printf("  | 统计文件大小...\n");
        size_t buildSize = calculate_folder_size_MB(isoModulePath);
        printf("  |   文件大小: %uMB\n", (unsigned int)buildSize);
        printf("  | 创建空img文件...\n");
        system(("dd if=/dev/zero of=" + isoPath + " bs=1M count=" + std::to_string(buildSize) + " status=progress").c_str());
        printf("  | 构建文件系统...\n");
        system(("mkfs.vfat -n LanternOS " + isoPath).c_str());
        printf("  | 挂载文件系统...\n");
        system(("sudo mount " + isoPath + " " + mountPath).c_str());
        printf("  | 拷贝文件...\n");
        system(("sudo cp -r " + isoModulePath + "/* " + mountPath + "/").c_str());
        printf("  | 卸载文件系统...\n");
        system(("sudo umount " + mountPath).c_str());
        if(access(isoPath.c_str(), F_OK) != 0) {
            std::cerr << "  | ISO镜像未生成。" << std::endl;
            bad_exit = true;
            throw "Error";
        }
    } catch (const std::exception &e) {
        std::cerr << "Catched a error while compiling: " << e.what() << std::endl;
    } catch (...) {}

    printf("[%d/%d] 清理编译文件...\n", stepTotal, stepTotal);
    std::remove(kernelPath.c_str());
    for(int i = 0; i < Links_size; ++i) {
        std::remove(Links[i].c_str());
    }
    if(bad_exit) {
        std::cerr << "构建失败。" << std::endl;
        return 1;
    }

    printf("构建完成: %s\n", isoPath.c_str());
    return 0;
}
