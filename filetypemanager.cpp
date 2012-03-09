#include "filetypemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QVariant>

QVariantMap FileType::toMap()
{
    QVariantMap typeMap;
    typeMap["typeName"] = typeName;
    typeMap["mimeName"] = mimeName;
    typeMap["extensions"] = extensions;
    return typeMap;
}

FileTypeManager *FileTypeManager::m_instance = NULL;

FileTypeManager::FileTypeManager(QObject *parent) : QObject(parent)
{
    loadTypes(); // Load and initialize all available file types
}

void FileTypeManager::loadTypes()
{
    QFile mapFile(":/file.types");
    FileType *type;
    m_filter = "All Files (*.*);;";

    if (!mapFile.open(QIODevice::ReadOnly)) return;

    while(!mapFile.atEnd()){
        QTextStream line(mapFile.readLine());
        type = new FileType;
        line >> type->typeName;
        line >> type->mimeName;

        if(type->typeName.isEmpty()) continue; //Invalid Type, continue
        type->typeName.replace('.', ' '); //Replace all occurences of the dot(.), in the type name, with spaces

        QStringList exts;
        QString extString;
        while(!line.atEnd()){
            QString ext;
            line >> ext;
            if(!ext.isEmpty()){
                exts << ext;
                extString += tr("*.%1 ").arg(ext);
            }
        }
        extString.chop(1); // Remove the leading space from the extension string
        type->extensions = exts;

        m_filter += tr("%1 (%2);;").arg(type->typeName).arg(extString);

        m_types.append(type);
        if(type->mimeName == "text/plain")
            m_defaultType = type;
    }
}


QList<FileType*>* FileTypeManager::getTypes()
{
    return &m_types;
}

int FileTypeManager::getTypeCount()
{
    return m_types.size();
}

FileType* FileTypeManager::getDefaultType()
{
    return m_defaultType;
}

FileType* FileTypeManager::getTypeByName(const QString &typeName)
{
    for(int i=0; i<m_types.size(); i++){
        if(typeName == m_types.at(i)->typeName)
            return m_types.at(i);
    }
    return m_defaultType;
}

FileType* FileTypeManager::getTypeByExtension(QString extension)
{
    for(int i=0; i<m_types.size(); i++){
        if(m_types.at(i)->extensions.contains(extension))
            return m_types.at(i);
    }
    return m_defaultType;
}

FileType* FileTypeManager::getTypeByFileName(const QString &fileName)
{
    QFileInfo info(fileName);
    return getTypeByExtension(info.suffix());
}

QString& FileTypeManager::getFilterString()
{
    return m_filter;
}
