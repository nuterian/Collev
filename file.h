#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFile>

struct FileType;

class QVariantMap;

class File : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int index READ getIndex)
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString path READ getPath)
    Q_PROPERTY(QString suffix READ getSuffix)
    Q_PROPERTY(QString content READ getContent WRITE setContent)
    Q_PROPERTY(QVariantMap type READ getTypeMap)
    Q_PROPERTY(bool isModified READ isModified WRITE setModified NOTIFY wasModified)
    Q_PROPERTY(bool isNew READ isNew)

public:
    explicit File(QFile *file);
    ~File();

    int getIndex();
    QString& getName();
    QString& getPath();
    QString& getSuffix();

    void setContent(QString&);
    QString& getContent();

    FileType* getType();
    void setType(FileType*);
    QVariantMap& getTypeMap();

    bool isModified();
    void setModified(bool);
    bool isNew();
    
signals:
    void wasModified(bool);
    
private:
    int m_index;
    QString m_name;
    QString m_path;
    QString m_suffix;
    QString m_content;
    FileType *m_type;
    bool m_modified;
    bool m_new;

    static int m_count;
    
};

#endif // FILE_H
