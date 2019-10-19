//
// Created by ahtyamovdanil on 19.07.2019.
//

#ifndef MAIN_PROJECT_FATAPI_H
#define MAIN_PROJECT_FATAPI_H

#include <string>
#include <windows.h>
#include <math.h>


struct geom{
    unsigned long cylinders;
    unsigned long bytesPerSector;
    int mediaType;
    unsigned long sectorsPerTrack;
    unsigned long tracksPerCylinder;
};
struct Boot{
        unsigned char FS_ident[8];
        int BytesPerSector;
        int SectorsPerCluster;
        int ReservedSectors;
        int FatCopies;
        int RootEntries;
        int SectorsOnDisk;
        int MediaType;
        int SectorsPerFAT;
        int SectorsPerTrack;
        int Heads;
        int HiddenSectors;
        int DiskNumber;
        int RootFolderCluser;
        const char* FS_type;
};

geom getDiskGeometry(LPCSTR path);
Boot getBootRecord(LPCSTR wszPath);
BYTE* getFAT(LPCSTR path);
BYTE* getRaw(LPCSTR wszPath, int start_sector, int n_sectors);
BYTE* getFolder(LPCSTR path);
BYTE* getRootFolder(LPCSTR path);
void printHex(BYTE* array, int length);
unsigned int hexToDec(BYTE* array, int length);
std::string getAttributes(int mask);

#endif //MAIN_PROJECT_FATAPI_H
