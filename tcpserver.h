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

//定义四个类:
//MyThread:继承QThread类, 重写run()函数,用于在线程启动时创建SocketHelper对象, 同时定义isocupancy判定该线程是否有被占用.
//MySocket:继承QTcpSocket类, 包含传输所需的各类参数, 并定义Socket对象传输、删除、寻找文件夹的方式, index用于记录当前thread位置, 方便进行删除.
//传输通过传递文件名和文件大小的方式(可添加额外信息), 先传递一条文件信息, 然后开始传输文件内容, 当已传输大小与需传输大小相等时进行下一个文件传输
//传输通过槽信号的方式, ByteWritten之后会触发下一条传输命令(原先使用while读完剩余信息, 但是出于对多线程的考虑使用了信号), 开头会判断是否已经传输完毕, 然后emit信号寻找下一个文件
//MyServer:继承QTcpServer类, 主要用于创建一个线程vector, 然后重写incomingConnection()函数, 在函数到来时就可以自动初始化并判断是否有空余线程并启动和创建Socket.
//SocketHelper:用于创建socket并传递信息, 目前只用了创建socket, 并创建槽函数接收信号.
//流程:
//收到连接->incommingConnection()->pushback(newthread)->createSocket(), 同时绑定槽函数
//收到传输请求->deal_send()判断是否抢占线程, 并判断是否在传输中收到信号->find_folder()通过文件名传输单个文件
//->传输文件头信息->通过槽信号触发传输内容continue_trans()->若当前文件传输成功则触发find_folder()继续读写或暂停


//待补充:
//1. 可以通过添加一个qint64来使头文件内带有更多信息,
//2. 传输文件夹


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

    QVector<QString>filename_list;//传输文件列表
    qint64 _upTotalsize;           //单文件总共需传输的大小
    qint64 _numBytes;              //每次写入的比特流大小
    qint64 _upBytesToWrite;        //还需写入的文件大小
    //qint64 downBytesWritten;      //废案, 删掉之后不知道有无影响
    QFile* _sendFile = nullptr;    //当前打开的文件
    int index;
    void sendFile(QString filepath, MySocket* socket); //传输文件头信息
    void dealSendSignal();                                  //接收信号后执行, 用于获取文件信息并执行传输
    void dealDeleteSignal();                                //删除操作, 同时使线程进入休眠态
    void findFolder();                                //判断文件是否有传输完毕, 没有则继续从数组内读取文件并传输
    void recursionFind(QFileInfoList &list_dir, QDir dir);                              //递归查找文件夹内文件内容
    void continueTransEvent();                             //接收信号后传输文件内容
    

private :
    void logging(QString message_log);                 //记录log.


    void  selectFile();

signals:
    void continueFolder();                             //用于发送当前文件传输完毕的信号

};


class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    QVector<MyThread*> list_thread;                     //线程列表
private:
    void incomingConnection(qintptr socketDescriptor);  //重写incomingConnection, 用于在新连接时直接做好初始化工作
};


class MyThread : public QThread
{
public:
    MyThread();
    socketHelper* socket_helper;                        //每个线程都有自己的socketHelper来创建Socket, 用于创建属于线程的socket.
    //MySocket* socket_this;
    void run() override;                                //重写run()函数, 在线程start()时便会自动创建槽函数等
    MyServer *myserver;                                 
    bool isocupancy = false;                            //判断是否有被占用
private:

};


class socketHelper : public QObject
{
    Q_OBJECT
public:
    void CreateSocket(qintptr socketDescriptor, int index); //创建属于该线程的socket.
    MyServer* myserver;
    MyThread *thread_this;
signals:
    //传输各种信号
    void create(qintptr  socketDescriptor, int index); 
    //可添加内容, 修饰socket的信息等, 目前没有用到
    void addlist(MySocket*, int);
    void remove(MySocket*, int);
};

#endif // TCPSERVER_H
