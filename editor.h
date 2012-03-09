#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>
#include <QVariant>


class QFile;
class QActionGroup;
class QAction;
class QMenu;
class File;

typedef QList<QVariantMap*> QVMapList;
class Editor : public QObject
{
    Q_OBJECT

public:
    Editor(QObject *parent = 0);

    void openFile(QFile &file);
    Q_INVOKABLE void newFile();

    Q_INVOKABLE bool isSidebarHidden();
    void setSidebarHidden(bool);
    bool hasOpenFile();
    bool hasOpenFileAndVisible();

    void show();
    void hide();
    bool isVisible();

    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeFileAction;
    QAction *closeAllFilesAction;

    QAction *nextFileStackAction;
    QAction *prevFileStackAction;
    QActionGroup *syntaxActions;
    QActionGroup *themeActions;
    QList<QObject*> actions;

public slots:
    void cycleNextFile();
    void cyclePrevFile();

private slots:
    void changeSyntaxMode(QAction* syntaxAction);
    void changeTheme(QAction* themeAction);
    void changeCurrent(bool s = false);
    bool saveCurrentFile();
    bool saveCurrentFileAs();
    void closeFile();
    bool closeCurrentFile();
    void closeAllFiles();


private:
    bool saveFile(File* file, QString fileName = QString());
    bool close(File* file = 0);
    void createActions();
    QAction* createAction( const QString & text, QObject * parent = 0, bool enabled = false);

signals:
    bool hasOpenFile(bool);
    bool hasUndo(bool);
    bool hasRedo(bool);

    void fileOpened();
    void currFileChanged(File*);

private:

    void open(File*);

    File *m_currFile;
    QList<File*>::iterator m_currFileIterator;
    QList<File*> m_files;

    bool sidebarHidden;
    bool visible;
};

#endif // EDITOR_H
