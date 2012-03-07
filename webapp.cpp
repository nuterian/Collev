#include "webapp.h"

WebApp *WebApp::m_instance = NULL;

WebApp::WebApp(QObject *parent) :QWebPage(parent)
{
}

void WebApp::loadFile(const QString &filePath)
{
    mainFrame()->setUrl(QUrl(file));
}

void WebApp::eval(const QString &scriptSource)
{
    mainFrame()->evaluateJavaScript(scriptSource);
}

void WebApp::addObject(const QString &name, QObject *object)
{
    mainFrame()->addToJavaScriptWindowObject(name, object);
}

void WebApp::log(const QString &text)
{
    eval(tr("log('%1')").arg(text));
}

