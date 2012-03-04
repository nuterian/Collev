#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QtCore/QUrl>
#include <QtGui>

#include "mainwindow.h"
#include "editor.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    view = new QWebView(this);

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
    qEditor = new Editor(this);
    frame = view->page()->mainFrame();
    attachObjects();
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObjects()));
    connect(qEditor, SIGNAL(currentChanged(int)), this, SLOT(updateCurrentFile(int)));
    connect(qEditor, SIGNAL(fileModified()), this, SLOT(fileWasModified()));
    connect(qEditor, SIGNAL(fileClose(int)), this, SLOT(closeFile(int)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), this, SLOT(setEmpty(bool)));

    loadFile("qrc:/index.html");
    setCentralWidget(view);

    createActions();
    createMenus();
   // menuBar()->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #2198c0, stop: 1 #0d5ca6)");
    this->setStyleSheet(" QMenuBar {padding:4px 6px;border-bottom:1px solid #444;background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e6e6e6, stop:1 #cfcfcf);}"
                        " QMenuBar::item {spacing: 6px; padding:4px 6px;margin:2px 0;background: transparent;border-radius: 2px;}"
                        " QMenuBar::item:selected {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e6e6e6, stop:1 #cfcfcf);border:1px solid #999;}"
                        " QMenuBar::item:pressed {color:#fff;background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6e6e6e, stop:1 #4e4e4e);border-color:#222;}");
}

MainWindow::~MainWindow()
{

}

void MainWindow::attachObjects()
{
    frame->addToJavaScriptWindowObject(QString("qEditor"), qEditor);
}

void MainWindow::log(const QString &text)
{
    frame->evaluateJavaScript(tr("log('%1')").arg(text));
}

void MainWindow::loadFile(const QString &fileName)
{
    frame->setUrl(QUrl(fileName));
}

void MainWindow::writeSettings()
{
    QSettings settings("Nuterian", "Collev");
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.setValue("Editor/showSidebar", toggleSidebarAction->data().toBool());
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

    saveAction = new QAction(tr("&Save"),this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save current file."));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Save &As"),this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save file with different name."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    closeFileAction = new QAction(tr("&Close File"),this);
    closeFileAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    closeFileAction->setStatusTip(tr("Close current open file."));
    connect(closeFileAction, SIGNAL(triggered()), this, SLOT(closeFile()));

    closeAllFilesAction = new QAction(tr("Close All Files"),this);
    closeAllFilesAction->setStatusTip(tr("Close all open files."));
    connect(closeAllFilesAction, SIGNAL(triggered()), this, SLOT(closeAllFiles()));

    exitAction = new QAction(tr("E&xit"),this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit to Windows."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(close()));

    undoAction = new QAction(tr("&Undo"),this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo last action."));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

    redoAction = new QAction(tr("&Redo"),this);
    redoAction->setShortcut(QKeySequence::Redo);
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

    nextFileStackAction = new QAction(tr("Next File in Stack"),this);
    nextFileStackAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    connect(nextFileStackAction, SIGNAL(triggered()), this, SLOT(nextFile()));

    prevFileStackAction = new QAction(tr("Previous File in Stack"),this);
    prevFileStackAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));
    connect(prevFileStackAction, SIGNAL(triggered()), this, SLOT(prevFile()));

    toggleSidebarAction= new QAction(tr("Hide Sidebar"),this);
    connect(toggleSidebarAction, SIGNAL(triggered()), this, SLOT(toggleSidebar()));
    bool sidebarShown = settings.value("Editor/showSidebar", true).toBool();
    qEditor->setSidebarHidden(!sidebarShown);
    if(!sidebarShown)
        toggleSidebarAction->setText("Show Sidebar");

    toggleConsoleAction= new QAction(tr("Show Console"),this);
    toggleConsoleAction->setShortcut(QKeySequence(tr("Ctrl+`")));
    connect(toggleConsoleAction, SIGNAL(triggered()), this, SLOT(toggleConsole()));
    toggleConsoleAction->setData(false);
    toggleFullScreenAction= new QAction(tr("Enter Full Screen"),this);
    toggleFullScreenAction->setShortcut(QKeySequence(Qt::Key_F11));
    connect(toggleFullScreenAction, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));

    saveAction->setEnabled(false);
    saveAsAction->setEnabled(false);
    closeFileAction->setEnabled(false);
    closeAllFilesAction->setEnabled(false);
    undoAction->setEnabled(false);
    redoAction->setEnabled(false);

    cutAction->setEnabled(false);
    copyAction->setEnabled(false);
    pasteAction->setEnabled(false);

    nextFileStackAction->setEnabled(false);
    prevFileStackAction->setEnabled(false);

    connect(qEditor, SIGNAL(hasOpenFile(bool)), saveAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), saveAsAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), closeFileAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), closeAllFilesAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), nextFileStackAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), prevFileStackAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasUndo(bool)), undoAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasRedo(bool)), redoAction, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newFileAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeFileAction);
    fileMenu->addAction(closeAllFilesAction);
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

    QVMapList fileTypes = qEditor->getFileTypes();
    syntaxActions.resize(fileTypes.size());
    for(int i=0; i<fileTypes.size(); i++){
        syntaxActions[i] = syntaxMenu->addAction(((*fileTypes.at(i))["name"]).toString(), this, SLOT(changeSyntaxMode()));
        syntaxActions[i]->setCheckable(true);
        syntaxActions[i]->setData((*fileTypes.at(i))["mime"]);
    }
    syntaxMenu->setEnabled(false);
    connect(qEditor, SIGNAL(hasOpenFile(bool)), syntaxMenu, SLOT(setEnabled(bool)));

    gotoMenu = menuBar()->addMenu(tr("&Goto"));
    gotoMenu->addAction(nextFileStackAction);
    gotoMenu->addAction(prevFileStackAction);
    //gotoMenu->addAction(matchBracketAction);

    prefMenu = menuBar()->addMenu(tr("&Preferences"));
    themeMenu = prefMenu->addMenu(tr("&Theme"));

    themeActions[0] = themeMenu->addAction("Default", this, SLOT(changeTheme()));
    themeMenu->addSeparator();
    themeActions[1] = themeMenu->addAction("Cobalt",this, SLOT(changeTheme()));
    themeActions[2] = themeMenu->addAction("Eclipse", this, SLOT(changeTheme()));
    themeActions[3] = themeMenu->addAction("Elegant", this, SLOT(changeTheme()));
    themeActions[4] = themeMenu->addAction("Monokai", this, SLOT(changeTheme()));
    themeActions[5] = themeMenu->addAction("Neat", this, SLOT(changeTheme()));
    themeActions[6] = themeMenu->addAction("Night", this, SLOT(changeTheme()));

    for(int i=0; i<7; i++){
        themeActions[i]->setCheckable(true);
    }
    themeActions[0]->setChecked(true);
}

