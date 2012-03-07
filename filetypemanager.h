#ifndef FILETYPEMANAGER_H
#define FILETYPEMANAGER_H

#include <QMetaType>
#include <QObject>

#define FileTypes FileTypeManager::getInstance()

class QStringList;
class QList;
class QVariantMap;

struct FileType
{
    QString typeName;
    QString mimeName;
    QStringList extensions;

    QVariantMap& toMap();
};

Q_DECLARE_METATYPE(FileType)


class FileTypeManager
{
    Q_OBJECT

    Q_PROPERTY(QString filterString READ getFilterString)
    Q_PROPERTY(FileType typeCount READ getTypeCount)
    Q_PROPERTY(FileType defaultType READ getDefaultType)

public:
    static FileTypeManager& getInstance()
    {
        if(!m_instance) m_instance = new FileTypeManager();
        return *m_instance;
    }

    QList<FileType*>* getTypes();
    int getTypeCount();
    FileType* getDefaultType();
    FileType* getTypeByName(const QString& typeName);
    FileType* getTypeByFileName(const QString& fileName);
    FileType* getTypeByExtension(QString extension);
    QString& getFilterString();

protected:
    FileTypeManager();
    static FileTypeManager *m_instance;

private:
    void loadTypes();

    QList<FileType*> m_types;
    FileType* m_defaultType;
    QString m_filter; // String used as an extension filter in dialogs
};

#endif // FILETYPEMANAGER_H
