#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QtGui>

#include "editor.h"
#include "webapp.h"
#include "file.h"

Editor::Editor(QObject *parent) : QObject(parent)
{
    sidebarHidden = false;
    createActions();

    visible = false;
}

void Editor::createActions()
{
    saveAction = createAction(tr("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save current file."));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveCurrentFile()));

    saveAsAction = createAction(tr("Save &As"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save file with different name."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveCurrentFileAs()));

    closeFileAction = createAction(tr("&Close File"));
    closeFileAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    closeFileAction->setStatusTip(tr("Close current open file."));
    connect(closeFileAction, SIGNAL(triggered()), this, SLOT(closeCurrentFile()));

    closeAllFilesAction = createAction(tr("Close All Files"));
    closeAllFilesAction->setStatusTip(tr("Close all open files."));
    connect(closeAllFilesAction, SIGNAL(triggered()), this, SLOT(closeAllFiles()));

    nextFileStackAction = createAction(tr("Next File in Stack"));
    nextFileStackAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    connect(nextFileStackAction, SIGNAL(triggered()), this, SLOT(cycleNextFile()));

    prevFileStackAction = createAction(tr("Previous File in Stack"));
    prevFileStackAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));
    connect(prevFileStackAction, SIGNAL(triggered()), this, SLOT(cyclePrevFile()));

    syntaxActions = new QActionGroup(parent());
    QList<FileType*>* fileTypes = FileTypes.getTypes();
    for(int i=0; i<fileTypes->size(); i++){
        FileType *type = fileTypes->at(i);
        QAction *a = createAction(type->typeName, syntaxActions);
        type->action = a;
        a->setData(type->mimeName);
        if(type == FileTypes.getDefaultType())
            a->setChecked(true);
    }

    themeActions = new QActionGroup(parent());
    createAction("Default", themeActions, true)->setChecked(true);
    createAction("", themeActions, true);
    createAction("Cobalt", themeActions, true);
    createAction("Eclipse", themeActions, true);
    createAction("Elegant", themeActions, true);
    createAction("Monokai", themeActions, true);
    createAction("Neat", themeActions, true);
    createAction("Night", themeActions, true);

    connect(syntaxActions, SIGNAL(triggered(QAction*)), this, SLOT(changeSyntaxMode(QAction*)));
    connect(themeActions, SIGNAL(triggered(QAction*)), this, SLOT(changeTheme(QAction*)));
}

void Editor::changeSyntaxMode(QAction *syntaxAction)
{
    QString modeName = syntaxAction->text();
    FileType *mode = FileTypes.getTypeByName(modeName);
    m_currFile->setMode(mode);
}

void Editor::changeTheme(QAction *themeAction)
{
    wApp.eval(tr("editor.changeTheme('%1')").arg(themeAction->text().toLower()));
}

QAction* Editor::createAction(const QString &text, QObject *parent, bool enabled)
{
    QAction *a;
    if(!parent) parent = this->parent();
    if(text.isEmpty()){
        a = new QAction(parent);
        a->setSeparator(true);
    }
    else{
        a = new QAction(text, parent);
    }
    if(a->actionGroup() == 0){
        actions.append(a);
        a->setEnabled(enabled);
        connect(this, SIGNAL(hasOpenFile(bool)), a, SLOT(setEnabled(bool)));
    }
    else{
        a->setCheckable(true);
        actions.append(a->actionGroup());
        a->actionGroup()->setEnabled(enabled);
        connect(this, SIGNAL(hasOpenFile(bool)), a->actionGroup(), SLOT(setEnabled(bool)));
    }
    return a;
}

void Editor::show()
{
    visible = true;
    /*
     * Show JS Implementation goes here...
     */
}

void Editor::hide()
{
    visible = false;
    /*
     * Hide JS Implementation goes here...
     */
}

bool Editor::isVisible()
{
    return visible;
}

void Editor::openFile(QFile &file)
{
    open(new File(&file));
}

void Editor::newFile()
{
    open(new File());
}