void MainWindow::changeTheme()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action){
        for(int i=0; i< 7; i++){
            if(themeActions[i]->isChecked() && themeActions[i] != action){
                themeActions[i]->setChecked(false);
            }
        }
        action->setChecked(true);
        frame->evaluateJavaScript(tr("editor.changeTheme('%1')").arg(action->text().toLower()));
    }
}

void MainWindow::changeSyntaxMode()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action){
        for(int i=0; i< syntaxActions.size(); i++){
            if(syntaxActions[i]->isChecked() && syntaxActions[i] != action){
                syntaxActions[i]->setChecked(false);
            }
        }
        action->setChecked(true);
        frame->evaluateJavaScript(tr("editor.switchCurrMode('%1','%2')").arg(action->text()).arg(action->data().toString()));
    }
}


bool MainWindow::saveFile(int index, const QString &fileName)
{
    qEditor->retrieveFile(index);
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Collev"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);

    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    #endif
        out << qEditor->getFileAttr(index, "content").toString();
    #ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
    #endif

    qEditor->setFileAttr(index, "modified", false);
    qEditor->setFileAttr(index, "new", false);
    QFileInfo info(file.fileName());
    qEditor->setFileAttr(index, "name", info.fileName());
    qEditor->setFileAttr(index, "path", info.filePath());

    updateCurrentFile(index);
    return true;

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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "All Files (*.*);;C (*.c *.h);;C++ (*.cpp *.cc *.cxx *.c++ *.h *.hpp *.hxx *.h++ *.inl *.ipp *.cp *.C *.hh);;HTML (*.html *.htm  *.shtml *.xhtml *.inc *.tmpl *.tpl *.ctp;;Java(*.java *.bsh);;Javascript(*.js *.htc *.jsx);;PHP (*.php);;Python (*.py *.rpy *.pyw);;SQL (*.sql *.ddl *.dml)");

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

bool MainWindow::save()
{
    if(qEditor->getCurrentFileAttr("new").toBool() == true) // Newly created File, hence needs to be saved as.
        return saveAs();
    return saveFile(qEditor->getCurrentFileAttr("id").toInt(),qEditor->getCurrentFileAttr("path").toString());
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if(fileName.isEmpty())
        return false;
    return saveFile(qEditor->getCurrentFileAttr("id").toInt(), fileName);
}

bool MainWindow::closeFile()
{
    if(qEditor->getCurrentFileAttr("modified").toBool())
    {
        QMessageBox msgBox;
        QString fileName = qEditor->getCurrentFileAttr("name").toString();
        if(qEditor->getCurrentFileAttr("new").toBool())
            fileName = "New File";
        msgBox.setText(tr("%1 has been modified.").arg(fileName));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::Save:
              if(!save())return false;
              break;
          case QMessageBox::Discard:
              break;
          case QMessageBox::Cancel:
              return false;
              break;
          default:
              return false;
              break;
        }
    }
    frame->evaluateJavaScript(tr("editor.close(%1)").arg(qEditor->getCurrentFileIndex()));
    return true;
}

bool MainWindow::closeFile(int index)
{
    qEditor->changeCurrent(index);
    return closeFile();
}

void MainWindow::closeAllFiles()
{
    int index = qEditor->getCurrentFileIndex();
    while(1){
        if(!closeFile(index)) break;
        if(!qEditor->hasOpenFile()) break;
        index = qEditor->cycleNextFile();
    }
}

void MainWindow::undo()
{
    frame->evaluateJavaScript("editor.undo()");
}

void MainWindow::redo()
{
    frame->evaluateJavaScript("editor.redo()");
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
    qEditor->setSidebarHidden(!qEditor->isSidebarHidden());
    frame->evaluateJavaScript(tr("editor.showSidebar(%1)").arg(!qEditor->isSidebarHidden()));
    if(qEditor->isSidebarHidden())
        toggleSidebarAction->setText("Show Sidebar");
    else
       toggleSidebarAction->setText("Hide Sidebar");
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

void MainWindow::updateCurrentFile(int index)
{
    this->setWindowTitle(tr("%1[*] - Collev").arg(qEditor->getCurrentFileAttr("name").toString()));
    setWindowModified(qEditor->getCurrentFileAttr("modified").toBool());
}

void MainWindow::fileWasModified()
{
    setWindowModified(true);
}

