#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QVariant>

#include "file.h"
#include "filetypemanager.h"

int File::m_count = 0;

File::File(QFile *file) : QObject(0)
{
    m_index = m_count++;
    setFile(file);
}

File::~File()
{
}

void File::setFile(QFile *file = 0)
{
    setModified(false);
    if(file){
        QFileInfo info(file->fileName());
        setName(info.fileName());
        setPath(info.filePath());
        setSuffix(info.suffix());

        QTextStream in(file);
        m_content = in.readAll();
        m_new = false;
        m_type = FileTypes.getTypeByExtension(m_suffix);
        setMode(m_type);
    }
    else{
        m_name = "untitled";
        m_path = m_suffix = m_content = "";
        m_new = true;
        m_type = m_mode = FileTypes.getDefaultType();
    }
}

int File::getIndex()
{
    return m_index;
}

void File::setName(QString name)
{
    if(m_name.isNull()){
        m_name = name;
    }
    else{
        if(m_name != name){
            m_name = name;
            emit nameChanged();
        }
    }
}

QString& File::getName()
{
    return m_name;
}

void File::setPath(QString path)
{
    m_path = path;
}

QString& File::getPath()
{
    return m_path;
}

QString& File::getSuffix()
{
    return m_suffix;
}

void File::setSuffix(QString suffix)
{
    m_suffix = suffix;
}

bool File::isModified()
{
    return m_modified;
}

void File::setModified(bool modified)
{
    qDebug()<<"Setting Modified... "<<modified;

    if(m_modified != modified){
        m_modified = modified;
        qDebug()<<"Emitting Signal... "<<modified;
        emit wasModified(m_modified);
    }
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

QVariantMap File::getTypeMap()
{
    return m_type->toMap();
}

void File::setMode(FileType *mode)
{
    if(!m_mode){
        m_mode = mode;
    }
    else{
        if(m_mode != mode){
            m_mode = mode;
            emit modeChanged();
        }
    }
}

FileType* File::getMode()
{
    return m_mode;
}

QVariantMap File::getModeMap()
{
    return m_mode->toMap();
}

QString& File::getContent()
{
    return m_content;
}

bool File::isNew()
{
    return m_new;
}

void File::setCurrent()
{
    emit isCurrent();
}

void File::setClosed()
{
    emit closed();
    delete this;
}

void File::setSave()
{
    emit save();
}