void Editor::open(File *file)
{
    m_files.push_front(file);
    wApp.addObject("file", file);
    emit fileOpened();
    m_currFile = file;
    m_currFileIterator = m_files.begin();
    QWidget* parentWindow = qobject_cast<QWidget*>(this->parent());
    connect(file, SIGNAL(wasModified(bool)), parentWindow, SLOT(setWindowModified(bool)));
    connect(file, SIGNAL(nameChanged()), this, SLOT(changeCurrent()));
    connect(file, SIGNAL(isCurrent(bool)), this, SLOT(changeCurrent(bool)));
    connect(file, SIGNAL(closing()), this, SLOT(closeFile()));
    file->setCurrent();
    if(m_files.size() == 1) emit hasOpenFile(true);
}

void Editor::changeCurrent(bool s)
{
    File *currFile = qobject_cast<File *>(sender());
    if(m_currFile != currFile){
        m_currFile = currFile;
        if(*m_currFileIterator != m_currFile){
            m_currFileIterator = m_files.begin();
            while(*m_currFileIterator != m_currFile)
                ++m_currFileIterator;
        }

        if(s){ // If switch current file tab order
            m_files.erase(m_currFileIterator);
            m_files.push_front(m_currFile);
            m_currFileIterator = m_files.begin();
        }
        m_currFile->getMode()->action->setChecked(true);
    }
    emit currFileChanged(m_currFile);
}

bool Editor::saveFile(File *file, QString fileName)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    file->setSave();
    if(fileName.isNull()) fileName = file->getPath();

    QFile qFile(fileName);
    if (!qFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(qobject_cast<QWidget*>(this->parent()), tr("Collev"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(qFile.errorString()));
        return false;
    }

    QTextStream out(&qFile);
    out << file->getContent();

    file->setFile(&qFile);

    QApplication::restoreOverrideCursor();
    return true;
}

bool Editor::saveCurrentFile()
{
    if(m_currFile->isNew())
        return saveCurrentFileAs();
    return saveFile(m_currFile);
}

bool Editor::saveCurrentFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(qobject_cast<QWidget*>(this->parent()), tr("Save File As"), "", FileTypes.getFilterString());
    if(fileName.isEmpty())
        return false;
    return saveFile(m_currFile, fileName);
}

bool Editor::close(File *file)
{
    if(!file)
        file = m_currFile;
    else
        file->setCurrent();
    QString fileName = file->getName();
    if(file->isModified()){
        QMessageBox msgBox(qobject_cast<QWidget*>(this->parent()));
        if(file->isNew())
            fileName = "New File";
        msgBox.setText(tr("%1 has been modified.").arg(fileName));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        switch (msgBox.exec()) {
          case QMessageBox::Save:
            if(!saveFile(file))return false;
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

    m_files.erase(m_currFileIterator);
    file->setClosed();
    if(m_files.size() == 0){
        emit hasOpenFile(false);
        emit hasUndo(false);
        emit hasRedo(false);
    }
    else{
        m_currFileIterator = m_files.begin();
        m_currFile = *m_currFileIterator;
        m_currFile->setCurrent();
    }
    return true;
}

void Editor::closeFile()
{
    close(qobject_cast<File *>(sender()));
}

bool Editor::closeCurrentFile()
{
    return close();
}

void Editor::closeAllFiles()
{
    while(1){
        if(!closeCurrentFile()) break;
        if(!hasOpenFile()) break;
        cycleNextFile();
    }
}

void Editor::cycleNextFile()
{
    ++m_currFileIterator;
    if(m_currFileIterator == m_files.end())
        m_currFileIterator = m_files.begin();
    m_currFile = *m_currFileIterator;
    m_currFile->setCurrent();
}

void Editor::cyclePrevFile()
{
    if(m_currFileIterator == m_files.begin()){
        m_currFileIterator = m_files.end();
    }
    --m_currFileIterator;
    m_currFile = *m_currFileIterator;
    m_currFile->setCurrent();
}

bool Editor::hasOpenFile()
{
    if(m_files.size() == 0) return false;
    else return true;
}

bool Editor::hasOpenFileAndVisible()
{
    return (hasOpenFile() && visible);
}


