#include <QtCore/QUrl>
#include <QtGui>
#include <QWebView>
#include <QUndoStack>

#include <QDebug>

#include "mainwindow.h"
#include "webapp.h"
#include "editor.h"
#include "file.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Read and Intialize Application Settings
    readSettings();

    // Initialize Data Members
    view = new QWebView(this);
    qEditor = new Editor(this);
    fileDialogString = &FileTypes.getFilterString();
    frame = wApp.mainFrame();

    attachObjects();
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObjects()));
    connect(qEditor, SIGNAL(currFileChanged(File*)), this, SLOT(updateCurrentFile(File*)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), this, SLOT(setEmpty(bool)));

    wApp.loadFile("qrc:/index.html");
    view->setPage(&wApp);
    wApp.setParent(view);
    setCentralWidget(view);

    createActions();
    createMenus();
    this->setStyleSheet(" QMenuBar {padding:4px 6px;border-bottom:1px solid #444;background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e6e6e6, stop:1 #cfcfcf);}"
                        " QMenuBar::item {spacing: 6px; padding:4px 6px;margin:2px 0;background: transparent;border-radius: 2px;}"
                        " QMenuBar::item:selected {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e6e6e6, stop:1 #cfcfcf);border:1px solid #999;}"
                        " QMenuBar::item:pressed {color:#fff;background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6e6e6e, stop:1 #4e4e4e);border-color:#222;}");

    view->setAcceptDrops(false);
    setAcceptDrops(true);
}

MainWindow::~MainWindow(){
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> files = event->mimeData()->urls();
    for(int i=0; i<files.size(); i++){
        QString fileName = files.at(i).toLocalFile();
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::critical(this, tr("Error"), tr("Could not open file at path: %1").arg(fileName));
            return;
        }
        qEditor->openFile(file);
        file.close();
    }
    event->acceptProposedAction();
}

void MainWindow::attachObjects()
{
    wApp.addObject(QString("qEditor"), qEditor);
}


void MainWindow::readSettings()
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    QSettings settings("Nuterian", "Collev");
    settings.beginGroup("MainWindow");
    if(settings.contains("geometry")){
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    else{
        resize(QSize(750, 500));
        QRect frect = frameGeometry();
        frect.moveCenter(QDesktopWidget().availableGeometry().center());
        move(frect.topLeft());
    }
    setMinimumSize(QSize(600, 400));
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings("Nuterian", "Collev");
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.setValue("showSidebar", wApp.isSidebarVisible());
}

void MainWindow::setSidebarVisible(bool visible)
{
    isSidebarVisible = visible;
    wApp.eval(tr("showSidebar(%1)").arg(isSidebarVisible));
}

