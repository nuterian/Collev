#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QtCore/QUrl>
#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    view = new QWebView(this);

    qEditor = new Editor(this);
    frame = view->page()->mainFrame();
    attachObjects();
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObjects()));
    connect(qEditor, SIGNAL(currentChanged(int)), this, SLOT(updateCurrentFile(int)));
    connect(qEditor, SIGNAL(fileModified()), this, SLOT(fileWasModified()));

    loadFile("qrc:/index.html");
    setCentralWidget(view);

    createActions();
    createMenus();
}

MainWindow::~MainWindow()
{

}

void MainWindow::attachObjects()
{
    frame->addToJavaScriptWindowObject(QString("qEditor"), qEditor);
}

void MainWindow::loadFile(const QString &fileName)
{
    frame->setUrl(QUrl(fileName));
}

void MainWindow::createActions()
{
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

    exitAction = new QAction(tr("E&xit"),this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit to Windows."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(close()));

    undoAction = new QAction(tr("&Undo"),this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo last action."));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

    redoAction = new QAction(tr("&Redo"),this);
    redoAction->setShortcut(QKeySequence::Undo);
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

    saveAction->setEnabled(false);
    saveAsAction->setEnabled(false);
    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);
    pasteAction->setEnabled(false);

    connect(qEditor, SIGNAL(hasOpenFile(bool)), saveAction, SLOT(setEnabled(bool)));
    connect(qEditor, SIGNAL(hasOpenFile(bool)), saveAsAction, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newFileAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
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

void MainWindow::undo()
{

}

void MainWindow::redo()
{

}


void MainWindow::cut()
{

}


void MainWindow::copy()
{

}


void MainWindow::paste()
{

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

