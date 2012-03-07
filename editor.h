#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QVariant>


class QFile;
class QActionGroup;
class QAction;
class QMenu;

typedef QList<QVariantMap*> QVMapList;
class Editor : public QObject
{
    Q_OBJECT

public:
    Editor(QObject *parent = 0);

    void openFile(QFile &file);
    void newFile();
    void retrieveFile(int index);
    void saveFile(int index);
    void closeFile(QVariantMap *filemap = NULL);
    Q_INVOKABLE void closeFile(int index);

    Q_INVOKABLE int getFileCount();
    Q_INVOKABLE QVariant getFileAttr(int index , const QString &key);
    Q_INVOKABLE QVariant getCurrentFileAttr(const QString &key);
    int getCurrentFileIndex();
    Q_INVOKABLE void setFileAttr(int index, const QString &key, const QVariant &value);
    Q_INVOKABLE void setCurrentFileAttr(const QString &key, const QVariant &value);

    void changeCurrent(QVariantMap*);
    Q_INVOKABLE void changeCurrent(int index);
    Q_INVOKABLE void switchCurrent(int index);
    Q_INVOKABLE void setFileModified();
    Q_INVOKABLE void saveFileContents(int index, QString content);

    Q_INVOKABLE bool isSidebarHidden();
    void setSidebarHidden(bool);
    bool hasOpenFile();
    bool hasOpenFileAndVisible();

    QVariantMap* getDefaultFileType();
    QVariantMap* getFileTypeByName(QString& fileName);
    QVariantMap* getFileTypeByExt(QString ext);
    QVariantMap* getFileTypeByMode(QString mode);
    QVMapList getFileTypes();
    QString* getFileDialogString();

    void show();
    void hide();
    bool isVisible();

    QAction *nextFileStackAction;
    QAction *prevFileStackAction;
    QActionGroup *syntaxActions;
    QActionGroup *themeActions;
    QList<QObject*> actions;

public slots:
    int cycleNextFile();
    int cyclePrevFile();

private slots:
    void changeSyntaxMode(QAction* syntaxAction);
    void changeTheme(QAction* themeAction);

private:
    void createActions();
    void sendMsgJS(QString msg);
    void switchCurrTab();
    void loadFileTypes();
    QAction* createAction( const QString & text, QObject * parent );

signals:
    bool hasOpenFile(bool);
    bool hasUndo(bool);
    bool hasRedo(bool);

    void fileOpened(QVariantMap file);
    void fileSave(int index);
    void fileSaved(int index);
    void fileTitleChanged(int index, QVariantMap file);
    void fileTypeChanged(int index, QVariantMap type);
    void fileModified();
    void fileClose(int index);
    void currentChanged(int index);
    void jsMsg(QString msg);

private:
    QVariantMap *currentOpenFile;
    QVMapList openFiles;
    QVMapList tabOrder;
    QVMapList::iterator currentTab;

    QVMapList fileTypes;
    QVariantMap *defaultType;
    QString fileDialogString;
    bool sidebarHidden;
    bool visible;
};

#endif // EDITOR_H
