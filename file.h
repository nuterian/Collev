#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFile>

#include "filetypemanager.h"


class File : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int index READ getIndex)
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString path READ getPath)
    Q_PROPERTY(QString suffix READ getSuffix)
    Q_PROPERTY(QString content READ getContent WRITE setContent)
    Q_PROPERTY(QVariantMap type READ getTypeMap)
    Q_PROPERTY(QVariantMap mode READ getModeMap)
    Q_PROPERTY(bool isModified READ isModified WRITE setModified NOTIFY wasModified)
    Q_PROPERTY(bool isNew READ isNew)

public:
    explicit File(QFile *file = 0);
    ~File();

    void setFile(QFile*);

    int getIndex();
    QString& getName();
    QString& getPath();
    QString& getSuffix();

    void setContent(QString&);
    QString& getContent();

    void setType(FileType*);
    FileType* getType();
    QVariantMap getTypeMap();

    void setMode(FileType*);
    FileType* getMode();
    QVariantMap getModeMap();

    bool isModified();
    Q_INVOKABLE void setModified(bool);
    bool isNew();
    void setNew(bool);

    void setCurrent();
    void setClosed();
    void setSave();
    
signals:
    void wasModified(bool);
    void isCurrent(bool s = false);
    void closing();
    void closed();
    void save();
    void modeChanged();
    void nameChanged();
    void typeChanged(FileType*);
    
private:

    void setName(QString);
    void setPath(QString);
    void setSuffix(QString);

    int m_index;
    QString m_name;
    QString m_path;
    QString m_suffix;
    QString m_content;
    FileType *m_type;
    FileType *m_mode;
    bool m_modified;
    bool m_new;

    static int m_count;
    
};

#endif // FILE_H
