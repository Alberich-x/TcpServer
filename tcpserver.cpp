#include "tcpserver.h"
#include "ui_tcpserver.h"
const qint64 perLoadSize = 64 * 1024;
QString dirsend_path = "./file_server";
MyThread* thread_temp;
QQueue<QString> queue_string;
QSemaphore* mutex;
MyServer* myserver;
MySocket* socket_temp = nullptr;
Ui::tcpServer* ui_out;

tcpServer::tcpServer(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::tcpServer)
{
	mutex = new QSemaphore(1);
	ui_out = ui;
	ui->setupUi(this);



	//QByteArray a;
	//QDataStream datastream(&a, QIODevice::ReadWrite);
	//datastream << qint64(13115);
	//qint64 tempo;
	//datastream >> tempo;
	//qDebug() << "tempo:" << tempo;





	myserver = new MyServer;
	if (!myserver->listen(QHostAddress::Any, 11451))
	{
		QMessageBox::warning(this, u8"错误", myserver->errorString());
		myserver->close();
		myserver->deleteLater();
		myserver = nullptr;
		return;
	}
	ui->textBrowser->insertPlainText(u8"开始监听: Port:[11451]\n");
}

tcpServer::~tcpServer()
{
	delete ui;
}

void tcpServer::appendText(QString text_append)
{
	ui->textBrowser->insertPlainText(text_append);
}







//处理到来的连接事件
void MyServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "New Connection.";
	int index_this = 0;
	foreach(MyThread * thread_this, list_thread)
	{
		if (thread_this->isocupancy == false)
		{
			thread_this->isocupancy = true;

			thread_this->start();
			thread_temp = thread_this;
			qDebug() << "Thread No." << index_this << thread_this;
			emit thread_this->socket_helper->CreateSocket(socketDescriptor, index_this);
			return;
		}
		index_this++;
	}
	MyThread* newthread = new MyThread;
	thread_temp = newthread;
	newthread->isocupancy = true;
	newthread->start();
	list_thread.push_back(newthread);

	qDebug() << "Thread No." << index_this << newthread;

	emit newthread->socket_helper->CreateSocket(socketDescriptor, index_this);
	index_this++;
}







MyThread::MyThread()
{
	this->socket_helper = new socketHelper;
}


//线程启动时为socket_helper绑定槽函数
void MyThread::run()
{
	this->socket_helper->myserver = this->myserver;
	//this->socket_helper->thread_this = this;
	connect(this->socket_helper, &socketHelper::create, this->socket_helper, &socketHelper::CreateSocket);
	exec();
}

//创建新的socket
void socketHelper::CreateSocket(qintptr socketDescriptor, int index)
{
	qDebug() << "Recieve the signal." << index;
	MySocket* tcpsocket = new MySocket;
	tcpsocket->moveToThread(thread_temp);
	tcpsocket->index = index;
	tcpsocket->setSocketDescriptor(socketDescriptor);
	ui_out->textBrowser->insertPlainText(tcpsocket->peerAddress().toString().section(':', 3) + " has connected.\n");
	qDebug() << tcpsocket->peerAddress(), tcpsocket->peerPort();
	tcpsocket->md5_maker = new MD5;
	connect(tcpsocket, &MySocket::readyRead, tcpsocket, &MySocket::dealSendSignal);
	connect(tcpsocket, &MySocket::disconnected, tcpsocket, &MySocket::dealDeleteSignal);
	connect(tcpsocket, &MySocket::bytesWritten, tcpsocket, &MySocket::continueTransEvent);
	//connect(tcpsocket, &MySocket::continueFolder, tcpsocket, &MySocket::find_folder);
}

void MySocket::dealSendSignal()
{
	//判断是否正在读取
	if (this->could_read == 0)
	{
		qDebug() << "Is reading";
		return;
	}
	qDebug() << "Deal file event. Thread: " << MyThread::currentThread();
	//此处进行锁操作,防止抢占线程
	queue_string.push_back(this->peerAddress().toString());
	while (1)
	{
		qDebug() << "Receive the signal form: " << this->peerAddress().toString();
		if (socket_temp != nullptr)
			socket_temp->waitForBytesWritten();
		mutex->acquire();
		if (this->peerAddress().toString() != queue_string.front())
		{
			mutex->release();
		}
		else
		{
			queue_string.pop_front();
			this->could_read = 0;
			break;
		}
	}
	selectFile();
}

void MySocket::selectFile()
{
	//读取当前当前文件夹内的文件信息
	//QString dir_path = QFileDialog::getExistingDirectory(nullptr, tr(u8"打开文件夹"), QDir::homePath());
	QString dir_path = dirsend_path;
	QDir dir(dir_path);
	//QFileInfoList list_dir = dir.entryInfoList(QDir::Files);
	QFileInfoList list_dir;
	recursionFind(list_dir, dir);
	foreach(QFileInfo fileinfo, list_dir)
	{
		qDebug() << fileinfo.path() + "/" + fileinfo.fileName();
		filename_list.push_back(fileinfo.path() + "/" + fileinfo.fileName());
		
	}
	findFolder();
}

void MySocket::recursionFind(QFileInfoList& list_dir, QDir dir)
{
	QStringList list_temp;
	QFileInfoList list_dir_temp;
	qDebug() << dir;
	if (!dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot).isEmpty())
	{
		list_temp = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		foreach(QString text_temp, list_temp)
		{
			qDebug() << "Dirs: " << text_temp;
			if (dir.cd(text_temp));
			{
				qDebug() << dir.path();
				recursionFind(list_dir, dir);
				dir.cdUp();
			}
		}
	}
	else
		qDebug() << "No directions here: " << dir.path();

	if(!dir.entryInfoList(QDir::Files).isEmpty())
	{
		list_dir_temp = dir.entryInfoList(QDir::Files);
		foreach(QFileInfo fi, list_dir_temp)
		list_dir.push_back(fi);
	}
	return;
	
}




