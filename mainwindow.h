#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QVarLengthArray>

class QAction;
class QMenu;
class QWebView;
class QWebFrame;

class Editor;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void log(const QString&);

private slots:
    void updateCurrentFile(int index);
    void fileWasModified();
    void attachObjects();

    void newFile();
    void openFile();
    bool save();
    bool saveAs();
    bool closeFile();
    bool closeFile(int index);
    void closeAllFiles();

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
    void changeTheme();
    void changeSyntaxMode();

    void setEmpty(bool);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void createActions();
    void createMenus();
    bool saveFile(int index, const QString &fileName);
    bool confirmQuit();
    void writeSettings();


private:

    Editor *qEditor;

    QWebView *view;
    QWebFrame *frame;

    QString fileDialogString;

    QMenu *fileMenu;
    QAction *newFileAction;
    QAction *openFileAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeFileAction;
    QAction *closeAllFilesAction;
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
    QVarLengthArray<QAction*> syntaxActions;
    QMenu *indentationMenu;

    QMenu *gotoMenu;
    QAction *nextFileStackAction;
    QAction *prevFileStackAction;
    QAction *matchBracketAction;

    QMenu *prefMenu;
    QMenu *themeMenu;
    QAction *themeActions[8];


};

#endif // MAINWINDOW_H