bool MainWindow::confirmQuit()
{
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (confirmQuit()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::createActions()
{
    QSettings settings("Nuterian", "Collev");
    newFileAction = new QAction(tr("&New"),this);
    newFileAction->setShortcut(QKeySequence::New);
    newFileAction->setStatusTip(tr("Create a new file."));
    connect(newFileAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openFileAction = new QAction(tr("&Open"),this);
    openFileAction->setShortcut(QKeySequence::Open);
    openFileAction->setStatusTip(tr("Open an existing file."));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));

    exitAction = new QAction(tr("E&xit"),this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit to Windows."));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    undoAction = new QAction(tr("&Undo"),this);
    //undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo last action."));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

    redoAction = new QAction(tr("&Redo"),this);
    //redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setStatusTip(tr("Redo last action."));
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));

    cutAction = new QAction(tr("Cu&t"),this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Move selected text to clipboard."));
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    copyAction = new QAction(tr("&Copy"),this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy selected text to clipboard."));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAction = new QAction(tr("&Paste"),this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Paste text from clipboard."));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));


    toggleSidebarAction= new QAction(tr("Hide Sidebar"),this);
    connect(toggleSidebarAction, SIGNAL(triggered()), this, SLOT(toggleSidebar()));
    wApp.setSidebarVisible(settings.value("showSidebar", true).toBool());
    if(!wApp.isSidebarVisible())
        toggleSidebarAction->setText("Show Sidebar");

    toggleConsoleAction= new QAction(tr("Show Console"),this);
    toggleConsoleAction->setShortcut(QKeySequence(tr("Ctrl+`")));
    connect(toggleConsoleAction, SIGNAL(triggered()), this, SLOT(toggleConsole()));
    toggleConsoleAction->setData(false);
    toggleFullScreenAction= new QAction(tr("Enter Full Screen"),this);
    toggleFullScreenAction->setShortcut(QKeySequence(Qt::Key_F11));
    connect(toggleFullScreenAction, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));

    undoAction->setEnabled(false);
    redoAction->setEnabled(false);

    cutAction->setEnabled(false);
    copyAction->setEnabled(false);
    pasteAction->setEnabled(false);

    connect(qEditor, SIGNAL(hasUndo(bool)), undoAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasRedo(bool)), redoAction, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newFileAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(qEditor->saveAction);
    fileMenu->addAction(qEditor->saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(qEditor->closeFileAction);
    fileMenu->addAction(qEditor->closeAllFilesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(toggleSidebarAction);
    viewMenu->addAction(toggleConsoleAction);
    viewMenu->addSeparator();
    viewMenu->addAction(toggleFullScreenAction);
    viewMenu->addSeparator();
    syntaxMenu = viewMenu->addMenu(tr("&Syntax"));
    syntaxMenu->addActions(qEditor->syntaxActions->actions());


    gotoMenu = menuBar()->addMenu(tr("&Goto"));
    gotoMenu->addAction(qEditor->nextFileStackAction);
    gotoMenu->addAction(qEditor->prevFileStackAction);

    prefMenu = menuBar()->addMenu(tr("&Preferences"));
    themeMenu = prefMenu->addMenu(tr("&Theme"));

    themeMenu->addActions(qEditor->themeActions->actions());
}

///////////////////////////////////////////////////////////////////////////
//  SLOTS
///////////////////////////////////////////////////////////////////////////

void MainWindow::newFile()
{
    qEditor->newFile();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", *fileDialogString);

    if(fileName != ""){
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        qEditor->openFile(file);
        file.close();
    }
}

void MainWindow::undo()
{
    wApp.undo();
}

void MainWindow::redo()
{
    wApp.redo();
}


void MainWindow::cut()
{
    view->page()->triggerAction(QWebPage::Cut);
}


void MainWindow::copy()
{
    view->page()->triggerAction(QWebPage::Copy);
}

void MainWindow::paste()
{
    view->page()->triggerAction(QWebPage::Paste);
}

void MainWindow::nextFile()
{
    qEditor->cycleNextFile();
}

void MainWindow::prevFile()
{
    qEditor->cyclePrevFile();
}

void MainWindow::toggleSidebar()
{
    wApp.setSidebarVisible(!wApp.isSidebarVisible());
    if(wApp.isSidebarVisible())
        toggleSidebarAction->setText("Hide Sidebar");
    else
       toggleSidebarAction->setText("Show Sidebar");
}

void MainWindow::toggleConsole()
{
    bool show = !toggleConsoleAction->data().toBool();
    frame->evaluateJavaScript(tr("showConsole(%1)").arg(show));
    toggleConsoleAction->setData(show);
    if(show)
        toggleConsoleAction->setText("Hide Console");
    else
       toggleConsoleAction->setText("Show Console");
}

void MainWindow::toggleFullScreen()
{
    if(isFullScreen()){
        showNormal();
        toggleFullScreenAction->setText("Enter Full Screen");
    }
    else{
        showFullScreen();
        toggleFullScreenAction->setText("Exit Full Screen");
    }
}

void MainWindow::setEmpty(bool status)
{
    if(status == false){
        this->setWindowTitle(tr("Collev"));
        setWindowModified(false);
    }
}

void MainWindow::updateCurrentFile(File *file)
{
    this->setWindowTitle(tr("%1[*] - Collev").arg(file->getName()));
    setWindowModified(file->isModified());
}

void MainWindow::fileWasModified()
{
    setWindowModified(true);
}

void MainWindow::setTitle(const QString &title)
{
    this->setWindowTitle(tr("%1[*] - Collev").arg(title));
}

