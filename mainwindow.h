#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "editor.h"

class QAction;
class QMenu;
class QWebView;
class QWebFrame;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateCurrentFile(int index);
    void fileWasModified();
    void attachObjects();

    void newFile();
    void openFile();
    bool save();
    bool saveAs();
    void exit();

    void undo();
    void redo();
    void cut(;)

private:
    void createActions();
    void createMenus();
    void loadFile(const QString &fileName);
    bool saveFile(int index, const QString &fileName);


private:

    Editor *qEditor;

    QWebView *view;
    QWebFrame *frame;

    QMenu *fileMenu;
    QAction *newFileAction;
    QAction *openFileAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QMenu *editMenu;
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;


};

#endif // MAINWINDOW_H
