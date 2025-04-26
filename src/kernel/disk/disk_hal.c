#include <kernel/disk/disk.h>

static EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fileSystem = NULL;
static EFI_FILE_PROTOCOL *rootDirectory = NULL;
static EFI_SYSTEM_TABLE *SystemTable = NULL;
static UINTN handleCount = 0;
static EFI_HANDLE *handleBuffer = NULL;

int disk_init(EFI_SYSTEM_TABLE *systemTable) {
    SystemTable = systemTable;
    EFI_STATUS status;
    status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handleCount, &handleBuffer);
    if(EFI_ERROR(status)) {
        return status;
    }

    for(UINTN i = 0; i < handleCount; ++i) {
        status = gBS->HandleProtocol(handleBuffer[i], &gEfiSimpleFileSystemProtocolGuid, (void **)&fileSystem);
        if(EFI_ERROR(status)) {
            continue;
        }
    }
}
size_t get_disk_counts(); // Get device counts
int mount(size_t diskCount, const char *targetPath); // Mount a disk to target path
int unmount(const char *targetPath); // Unmount a disk
void ergodic_directory(const char *directory); // Ergodic directory in target disk
size_t open(const char *filePath); // Open the file
size_t read(size_t fileNumber, char *buffer, size_t readSize, size_t bufferSize); // Read a file with target size(readSize=0 stands for all contents)
size_t close(size_t fileNumber); // Close the file
