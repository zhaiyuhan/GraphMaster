#include "FileHelper.h"
#include <QFile>
#include <QMap>
#include <QDebug>
#include <QTextStream>
FileHelper::FileHelper(QObject *parent)
    : QObject{parent}
{

}

QList<QList<int>> FileHelper::ConvertToGraph(QString filepath)
{
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly |QIODevice::Text)){

    }else {
        qDebug() << "打开失败";
    }
    QByteArray array;
    while (!file.atEnd()){
        array += file.readLine();
    }
    QString str = array;
    QList<QString> list = str.split("\n");
    QMap<QList<int>, int> map;
    for (int i = 0; i < list.size(); ++i) {
        QList<QString> list1 = list[i].split(' ');
        for (int j = 0; j < list1.size() - 2; ++j) {
            if (j > 0 && list1[j] != "-1"){
                QList<int> keyList;
                if (list1[0].toInt() < j){
                    keyList.append(list1[0].toInt());
                    keyList.append(j);
                } else {
                    keyList.append(j);
                    keyList.append(list1[0].toInt());
                }
                map.insert(keyList, list1[j].toInt());
            }
        }
    }

    //最终结果是resList,一个QList嵌套QList的格式
    QList<QList<int>> resList;
    for (auto i = map.begin(); i != map.end(); i++) {
        QList<int> newList;
        if (i.value() != 0){
            newList.append(i.key());
            newList.append(i.value());
        }
        resList.append(newList);
    }

    file.close();
    return resList;
}
