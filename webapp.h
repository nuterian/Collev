#ifndef WEBAPP_H
#define WEBAPP_H

#include <QWebPage>
#include <QWebFrame>

#define wApp WebApp::getInstance()

class WebApp : public QWebPage
{
    Q_OBJECT
public:
    static WebApp& getInstance()
    {
        if(!m_instance) m_instance = new WebApp();
        return *m_instance;
    }

    void loadFile(const QString &filePath);
    QVariant eval(const QString &scriptSource);
    void addObject(const QString &name, QObject *object);

public slots:
    void log(const QString&);
protected:
    WebApp(QObject *parent = 0);
    static WebApp *m_instance;
    
signals:
    
public slots:
    
};

#endif // WEBAPP_H
