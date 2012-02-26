#include "file.h"

#include <QFileInfo>
#include <QTextStream>

File::File(QObject *parent) :QVariantMap(parent)
{
}

File::File(QFile &file, QObject *parent = 0) :QVariantMap(parent)
{
    QFileInfo info(file.fileName());
    this->insert("name", info.fileName());
    this->insert("path", info.filePath());
    this->insert("ext", info.suffix());
    QTextStream in(&file);
    this->insert("content", in.readAll());
    this->insert("content", in.readAll());
}
