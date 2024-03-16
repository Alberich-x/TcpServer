#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <qtcpserver.h>
#include <qdatastream.h>
#include <qtcpsocket.h>
#include <qthread.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qdir.h>
#include <QDateTime>
#include <QQueue>
#include <qsemaphore.h>
#include <QTimer>
#include <MD5.h>



QT_BEGIN_NAMESPACE
namespace Ui { class tcpServer; }
QT_END_NAMESPACE
class MyThread;
class socketHelper;
class MyServer;

//�����ĸ���:
//MyThread:�̳�QThread��, ��дrun()����,�������߳�����ʱ����SocketHelper����, ͬʱ����isocupancy�ж����߳��Ƿ��б�ռ��.
//MySocket:�̳�QTcpSocket��, ������������ĸ������, ������Socket�����䡢ɾ����Ѱ���ļ��еķ�ʽ, index���ڼ�¼��ǰthreadλ��, �������ɾ��.
//����ͨ�������ļ������ļ���С�ķ�ʽ(����Ӷ�����Ϣ), �ȴ���һ���ļ���Ϣ, Ȼ��ʼ�����ļ�����, ���Ѵ����С���贫���С���ʱ������һ���ļ�����
//����ͨ�����źŵķ�ʽ, ByteWritten֮��ᴥ����һ����������(ԭ��ʹ��while����ʣ����Ϣ, ���ǳ��ڶԶ��̵߳Ŀ���ʹ�����ź�), ��ͷ���ж��Ƿ��Ѿ��������, Ȼ��emit�ź�Ѱ����һ���ļ�
//MyServer:�̳�QTcpServer��, ��Ҫ���ڴ���һ���߳�vector, Ȼ����дincomingConnection()����, �ں�������ʱ�Ϳ����Զ���ʼ�����ж��Ƿ��п����̲߳������ʹ���Socket.
//SocketHelper:���ڴ���socket��������Ϣ, Ŀǰֻ���˴���socket, �������ۺ��������ź�.
//����:
//�յ�����->incommingConnection()->pushback(newthread)->createSocket(), ͬʱ�󶨲ۺ���
//�յ���������->deal_send()�ж��Ƿ���ռ�߳�, ���ж��Ƿ��ڴ������յ��ź�->find_folder()ͨ���ļ������䵥���ļ�
//->�����ļ�ͷ��Ϣ->ͨ�����źŴ�����������continue_trans()->����ǰ�ļ�����ɹ��򴥷�find_folder()������д����ͣ


//������:
//1. ����ͨ�����һ��qint64��ʹͷ�ļ��ڴ��и�����Ϣ,
//2. �����ļ���


class tcpServer : public QMainWindow
{
    Q_OBJECT

public:
    tcpServer(QWidget *parent = nullptr);
    ~tcpServer();
    void appendText(QString text_append);
    Ui::tcpServer* ui;
private:

private slots:
    

};

class MySocket : public QTcpSocket{
    Q_OBJECT
public :
    bool could_read = 1;
    MD5* md5_maker;
    QTimer _timer;

    QVector<QString>filename_list;//�����ļ��б�
    qint64 _upTotalsize;           //���ļ��ܹ��贫��Ĵ�С
    qint64 _numBytes;              //ÿ��д��ı�������С
    qint64 _upBytesToWrite;        //����д����ļ���С
    //qint64 downBytesWritten;      //�ϰ�, ɾ��֮��֪������Ӱ��
    QFile* _sendFile = nullptr;    //��ǰ�򿪵��ļ�
    int index;
    void sendFile(QString filepath, MySocket* socket); //�����ļ�ͷ��Ϣ
    void dealSendSignal();                                  //�����źź�ִ��, ���ڻ�ȡ�ļ���Ϣ��ִ�д���
    void dealDeleteSignal();                                //ɾ������, ͬʱʹ�߳̽�������̬
    void findFolder();                                //�ж��ļ��Ƿ��д������, û��������������ڶ�ȡ�ļ�������
    void recursionFind(QFileInfoList &list_dir, QDir dir);                              //�ݹ�����ļ������ļ�����
    void continueTransEvent();                             //�����źź����ļ�����
    

private :
    void logging(QString message_log);                 //��¼log.


    void  selectFile();

signals:
    void continueFolder();                             //���ڷ��͵�ǰ�ļ�������ϵ��ź�

};


class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    QVector<MyThread*> list_thread;                     //�߳��б�
private:
    void incomingConnection(qintptr socketDescriptor);  //��дincomingConnection, ������������ʱֱ�����ó�ʼ������
};


class MyThread : public QThread
{
public:
    MyThread();
    socketHelper* socket_helper;                        //ÿ���̶߳����Լ���socketHelper������Socket, ���ڴ��������̵߳�socket.
    //MySocket* socket_this;
    void run() override;                                //��дrun()����, ���߳�start()ʱ����Զ������ۺ�����
    MyServer *myserver;                                 
    bool isocupancy = false;                            //�ж��Ƿ��б�ռ��
private:

};


class socketHelper : public QObject
{
    Q_OBJECT
public:
    void CreateSocket(qintptr socketDescriptor, int index); //�������ڸ��̵߳�socket.
    MyServer* myserver;
    MyThread *thread_this;
signals:
    //��������ź�
    void create(qintptr  socketDescriptor, int index); 
    //���������, ����socket����Ϣ��, Ŀǰû���õ�
    void addlist(MySocket*, int);
    void remove(MySocket*, int);
};

#endif // TCPSERVER_H
