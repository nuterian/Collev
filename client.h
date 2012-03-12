#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

class QUdpSocket;

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = 0);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
};


#endif // CLIENT_H
