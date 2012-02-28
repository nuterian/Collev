#include "editor.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

Editor::Editor(QObject *parent) : QObject(parent)
{
    //currentOpenFile = NULL;
}

void Editor::openFile(QFile &file)
{
    QVariantMap *filemap;
    filemap = new QVariantMap;
    QFileInfo info(file.fileName());
    (*filemap)["name"] = info.fileName();
    (*filemap)["path"] = info.filePath();
    (*filemap)["type"] = info.suffix();
    QTextStream in(&file);
    (*filemap)["content"] = in.readAll();
    int index = openFiles.size();
    (*filemap)["id"] = index;
    (*filemap)["modified"] = false;
    (*filemap)["new"] = false;
    openFiles.append(filemap);

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
   (*openFiles.at(index))[key] = value;

    if(key == "name")
        emit fileTitleChanged(index, value.toString());
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

