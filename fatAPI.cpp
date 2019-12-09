//
// Created by ahtyamovdanil on 20.07.2019.
//
#include "fatAPI.h"
#include <windows.h>
//#include <assert.h>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include <certbcli.h>

using namespace std;

//tested on fat32 only

unsigned int hexToDec(BYTE* array, int length){
    unsigned int result = 0;
    for (int i=0; i<length; i++){
        result += (int)array[i]*pow(16, i*2);
    }
    return result;
}

geom getDiskGeometry(LPCSTR path){
    auto hDevice = CreateFileA(path,          // drive to open
                               0,                // no access to the drive
                               FILE_SHARE_READ | // share mode
                               FILE_SHARE_WRITE,
                               nullptr,             // default security attributes
                               OPEN_EXISTING,    // disposition
                               0,                // file attributes
                               nullptr              // do not copy file attributes
    );

    DISK_GEOMETRY pdg = { 0 };
    DWORD junk     = 0;
    auto bResult = DeviceIoControl(hDevice,                       // device to be queried
                                   IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
                                   nullptr, 0,                       // no input buffer
                                   &pdg, sizeof(pdg),            // output buffer
                                   &junk,                         // # bytes returned
                                   (LPOVERLAPPED) NULL);          // synchronous I/O

    return {pdg.Cylinders.LowPart,
            pdg.BytesPerSector,
            pdg.MediaType,
            pdg.SectorsPerTrack,
            pdg.TracksPerCylinder};
}

//return structure with info from boot record
boot getBootRecord(LPCSTR path){
    BYTE* bootSector = getRaw(path, 0, 1);
    boot bootStruct;
    for(int i=0; i<8;i++)
        bootStruct.FS_ident[i] = bootSector[i+3];
    bootStruct.BytesPerSector =  hexToDec(bootSector+11, 2);
    bootStruct.SectorsPerCluster = (int)bootSector[13];
    bootStruct.ReservedSectors = hexToDec(bootSector+14, 2);
    bootStruct.FatCopies = (int)bootSector[16];
    bootStruct.SectorsOnDisk = hexToDec(bootSector+19, 2);
    if (bootStruct.SectorsOnDisk == 0)
        bootStruct.SectorsOnDisk = hexToDec(bootSector+32, 4);
    bootStruct.MediaType = (int)bootSector[21];
    bootStruct.SectorsPerFAT = hexToDec(bootSector+22, 2);
    if (bootStruct.SectorsPerFAT == 0)
        bootStruct.SectorsPerFAT = hexToDec(bootSector+36, 4);
    bootStruct.SectorsPerTrack = hexToDec(bootSector+24, 2);
    bootStruct.Heads = hexToDec(bootSector+26, 2);
    bootStruct.HiddenSectors = hexToDec(bootSector+28, 4);
    bootStruct.DiskNumber = (int)bootSector[36];
    bootStruct.RootFolderCluser = hexToDec(bootSector+44, 4);
    bootStruct.FS_type = "FAT32";
    if(bootStruct.SectorsOnDisk/bootStruct.SectorsPerCluster < 65525){
        //bootStruct.SectorsPerFAT = hexToDec(bootSector+22, 2);
        bootStruct.FS_type = "FAT16";
        bootStruct.RootFolderCluser = hexToDec(bootSector+17, 2);
    }

    return bootStruct;
}

//return array of bytes from disk sectors
BYTE* getRaw(LPCSTR path, int startSector, int nSectors){
    auto sectorSize = getDiskGeometry(path).bytesPerSector;
    DWORD nRead;
    BYTE* buf = new  BYTE[nSectors * sectorSize];
    HANDLE hDisk = CreateFileA(path,
                              GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, 0, nullptr);

    SetFilePointer(hDisk, startSector*sectorSize, 0, FILE_BEGIN); //IDistanceToMove takes n*sector_size //2212
    auto ok = ReadFile(hDisk, buf, nSectors*sectorSize, &nRead, nullptr); //ReadFile 3rd arg takes n*sector_size
    CloseHandle(hDisk);
    if(!ok){
        std::cout << "terminated" << " " << string(path) << std::endl;
        exit(0);
    }
    return buf;
}

//return array of BYTE with fat table
BYTE* getFAT(LPCSTR path){
    auto boot = getBootRecord(path);

    /*
    BYTE* bootSector = getRaw(path, 0, 1);
    //size of fat = 2 bytes
    BYTE size_low = bootSector[36];
    BYTE size_high = bootSector[37];
    //first sector with fat = 2 bytes
    BYTE start_low = bootSector[14];
    BYTE start_high = bootSector[15];
    //convert to decimal format
    int size = 16*16*(int)size_high + (int)size_low;
    std::cout << size << std::endl;
    int start = 16*16*(int)start_high + (int)start_low;
    */
    return getRaw(path, boot.ReservedSectors, boot.SectorsPerFAT);
}

//print array of hex numbers to console
void printHex(BYTE* array, int length){
    for (int currentpos=0;currentpos < length;currentpos++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(array[currentpos]) << " ";
        if((currentpos+1) % 4 == 0)
            cout <<' ';
        if((currentpos+1) % 16 == 0)
            cout <<std::endl;
    }
}

BYTE* getRootFolder(LPCSTR path){
    auto boot = getBootRecord(path);

    //boot.
    auto rootEntry = boot.ReservedSectors + boot.SectorsPerFAT * boot.FatCopies;

    return getRaw(path, rootEntry, 252);
};


string getAttributes(int mask){
    string attr[8] = {"RO", "СКР", "СИСТ", "МТК", "КАТ", "АРХ", "УСТР", "РЕЗ"};
    string attrList = "";
    for(int i=0; i<8; i++){
        if((mask & (int)pow(2,i)) !=0){
            attrList += attr[i]+ ' ';
        }
    }
    return attrList;
}
















