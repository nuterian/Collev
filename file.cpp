#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QVariant>

#include "file.h"
#include "filetypemanager.h"

int File::m_count = 0;

File::File(QFile *file = 0) : QObject(0)
{
    m_index = m_count++;
    m_modified = false;
    if(file){
        QFileInfo info(file->fileName());
        m_name = info.fileName();
        m_path = info.filePath();
        m_suffix = info.suffix();

        QTextStream in(file);
        m_content = in.readAll();
        m_new = false;
        m_type = FileTypes.getTypeByFileName(m_name);
    }
    else{
        m_name = "untitled";
        m_path = m_suffix = m_content = "";
        m_new = true;
        m_type = FileTypes.getDefaultType();
    }
}

File::~File()
{
}

int File::getIndex()
{
    return m_index;
}

QString& File::getName()
{
    return m_name;
}

QString& File::getPath()
{
    return m_path;
}

QString& File::getSuffix()
{
    return m_suffix;
}

bool File::isModified()
{
    return m_modified;
}

void File::setModified(bool modified)
{
    m_modified = modified;
    emit wasModified(m_modified);
}

void File::setContent(QString &content)
{
    m_content = content;
}

FileType* File::getType()
{
    return m_type;
}

void File::setType(FileType *type)
{
    m_type = type;
}

QVariantMap& File::getTypeMap()
{
    return m_type->toMap();
}

QString& File::getContent()
{
    return m_content;
}

bool File::isNew()
{
    return m_new;
}
