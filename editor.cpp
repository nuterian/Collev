#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QtGui>

#include "editor.h"
#include "webapp.h"
#include "file.h"

Editor::Editor(QObject *parent) : QObject(parent)
{
    //currentOpenFile = NULL;
    sidebarHidden = false;
    defaultType = NULL;
    loadFileTypes();
    createActions();

    visible = false;
    show(); // Let's show the editor by default for now
}

void Editor::createActions()
{
    nextFileStackAction = createAction(tr("Next File in Stack"), parent());
    nextFileStackAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    connect(nextFileStackAction, SIGNAL(triggered()), this, SLOT(cycleNextFile()));

    prevFileStackAction = createAction(tr("Previous File in Stack"), parent());
    prevFileStackAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));
    connect(prevFileStackAction, SIGNAL(triggered()), this, SLOT(cyclePrevFile()));

    syntaxActions = new QActionGroup(parent());
    for(int i=0; i<fileTypes.size(); i++){
        QVariantMap *typeMap = fileTypes.at(i);
        QAction *a = createAction((*typeMap)["name"].toString(), syntaxActions);
        a->setData((*typeMap)["mime"]);
        if(typeMap == defaultType)
            a->setChecked(true);
    }

    themeActions = new QActionGroup(parent());
    createAction("Default", themeActions)->setChecked(true);
    createAction("", themeActions);
    createAction("Cobalt", themeActions);
    createAction("Eclipse", themeActions);
    createAction("Elegant", themeActions);
    createAction("Monokai", themeActions);
    createAction("Neat", themeActions);
    createAction("Night", themeActions);

    connect(syntaxActions, SIGNAL(triggered(QAction*)), this, SLOT(changeSyntaxMode(QAction*)));
    connect(themeActions, SIGNAL(triggered(QAction*)), this, SLOT(changeTheme(QAction*)));
}

void Editor::changeSyntaxMode(QAction *syntaxAction)
{
    wApp.eval(tr("editor.switchCurrMode('%1','%2')").arg(syntaxAction->text()).arg(syntaxAction->data().toString()));
    setCurrentFileAttr("mode", syntaxAction->text());
}

void Editor::changeTheme(QAction *themeAction)
{
    wApp.eval(tr("editor.changeTheme('%1')").arg(themeAction->text().toLower()));
}

