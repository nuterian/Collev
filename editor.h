#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QVariant>

typedef QList<QVariantMap*> QVMapList;

class QFile;

class Editor : public QObject
{
    Q_OBJECT

public:
    Editor(QObject *parent = 0);

    void openFile(QFile &file);
    void newFile();
    void retrieveFile(int index);
    void saveFile(int index);
    Q_INVOKABLE void closeFile(int index);

    Q_INVOKABLE int getFileCount();
    Q_INVOKABLE QVariant getFileAttr(int index , const QString &key);
    Q_INVOKABLE QVariant getCurrentFileAttr(const QString &key);
    Q_INVOKABLE void setFileAttr(int index, const QString &key, const QVariant &value);
    Q_INVOKABLE void setCurrentFileAttr(const QString &key, const QVariant &value);

    Q_INVOKABLE void changeCurrent(int index);
    Q_INVOKABLE void switchCurrent(int index);
    void changeCurrent(QVariantMap*);
    Q_INVOKABLE void setFileModified();
    Q_INVOKABLE void saveFileContents(int index, QString content);

    void cycleNextFile();
    void cyclePrevFile();
    void switchPrevFile();

private:
    void sendMsgJS(QString msg);
    void switchCurrTab();

signals:
    bool hasOpenFile(bool);
    bool hasUndo(bool);
    bool hasRedo(bool);

    void fileOpened(QVariantMap file);
    void fileSave(int index);
    void fileSaved(int index);
    void fileTitleChanged(int index, QString title);
    void fileModified();
    void currentChanged(int index);
    void jsMsg(QString msg);

private:
    QVariantMap *currentOpenFile;
    QVMapList openFiles;
    QVMapList tabOrder;
    QVMapList::iterator currentTab;

};

#endif // EDITOR_H
