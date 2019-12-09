#include "application.h"
#include <iostream>
#include <QLocale>
#include <QByteArray>
#include <cstring>
#include <QDebug>

Application::Application(LPCSTR path){
    this->path = path;
}

Application::Application(){}

void Application::setBootTable(QTableWidget *qtable){
    qtable->setColumnCount(2);
    qtable->setRowCount(14);
    qtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtable->setColumnWidth(0, 170);
    qtable->setHorizontalHeaderLabels({ "Параметр", "Значение"});

    for(int i=0; i<14; i++){
        qtable->setItem(i, 0, new QTableWidgetItem(params[i].c_str()));
    }

    auto boot = getBootRecord(path);
    qtable->setItem(0, 1, new QTableWidgetItem(std::string(reinterpret_cast< char const* >(boot.FS_ident)).c_str()));
    qtable->setItem(1, 1, new QTableWidgetItem(std::to_string(boot.BytesPerSector).c_str()));
    qtable->setItem(2, 1, new QTableWidgetItem(std::to_string(boot.SectorsPerCluster).c_str()));
    qtable->setItem(3, 1, new QTableWidgetItem(std::to_string(boot.ReservedSectors).c_str()));
    qtable->setItem(4, 1, new QTableWidgetItem(std::to_string(boot.FatCopies).c_str()));
    qtable->setItem(5, 1, new QTableWidgetItem(std::to_string(boot.SectorsOnDisk).c_str()));
    qtable->setItem(6, 1, new QTableWidgetItem(std::to_string(boot.MediaType).c_str()));
    qtable->setItem(7, 1, new QTableWidgetItem(std::to_string(boot.SectorsPerFAT).c_str()));
    qtable->setItem(8, 1, new QTableWidgetItem(std::to_string(boot.SectorsPerTrack).c_str()));
    qtable->setItem(9, 1, new QTableWidgetItem(std::to_string(boot.Heads).c_str()));
    qtable->setItem(10, 1, new QTableWidgetItem(std::to_string(boot.HiddenSectors).c_str()));
    qtable->setItem(11, 1, new QTableWidgetItem(std::to_string(boot.DiskNumber).c_str()));
    qtable->setItem(12, 1, new QTableWidgetItem(std::to_string(boot.RootFolderCluser).c_str()));
    if(std::string(boot.FS_type) == "FAT16"){
        this->fat_type = 16;
        qtable->setItem(12,0, new QTableWidgetItem("Макс число элементов корневого каталога"));
    }
    else
        this->fat_type = 32;
    qtable->setItem(13, 1, new QTableWidgetItem(boot.FS_type));


    int a = 92;
    QString test = "абвгд";
    int jojo = test.at(1).unicode();
    std::cout << "----" << jojo << "----" << std::endl;
}