QAction* Editor::createAction(const QString &text, QObject *parent)
{
    QAction *a;
    if(text.isEmpty()){
        a = new QAction(parent);
        a->setSeparator(true);
    }
    else{
        a = new QAction(text, parent);
    }
    if(a->actionGroup() == 0){
        actions.append(a);
        a->setEnabled(false);
        connect(this, SIGNAL(hasOpenFile(bool)), a, SLOT(setEnabled(bool)));
    }
    else{
        a->setCheckable(true);
        actions.append(a->actionGroup());
        a->actionGroup()->setEnabled(false);
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
    QVariantMap *filemap;
    filemap = new QVariantMap;
    QFileInfo info(file.fileName());
    (*filemap)["name"] = info.fileName();
    (*filemap)["path"] = info.filePath();
    (*filemap)["type"] = info.suffix();
    QVariantMap *typeMap = getFileTypeByExt((*filemap)["type"].toString());
    (*filemap)["mode"] = (*typeMap)["name"];
    (*filemap)["mime"] = (*typeMap)["mime"];
    // QTextStream in(&file);
    //(*filemap)["content"] = in.readAll();
    int index = openFiles.size();
    (*filemap)["id"] = index;
    (*filemap)["modified"] = false;
    (*filemap)["new"] = false;
    openFiles.append(filemap);

    File *f = new File(&file);
    files.append(f);

    wApp.addObject("file", f);
    emit fOpened();

    emit fileOpened(*filemap);
    changeCurrent(index);
    if(openFiles.size() == 1) emit hasOpenFile(true);
}

void Editor::newFile()
{
    QVariantMap *filemap;
    filemap = new QVariantMap;
    (*filemap)["name"] = "untitled";
    (*filemap)["path"] = (*filemap)["type"] = (*filemap)["content"] = "";
    QVariantMap *typeMap = getDefaultFileType();
    (*filemap)["mode"] = (*typeMap)["name"];
    (*filemap)["mime"] = (*typeMap)["mime"];
    int index = openFiles.size();
    (*filemap)["id"] = index;
    (*filemap)["modified"] = false;
    (*filemap)["new"] = true;
    openFiles.append(filemap);

    emit fileOpened(*filemap);
    changeCurrent(index);
    if(openFiles.size() == 1) emit hasOpenFile(true);
}

void Editor::closeFile(QVariantMap *filemap)
{
    if(filemap == NULL) filemap = currentOpenFile;
    tabOrder.removeOne(filemap);
    delete filemap;
    if(openFiles.size() == 0)
        emit hasOpenFile(false);
    else
        changeCurrent(openFiles.indexOf(tabOrder.first()));
}

void Editor::closeFile(int index)
{
    closeFile(openFiles.takeAt(index));
}

int Editor::cycleNextFile()
{
    ++currentTab;
    if(currentTab == tabOrder.end())
        currentTab = tabOrder.begin();
    int index = openFiles.indexOf(*currentTab);
    changeCurrent(index);
    return index;
}

int Editor::cyclePrevFile()
{
    if(currentTab == tabOrder.begin()){
        currentTab = tabOrder.end();
    }
    --currentTab;
    int index = openFiles.indexOf(*currentTab);
    changeCurrent(index);
    return index;
}

bool Editor::hasOpenFile()
{
    if(openFiles.size() == 0) return false;
    else return true;
}

bool Editor::hasOpenFileAndVisible()
{
    return (hasOpenFile() && visible);
}

void Editor::switchCurrTab()
{
    tabOrder.push_front(tabOrder.takeAt(tabOrder.indexOf(*currentTab)));
    currentTab = tabOrder.begin();
}

void Editor::changeCurrent(QVariantMap *curr)
{
    currentOpenFile = curr;
    //if(!(QApplication::keyboardModifiers() & Qt::ControlModifier)){
        //int tabIndex = tabOrder.indexOf(currentOpenFile);
        if(!tabOrder.contains(currentOpenFile)){ //Newly created file
            tabOrder.push_front(currentOpenFile);
            currentTab = tabOrder.begin();
        }
        else{
            currentTab = tabOrder.begin();
            while(*currentTab != currentOpenFile)
                ++currentTab;
        }
        //currentTab = tabOrder.begin();
    //}
    emit currentChanged(openFiles.indexOf(currentOpenFile));
}

void Editor::changeCurrent(int index)
{
    currentOpenFile = openFiles.at(index);
    if(!tabOrder.contains(currentOpenFile)){ //Newly created file
        tabOrder.push_front(currentOpenFile);
        currentTab = tabOrder.begin();
    }
    else{
        currentTab = tabOrder.begin();
        while(*currentTab != currentOpenFile)
            ++currentTab;
    }

    QList<QAction*> actions = syntaxActions->actions();
    for(int i=0; i< actions.size(); i++){
        if(actions[i]->text() != (*currentOpenFile)["mode"]){
            if(actions[i]->isChecked())
                actions[i]->setChecked(false);
        }
        else{
            actions[i]->setChecked(true);
        }
    }
    emit currentChanged(index);
}

void Editor::switchCurrent(int index)
{
    changeCurrent(openFiles.at(index));
    switchCurrTab();
}

int Editor::getFileCount()
{
    return openFiles.length();
}

QVariant Editor::getFileAttr(int index, const QString &key)
{
    return (*openFiles.at(index))[key];
}

QVariant Editor::getCurrentFileAttr(const QString &key)
{
    return (*currentOpenFile)[key];
}

int Editor::getCurrentFileIndex()
{
    return openFiles.indexOf(currentOpenFile);
}

void Editor::setFileAttr(int index, const QString &key, const QVariant &value)
{
    QVariantMap *fileMap = openFiles.at(index);

    (*fileMap)[key] = value;
    if(key == "name"){
        emit fileTitleChanged(index, *fileMap);

        /* Check whether the extension was changed and update mode accordingly */
        QFileInfo info(value.toString());
        if((*fileMap)["type"] == info.suffix()){
            QVariantMap *typeMap = getFileTypeByExt(info.suffix());
            (*fileMap)["type"] = info.suffix();
            (*fileMap)["mode"] = (*typeMap)["name"];
            (*fileMap)["mime"] = (*typeMap)["mime"];
            emit fileTypeChanged(index, *typeMap);
        }
    }
    else if(key == "mode"){
        QVariantMap *typeMap = getFileTypeByMode(value.toString());
        (*fileMap)["mime"] = (*typeMap)["mime"];
        emit fileTypeChanged(index, *typeMap);
    }
}

void Editor::setCurrentFileAttr(const QString &key, const QVariant &value)
{
    setFileAttr(openFiles.indexOf(currentOpenFile), key, value);
}

void Editor::sendMsgJS(QString msg)
{
    emit jsMsg(tr("[QTMSG] => %1").arg(msg));
}

void Editor::setFileModified()
{
    if(getCurrentFileAttr("modified").toBool() == false){
        setCurrentFileAttr("modified", true);
        emit fileModified();
    }
}

void Editor::saveFileContents(int index, QString content)
{
    setFileAttr(index, "content", content);
}

void Editor::retrieveFile(int index)
{
    emit fileSave(index);
}

bool Editor::isSidebarHidden()
{
    return sidebarHidden;
}

void Editor::setSidebarHidden(bool ok)
{
    sidebarHidden = ok;
}

