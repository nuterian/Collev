#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QVarLengthArray>

#include "client.h"

class QAction;
class QMenu;
class QWebView;
class QWebFrame;

class Editor;
class File;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setTitle(const QString &);

private slots:
    void updateCurrentFile(File*);
    void fileWasModified();
    void attachObjects();

    void newFile();
    void openFile();

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    void nextFile();
    void prevFile();

    void toggleSidebar();
    void toggleConsole();
    void toggleFullScreen();

    void setEmpty(bool);
    //void onLoad(bool);

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void createActions();
    void createMenus();
    bool confirmQuit();
    void readSettings();
    void writeSettings();

    void setSidebarVisible(bool);


private:

    Editor *qEditor;
    Client client;

    QWebView *view;
    QWebFrame *frame;

    QString *fileDialogString;

    QMenu *fileMenu;
    QAction *newFileAction;
    QAction *openFileAction;
    QAction *exitAction;

    QMenu *editMenu;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;

    QMenu *viewMenu;
    QAction *toggleSidebarAction;
    QAction *toggleConsoleAction;
    QAction *toggleFullScreenAction;
    QMenu *syntaxMenu;

    QMenu *gotoMenu;
    QAction *nextFileStackAction;
    QAction *prevFileStackAction;
    QAction *matchBracketAction;

    QMenu *prefMenu;
    QMenu *themeMenu;

    bool isSidebarVisible;
};

#endif // MAINWINDOW_H
