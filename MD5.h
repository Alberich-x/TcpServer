#pragma once
#include <QString>
#include <QDebug>
#define F(x, y, z)  ((x & y) | ((~x) & z))
#define G(x, y, z)  ((x & z) | (y & (~z)))
#define H(x, y, z)  ((x) ^ (y) ^ (z))
#define I(x, y, z)  (y ^ (x | (~z)))



//–£—ÈÀ„∑®
struct MD5
{
public:
	quint32 h[4];
	MD5();
	QByteArray getMD5();
	void trans(QByteArray message);
private:
	void init(QByteArray message);
	void process(QByteArray message);
	quint32 r[64] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23 ,6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
	quint32 k[64];

};