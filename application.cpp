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
    qtable->setRowCount(12);
    qtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtable->setColumnWidth(0, 170);
    qtable->setHorizontalHeaderLabels({ "Параметр", "Значение"});

    for(int i=0; i<12; i++){
        qtable->setItem(i, 0, new QTableWidgetItem(params[i].c_str()));
    }

    auto boot = getBootRecord(path);
    qtable->setItem(0, 1, new QTableWidgetItem(std::to_string(boot.BytesPerSector).c_str()));
    qtable->setItem(1, 1, new QTableWidgetItem(std::to_string(boot.SectorsPerCluster).c_str()));
    qtable->setItem(2, 1, new QTableWidgetItem(std::to_string(boot.ReservedSectors).c_str()));
    qtable->setItem(3, 1, new QTableWidgetItem(std::to_string(boot.FatCopies).c_str()));
    qtable->setItem(4, 1, new QTableWidgetItem(std::to_string(boot.SectorsOnDisk).c_str()));
    qtable->setItem(5, 1, new QTableWidgetItem(std::to_string(boot.MediaType).c_str()));
    qtable->setItem(6, 1, new QTableWidgetItem(std::to_string(boot.SectorsPerFAT).c_str()));
    qtable->setItem(7, 1, new QTableWidgetItem(std::to_string(boot.SectorsPerTrack).c_str()));
    qtable->setItem(8, 1, new QTableWidgetItem(std::to_string(boot.Heads).c_str()));
    qtable->setItem(9, 1, new QTableWidgetItem(std::to_string(boot.HiddenSectors).c_str()));
    qtable->setItem(10, 1, new QTableWidgetItem(std::to_string(boot.DiskNumber).c_str()));
    qtable->setItem(11, 1, new QTableWidgetItem(std::to_string(boot.RootFolderCluser).c_str()));

    int a = 92;
    QString test = "абвгд";
    int jojo = test.at(1).unicode();
    std::cout << "----" << jojo << "----" << std::endl;
    //QString abc = QString((unsigned char)a);
    //qtable->setItem(11, 1, new QTableWidgetItem(abc.toLocal8Bit().data()));
    //qtable->setItem(0, 0, new QTableWidgetItem(lol));

}

void Application::setFatTable(QTableWidget *qtable){
    int columns = 10;
    int rows = 1000;
    qtable->setColumnCount(columns);
    qtable->setRowCount(rows);
    qtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for(int i=0; i<10; i++){
        qtable->setColumnWidth(i, 50);
    }
    qtable->setHorizontalHeaderLabels({ "0", "1", "2", "3", "4", "4", "6", "7", "8", "9"});
    for(int i=0; i<1000; i++){
        qtable->setVerticalHeaderItem(i, new QTableWidgetItem(std::to_string(i*10).c_str()));
    }
    auto fatTable = getFAT(path);
    for(int i=0; i<rows; i++){
        for(int j=0; j<columns; j++){
            unsigned int value = hexToDec(fatTable+(i*10+j)*4, 4);
            if(value >= 268435448){
                qtable->setItem(i, j, new QTableWidgetItem("EOC"));
                continue;
            }
            qtable->setItem(i, j, new QTableWidgetItem(std::to_string(value).c_str()));
        }
    }
    qtable->setItem(0, 0, new QTableWidgetItem("RES"));
    qtable->setItem(0, 1, new QTableWidgetItem("RES"));
}

void Application::setRootFolder(QTableWidget *qtable){
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
        unsigned char fileName[8] = {rootArray[i], rootArray[i+1], rootArray[i+2], rootArray[i+3],
                                  rootArray[i+4], rootArray[i+5], rootArray[i+6], rootArray[i+7]};

        //QString testStr = getStringFromUnsignedChar(fileName, 8);
        //qDebug() << testStr;
        //auto qb = QByteArray(rootArray,8);
        //std::cout << "----" << QString::fromUtf8(reinterpret_cast<char*>(fileName), 8) << "----" << std::endl;
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
            //qtable->item(i/32, 0)->setBackground(Qt::lightGray);
            continue;
        }
        else{
            qtable->setItem(i/32, 0, new QTableWidgetItem(name.c_str()));
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
            //qtable->setItem(i/32, 2, new QTableWidgetItem("КАТ"));
            if(rootArray[i] != 'x'){
                qtable->item(i/32, 0)->setBackground(Qt::gray);
                qtable->item(i/32, 1)->setBackground(Qt::gray);
                qtable->item(i/32, 2)->setBackground(Qt::gray);
                qtable->item(i/32, 3)->setBackground(Qt::gray);
                qtable->item(i/32, 4)->setBackground(Qt::gray);
                //qtable->item(i/32, 4)->setBackground(Qt::gray);
            }

        }

    }
}

void Application::setRootFolder(QTableWidget *qtable, int startCluster, int nSectors){
    if(startCluster == 0){
        setRootFolder(qtable);
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
                            (startCluster-boot.FatCopies)*boot.SectorsPerCluster , nSectors);
    for(int i=0; i<8192; i+=32){
        std::string myStr;
        if(rootArray[i]==229)
            rootArray[i] = 'x';
        unsigned char fileName[8] = {rootArray[i], rootArray[i+1], rootArray[i+2], rootArray[i+3],
                                  rootArray[i+4], rootArray[i+5], rootArray[i+6], rootArray[i+7]};
        unsigned char fileExt[3] = {rootArray[i+8], rootArray[i+9], rootArray[i+10]};
        BYTE fileAtr = rootArray[i+11];
        std::string name(reinterpret_cast<char*>(fileName), 8);
        std::string ext(reinterpret_cast<char*>(fileExt), 3);
        unsigned int size = hexToDec(rootArray+i+26, 2);
        unsigned int entry = hexToDec(rootArray+i+28, 4);
        //QString str = (reinterpret_cast<char *>(array));
        /*
        std::string str = "фыва";
        str[0] = 'р';
        for(int j=0; j<8; j++){
            std::cout<<(int)s[j]<<std::endl;
            if(256+(int)s[j]>127 && 256+(int)s[j]<256){
                str[j] = 'э';//alph[array[j]-128];
                std::cout<<"ololo"<<std::endl;
            }
        }
        */
        if(name[0]==0)
            break;
        if(rootArray[i+11]==15){
            qtable->setItem(i/32, 0, new QTableWidgetItem("lfn"));
            //qtable->item(i/32, 0)->setBackground(Qt::lightGray);
            continue;
        }
        else{
            qtable->setItem(i/32, 0, new QTableWidgetItem(name.c_str()));
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
            //qtable->setItem(i/32, 2, new QTableWidgetItem("КАТ"));
            if(rootArray[i] != 'x'){
                qtable->item(i/32, 0)->setBackground(Qt::gray);
                qtable->item(i/32, 1)->setBackground(Qt::gray);
                qtable->item(i/32, 2)->setBackground(Qt::gray);
                qtable->item(i/32, 3)->setBackground(Qt::gray);
                qtable->item(i/32, 4)->setBackground(Qt::gray);
                //qtable->item(i/32, 4)->setBackground(Qt::gray);
            }

        }

    }
}

QString Application::getStringFromUnsignedChar(unsigned char *str, int rev)
{

    QString s;
    QString result = "";
    //int rev = std::strlen(str);

    // Print String in Reverse order....
    for ( int i = 0; i<rev; i++)
        {
           s = QString("%1").arg(str[i],0,16);

           if(s == "0"){
              s="00";
             }
         result.append(s);

         }
   return result;
}
