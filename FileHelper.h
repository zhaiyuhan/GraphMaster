#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>

class FileHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = nullptr);
    QList<QList<int>> ConvertToGraph(QString filepath);
};

#endif // FILEHELPER_H
