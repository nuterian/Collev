#include "webapp.h"
#include <QDebug>
#include <QUndoStack>

WebApp *WebApp::m_instance = NULL;

WebApp::WebApp(QObject *parent) :QWebPage(parent)
{
    m_isSidebarVisible = true;
    m_undoStack = this->undoStack();
    connect(this->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()));
    connect(m_undoStack, SIGNAL(canUndoChanged(bool)), this, SLOT(undoChanged(bool)));
}

void WebApp::loadFile(const QString &filePath)
{
    mainFrame()->setUrl(QUrl(filePath));
}

QVariant WebApp::eval(const QString &scriptSource)
{
    return mainFrame()->evaluateJavaScript(scriptSource);
}

void WebApp::addObject(const QString &name, QObject *object)
{
    mainFrame()->addToJavaScriptWindowObject(name, object);
}

void WebApp::log(const QString &text)
{
    eval(tr("log('%1')").arg(text));
}

void WebApp::attachObject()
{
    addObject("APP", this);
}

bool WebApp::isSidebarVisible()
{
    return m_isSidebarVisible;
}

void WebApp::setSidebarVisible(bool visible)
{
    if(m_isSidebarVisible != visible)
    {
        m_isSidebarVisible = visible;
        emit toggleSidebar(visible);
    }
}

void WebApp::undoChanged(bool change)
{
    qDebug()<<"Undo changed: "<<change;
    qDebug()<<"Redo: "<<m_undoStack->canRedo();
}

void WebApp::undo()
{
    eval("editor.undo()");
}

void WebApp::redo()
{
    eval("editor.redo()");
}


