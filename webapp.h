#ifndef WEBAPP_H
#define WEBAPP_H

#include <QWebPage>
#include <QWebFrame>

#define wApp WebApp::getInstance()

class WebApp : public QWebPage
{
    Q_OBJECT

    Q_PROPERTY(bool isSidebarVisible READ isSidebarVisible WRITE setSidebarVisible NOTIFY toggleSidebar)

public:
    static WebApp& getInstance()
    {
        if(!m_instance) m_instance = new WebApp();
        return *m_instance;
    }

    void loadFile(const QString &filePath);
    QVariant eval(const QString &scriptSource);
    void addObject(const QString &name, QObject *object);

    bool isSidebarVisible();
    void setSidebarVisible(bool);

    void undo();
    void redo();

signals:
    void toggleSidebar(bool);

public slots:
    void log(const QString&);
protected:
    WebApp(QObject *parent = 0);
    static WebApp *m_instance;
    
private slots:
    void attachObject();
    void undoChanged(bool);

private:
    bool m_isSidebarVisible;
    QUndoStack *m_undoStack;
    
};

#endif // WEBAPP_H