void MySocket::findFolder()
{

	//判断是否传输完所有的文件
	if (!filename_list.isEmpty())
	{
		if (_sendFile != nullptr)
			_sendFile->close();
		
		qDebug() << "Wait for a moment.";
		this->flush();
		this->waitForBytesWritten();
		QThread::msleep(20);
		


		qDebug() << "Next file.";

		sendFile(filename_list.front(), this);
		filename_list.pop_front();
		return;
	}
	//结束传输单个文件, 并进行关闭文件夹等操作.
	QString dir_path = dirsend_path;
	QString text_this = "Has send " + dir_path + " to " + this->peerAddress().toString().section(':', 3) + "\n";
	socket_temp = this;
	this->could_read = 1;
	this->flush();
	this->waitForBytesWritten();
	qDebug() << text_this;
	mutex->release();
	//appendText(text_this);
	ui_out->textBrowser->insertPlainText(text_this);
	logging(text_this);
}

void MySocket::dealDeleteSignal()
{
	//结束socket, 并休眠线程.
	qDebug() << "Try to Delete.";
	qDebug() << index;
	ui_out->textBrowser->insertPlainText(this->peerAddress().toString().section(':', 3) + " has disconnected.\n");
	logging(this->peerAddress().toString().section(':', 3) + " has disconnected.\n");
	myserver->list_thread[index]->quit();
	myserver->list_thread[index]->isocupancy = false;
	qDebug() << "Disconnect, thread released.";
	this->deleteLater();
}





























void MySocket::sendFile(QString filePath, MySocket* socket)
{
	//开始传输头文件信息
	QByteArray outblock;
	QString filename;
	_upTotalsize = 0;
	if (socket != nullptr)
	{
		_sendFile = new QFile(filePath);
		if (!_sendFile->open(QFile::ReadOnly))
		{
			qDebug() << "Can't open the File:" << filePath;
		}
		qDebug() << "Try to send the file:" << filePath;
		_upTotalsize = _sendFile->size();
		QDataStream datastream(&outblock, QIODevice::ReadWrite);     
		datastream.setVersion(QDataStream::Qt_5_14);//将outblock和datastream绑定
		//filename = filePath.right(filePath.size() - filePath.lastIndexOf('/') - 1);                                 //仅保留文件名
		filename = filePath.right(filePath.size() - filePath.indexOf('/', 2) - 1);


		qDebug() << filename;

		datastream << qint64(0) << qint64(0) << filename;                                                           //预留文件大小和文件名大小
		_upTotalsize += outblock.size();                                                                             //发送文件文件结构的大小
		datastream.device()->seek(0);                                                                               //回到文件头
		datastream << _upTotalsize << qint64(outblock.size() - sizeof(qint64) * 2);

		//测试传输是否正确

		_numBytes = (int)socket->write(outblock);
		_upBytesToWrite = _upTotalsize - outblock.size();
		qDebug() << u8"已经传入名称:" << filename << u8"文件大小:" << _upTotalsize;

		//socket->flush();
		//socket->waitForBytesWritten();
		outblock.resize(0);
		//传输文件内容
	}
}



void MySocket::continueTransEvent()
{
	//通过槽信号传输来进行的继续传输

	QByteArray outblock;
	outblock.resize(0);
	{
		outblock = _sendFile->read(qMin(_upBytesToWrite, perLoadSize));   //读出剩下的数据，小于perLoadSize则读剩下数据
		md5_maker->trans(outblock);
		QByteArray byteArray = md5_maker->getMD5();
		//QString hexString = byteArray.toHex();
		qDebug() << byteArray;
		int write_read = (int)this->write(outblock);
		_upBytesToWrite -= write_read;                                   //记录剩下需要读取的信息.
		outblock.resize(0);
	}
	if (_upBytesToWrite == 0)
	{

		findFolder();
		return;
	}
}






















void MySocket::logging(QString message_log)
{
	QDateTime dateTime = QDateTime::currentDateTime();//获取系统当前的时间
	QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss") + ": ";//格式化时间
	message_log = str + message_log;
	QString file_log = "log.txt";
	QFile file(file_log);
	if (file.exists())
	{
		if (!file.open(QIODevice::Append | QIODevice::Text))
		{
			qDebug() << "Can't open the log file.";
		}
		QTextStream in_text(&file);
		in_text << message_log;
	}
	else
	{
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream in_text(&file);
		in_text << message_log;
	}
	return;
}








/*废案区


//void MySocket::updateProgramme(qint64 _numBytes, qint64 _upBytesToWrite, QFile* _sendfile)
//{
//    qint64 downBytesWritten = 0;
//    QByteArray outblock;
//    downBytesWritten += (int)_numBytes;
//    while (_upBytesToWrite > 0)
//    {
//        outblock = _sendfile->read(qMin(_upBytesToWrite, perLoadSize));   //读出剩下的数据，小于perLoadSize则读剩下数据
//        int write_read = (int)socket->write(outblock);
//        _upBytesToWrite -= write_read;                                   //记录剩下需要读取的信息.
//        outblock.resize(0);
//    }
//    qDebug() << "Completed.";
//    QThread::msleep(100);
//    _sendfile->close();
//}

*/