void Application::setFatTable(QTableWidget *qtable){
    auto boot = getBootRecord(path);
    int size;
    (this->fat_type == 32)? size = 4: size = 2;
    int columns = 10;
    //int rows = 1254;
    int rows = boot.SectorsPerFAT * boot.BytesPerSector /size / 10;
    std::cout << rows << std::endl;
    qtable->setColumnCount(columns);
    qtable->setRowCount(rows);
    qtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0; i<10; i++){
        qtable->setColumnWidth(i, 50);
    }
    qtable->setHorizontalHeaderLabels({ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
    for(int i=0; i<rows; i++){
        qtable->setVerticalHeaderItem(i, new QTableWidgetItem(std::to_string(i*10).c_str()));
    }
    auto fatTable = getFAT(path);
    for(int i=0; i<rows; i++){
        for(int j=0; j<columns; j++){
            unsigned int value = hexToDec(fatTable+(i*10+j)*size, size);
            if((size==4 && value >= 268435448)||(size==2 && value >= 65528)){
                qtable->setItem(i, j, new QTableWidgetItem("EOC"));
                continue;
            }
            qtable->setItem(i, j, new QTableWidgetItem(std::to_string(value).c_str()));
        }
    }
    qtable->setItem(0, 0, new QTableWidgetItem("RES"));
    qtable->setItem(0, 1, new QTableWidgetItem("RES"));
}

void Application::setFolderTable(QTableWidget *qtable){
    qtable->clear();
    qtable->setColumnCount(5);
    qtable->setRowCount(1000);
    qtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtable->setColumnWidth(0, 170);
    qtable->setHorizontalHeaderLabels({ "Имя", "Расширение", "Атрибуты", "1-й кластер", "Размер (байт)"});
    auto rootArray = getRootFolder(path);
    //auto rootArray = getRaw(path, 8324, 2);

    for(int i=0; i<8192; i+=32){
        std::string myStr;
        if(rootArray[i]==229)
            rootArray[i] = 'x';
        QString filename = "        ";
        for(int j=0; j<8; j++){
            if((rootArray[i+j] >= 128)&&(rootArray[i+j] <= 159))
                filename[j] = QChar(rootArray[i+j]+912);
            else
                filename[j] = QChar(rootArray[i+j]);
        }
        unsigned char fileName[8] = {rootArray[i], rootArray[i+1], rootArray[i+2], rootArray[i+3],
                                  rootArray[i+4], rootArray[i+5], rootArray[i+6], rootArray[i+7]};

        unsigned char fileExt[3] = {rootArray[i+8], rootArray[i+9], rootArray[i+10]};
        BYTE fileAtr = rootArray[i+11];
        std::string name(reinterpret_cast<char*>(fileName), 8);
        std::string ext(reinterpret_cast<char*>(fileExt), 3);
        unsigned int size = hexToDec(rootArray+i+26, 2);
        unsigned int entry = hexToDec(rootArray+i+28, 4);
        if(name[0]==0)
            break;
        if(rootArray[i+11]==15){
            qtable->setItem(i/32, 0, new QTableWidgetItem("lfn"));
            continue;
        }
        else{
            qtable->setItem(i/32, 0, new QTableWidgetItem(filename));
            qtable->setItem(i/32, 1, new QTableWidgetItem(ext.c_str()));
            qtable->setItem(i/32, 3, new QTableWidgetItem(QString::number(size)));
            qtable->setItem(i/32, 4, new QTableWidgetItem(QString::number(entry)));
        }
        if(rootArray[i] == 'x'){
        }
        std::string attr = getAttributes(rootArray[i+11]);
        qtable->setItem(i/32, 2, new QTableWidgetItem(attr.c_str()));
        if((rootArray[i+11] & 16) != 0){
            if(rootArray[i] != 'x'){
                qtable->item(i/32, 0)->setBackground(Qt::gray);
                qtable->item(i/32, 1)->setBackground(Qt::gray);
                qtable->item(i/32, 2)->setBackground(Qt::gray);
                qtable->item(i/32, 3)->setBackground(Qt::gray);
                qtable->item(i/32, 4)->setBackground(Qt::gray);
            }

        }

    }
}

void Application::setFolderTable(QTableWidget *qtable, int startCluster, int nSectors){
    if(startCluster == 0){
        setFolderTable(qtable);
        return;
    }
    qtable->clear();
    qtable->setColumnCount(5);
    qtable->setRowCount(1000);
    qtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtable->setColumnWidth(0, 170);
    qtable->setHorizontalHeaderLabels({ "Имя", "Расширение", "Атрибуты", "1-й кластер", "Размер (байт)"});

    //auto rootArray = getRootFolder(path);

    auto boot = getBootRecord(path);

    auto rootArray = getRaw(path, boot.ReservedSectors + boot.SectorsPerFAT * boot.FatCopies +
                           (startCluster - boot.RootFolderCluser) * boot.SectorsPerCluster , nSectors);

    if(fat_type == 16){
        rootArray = getRaw(path, boot.ReservedSectors + boot.SectorsPerFAT * boot.FatCopies + 32
                           + (startCluster - 2) * boot.SectorsPerCluster, nSectors);

        std::cout <<  boot.ReservedSectors + boot.SectorsPerFAT * boot.FatCopies + 32
                      + (startCluster - 2) * boot.SectorsPerCluster << std::endl;

        std::cout <<  boot.ReservedSectors << ' '
                   << boot.SectorsPerFAT << ' '
                   << boot.FatCopies << ' '
                   << startCluster << ' '
                   << boot.SectorsPerCluster << std::endl;
    }


    for(int i=0; i<8192; i+=32){
        std::string myStr;
        if(rootArray[i]==229)
            rootArray[i] = 'x';
        QString filename = "        ";
        for(int j=0; j<8; j++){
            if((rootArray[i+j] >= 128)&&(rootArray[i+j] <= 159))
                filename[j] = QChar(rootArray[i+j]+912);
            else
                filename[j] = QChar(rootArray[i+j]);
        }
        unsigned char fileName[8] = {rootArray[i], rootArray[i+1], rootArray[i+2], rootArray[i+3],
                                  rootArray[i+4], rootArray[i+5], rootArray[i+6], rootArray[i+7]};
        unsigned char fileExt[3] = {rootArray[i+8], rootArray[i+9], rootArray[i+10]};
        BYTE fileAtr = rootArray[i+11];
        //std::string name(reinterpret_cast<char*>(fileName), 8);
        std::string ext(reinterpret_cast<char*>(fileExt), 3);
        unsigned int size = hexToDec(rootArray+i+26, 2);
        unsigned int entry = hexToDec(rootArray+i+28, 4);

        if(filename[0]==0)
            break;
        if(rootArray[i+11]==15){
            qtable->setItem(i/32, 0, new QTableWidgetItem("lfn"));
            //qtable->item(i/32, 0)->setBackground(Qt::lightGray);
            continue;
        }
        else{
            qtable->setItem(i/32, 0, new QTableWidgetItem(filename));
            qtable->setItem(i/32, 1, new QTableWidgetItem(ext.c_str()));
            qtable->setItem(i/32, 3, new QTableWidgetItem(QString::number(size)));
            qtable->setItem(i/32, 4, new QTableWidgetItem(QString::number(entry)));
        }
        if(rootArray[i] == 'x'){
            //qtable->item(i/32, 0)->setBackground(Qt::gray);
        }
        std::string attr = getAttributes(rootArray[i+11]);
        qtable->setItem(i/32, 2, new QTableWidgetItem(attr.c_str()));
        if((rootArray[i+11] & 16) != 0){
            if(rootArray[i] != 'x'){
                qtable->item(i/32, 0)->setBackground(Qt::gray);
                qtable->item(i/32, 1)->setBackground(Qt::gray);
                qtable->item(i/32, 2)->setBackground(Qt::gray);
                qtable->item(i/32, 3)->setBackground(Qt::gray);
                qtable->item(i/32, 4)->setBackground(Qt::gray);
            }

        }

    }
}

QString Application::getStringFromUnsignedChar(unsigned char *str, int rev)
{

    QString s;
    QString result = "";
    // Print String in Reverse order....
    for ( int i = 0; i<rev; i++){
           s = QString("%1").arg(str[i],0,16);
           if(s == "0"){
              s="00";
           }
         result.append(s);

        }
   return result;
}
