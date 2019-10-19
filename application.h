#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <QTableWidget>
#include <windows.h>
#include <fatAPI.h>

class Application
{
public:
    Application(LPCSTR path);
    Application();
    LPCSTR path;
    int fat_type;
    void setBootTable(QTableWidget *table);
    void setFatTable(QTableWidget *table);
    void setRootFolder(QTableWidget *qtable);
    void setRootFolder(QTableWidget *qtable, int startSector, int n);
    QString getStringFromUnsignedChar(unsigned char *str, int rev);
    std::string params[21] = {
        "Идентификатор файловой системы",
        "Байт в секторе",
        "Секторов в кастере",
        "Зарезервировано секторов",
        "Количество таблиц FAT",
        //"Объектов в корневом каталоге",
        "Секторов на диске",
        "Метка носителя",
        //"Секторов в кластере",
        "Секторов в FAT",
        "Секторов в дорожке",
        "Головок",
        "Скрытых секторов",
        //"Режим обновления",
        "Номер устройства",
        //"Номер версии",
        "Кластер с корневым каталогом",
        "Тип файловой системы"
        //"Среда носителя",
        //"Секторов на дорожке",
        //"Поверхностей",
        //"Скрытых секторов",
        //"Общее количество секторов",
        //"Номер активной FAT таблицы",
        //"Версия FAT32",
        //"Номер начального кластера root",
        //"Номер сектора FInfo",
        //"Номер сектора резервной копии загр. диска",
        //"Номер логического диска",
        //"Метка логического диска",
        //"Тип файловой системы"
    };
    std::string alph = "абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
};

#endif // APPLICATION_H
