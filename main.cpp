#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <fatAPI.h>
#include <windows.h>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //QString s = QString::fromUtf8(999,1);
    //qDebug() << s;
    return a.exec();

/*
    LPCSTR wszPath = "\\\\.\\F:";
    auto rootArray = getRootFolder(wszPath);
    for(int i=0; i<2048; i+=32){
        std::string myStr;
        //myStr.assign(kek[i], 8);
        if(rootArray[i]==229)
            rootArray[i] = 'x';
        unsigned char array[8] = {rootArray[i], rootArray[i+1], rootArray[i+2], rootArray[i+3],
                                  rootArray[i+4], rootArray[i+5], rootArray[i+6], rootArray[i+7]};
        std::string s(reinterpret_cast<char*>(array), 8);

        if(rootArray[i+11]==15)
            std::cout<<"lfn"<<std::endl;
        else
            std::cout<<s<< std::endl;
    }

    //int a = 85;
   // int b = 92;
    //setlocale( LC_ALL, "rus");
    //QString c = QString((unsigned char)a);
    //std::cout<<(unsigned char)a << ' ' <<(unsigned char)b << std::endl;
    //std::cout<<c.toLocal8Bit().toStdString() << ' ' <<(int)(unsigned char)b << std::endl;
    //QTextStream outStream(stdout);
        //outStream.setCodec(QTextCodec::codecForName("cp866"));
        //outStream << QString("Русский текст в консоли") << flush;
    return 0;
*/
}
