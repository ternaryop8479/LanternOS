#ifndef LanternOS_Header_KERNEL_DISK_DISK_H
#define LanternOS_Header_KERNEL_DISK_DISK_H

#include <kernel/types.h>

int disk_init(); // Initialize the disk module 
size_t get_disk_counts(); // Get device counts
int mount(size_t diskCount, const char *targetPath); // Mount a disk to target path
int unmount(const char *targetPath); // Unmount a disk
void ergodic_directory(const char *directory); // Ergodic directory in target disk
size_t open(const char *filePath); // Open the file
size_t read(size_t fileNumber, char *buffer, size_t readSize, size_t bufferSize); // Read a file with target size(readSize=0 stands for all contents)
size_t close(size_t fileNumber); // Close the file

#endif
