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

void Editor::changeCurrent(int index)
{
    currentOpenFile = openFiles.at(index);
    emit currentChanged(index);
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

void Editor::setFileAttr(int index, const QString &key, const QVariant &value)
{
   (*openFiles.at(index))[key] = value;

    if(key == "name")
        emit fileTitleChanged(index, value.toString());
}

void Editor::setCurrentFileAttr(const QString &key, const QVariant &value)
{
    setFileAttr((*currentOpenFile)["id"].toInt(), key, value);
}

void Editor::sendMsgJS(QString msg)
{
    emit jsMsg(tr("[QTMSG] => %1").arg(msg));
}

void Editor::setFileModified()
{
    setCurrentFileAttr("modified", true);
    emit fileModified();
}

void Editor::saveFileContents(int index, QString content)
{
    setFileAttr(index, "content", content);
}

void Editor::retrieveFile(int index)
{
    emit fileSave(index);
}

