#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdatomic.h>
#include <unistd.h>
#include <string>
#include <climits>
#include <libgen.h>

const std::string isoName = "LanternOS_2025.4-1_x86_64.iso";

int stepNum = 0, stepTotal = 6;

const size_t MAX_FILE_NUM = 1024;

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
    const std::string kernelPath = fullPath + "/x86_64/boot/LanternKernel.bin"; // 内核路径
    const std::string linkerPath = srcPath + "/KernelLinker.lds"; // KernelLinker.lds
    const std::string isoModulePath = fullPath + "/x86_64"; // ISO打包时取的文件夹

    bool bad_exit = false;

    std::string Links[MAX_FILE_NUM * 2]; // 链接出的.o文件
    size_t Links_size = 0;

    try {
        printf("[%d/%d] 清理构建文件...\n", stepNum++, stepTotal);
        std::remove(isoPath.c_str());

        printf("[%d/%d] 编译汇编文件...\n", stepNum++, stepTotal);
        std::ifstream asmLists("asmLists.txt", std::ios::in);
        if(!asmLists) {
            std::cerr << "Failed to read asmLists.txt, stop making." << std::endl;
            bad_exit = true;
            throw "Error";
        }
        std::string asmFiles[MAX_FILE_NUM];
        size_t asmFiles_size = 0;
        for(; getline(asmLists, asmFiles[asmFiles_size]); asmFiles[asmFiles_size] = rootPath + asmFiles[asmFiles_size], ++asmFiles_size);
        asmLists.close();
        for(int i = 0; i < asmFiles_size; ++i) {
            Links[Links_size + i] = asmFiles[i];
            Links[Links_size + i].replace(Links[Links_size + i].find(".S"), 2, ".o");
            printf("  | 正在编译 %s 到 %s ...\n", asmFiles[i].c_str(), Links[Links_size + i].c_str());
            system(("gcc -c \'" + asmFiles[i] + "\' -o \'" + Links[Links_size + i] + "\' -ffreestanding -O2 -Wall -fno-stack-protector -Wextra -I\"" + includePath + "\"").c_str());
        }
        Links_size += asmFiles_size;

        printf("[%d/%d] 编译C文件...\n", stepNum++, stepTotal);
        std::ifstream cLists("cLists.txt", std::ios::in);
        if(!cLists) {
            std::cerr << "Failed to read cLists.txt, stop making." << std::endl;
            bad_exit = true;
            throw "Error";
        }
        std::string cFiles[MAX_FILE_NUM];
        size_t cFiles_size = 0;
        for(; getline(cLists, cFiles[cFiles_size]); cFiles[cFiles_size] = rootPath + cFiles[cFiles_size], ++cFiles_size);
        cLists.close();
        for(int i = 0; i < cFiles_size; ++i) {
            Links[Links_size + i] = cFiles[i];
            Links[Links_size + i].replace(Links[Links_size + i].find(".c"), 2, ".o");
            printf("  | 正在编译 %s 到 %s ...\n", cFiles[i].c_str(), Links[Links_size + i].c_str());
            system(("gcc -c \'" + cFiles[i] + "\' -o \'" + Links[Links_size + i] + "\' -ffreestanding -O2 -Wall -fno-stack-protector -Wextra -I\"" + includePath + "\"").c_str());
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
        std::string linkCommand = "ld -n -T \'" + linkerPath + "\' -o \'" + kernelPath + "\'";
        for(int i = 0; i < Links_size; ++i) {
            linkCommand += " \'" + Links[i] + "\'";
        }
        system(linkCommand.c_str());
        if(access(kernelPath.c_str(), F_OK) != 0) {
            std::cerr << "  | 内核未生成。" << std::endl;
            bad_exit = true;
            throw "Error";
        }

        printf("[%d/%d] 生成ISO镜像...\n", stepNum++, stepTotal);
        system(("grub-mkrescue \'" + isoModulePath + "\' -o \'" + isoPath + "\' 2> /dev/null").c_str());
        if(access(isoPath.c_str(), F_OK) != 0) {
            std::cerr << "  | ISO镜像未生成。" << std::endl;
            bad_exit = true;
            throw "Error";
        }
    } catch (...) {}

    printf("[%d/%d] 清理编译文件...\n", stepNum++, stepTotal);
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
