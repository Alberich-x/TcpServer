#include <MD5.h>
#include <cmath>


MD5::MD5()
{
	for (int i = 0; i < 64; i++)
	{
		k[i] = floor(abs(sin(i + 1) * pow(2, 32)));
	}

	int h1 = 0x67452301;
	int h2 = 0xEFCDAB89;
	int h3 = 0x98BADCFE;
	int h4 = 0x10325476;

}
void MD5::trans(QByteArray message)
{
	init(message);
}




void MD5::init(QByteArray message)
{
	int array_size = message.size();
	if (array_size % 512 != 448)
	{
		int num_append = array_size % 512 - 448 > 0?
			512 - (array_size % 512 - 448) : array_size % 512;
		message.append('\0');
		for (int i = 0; i < num_append - 1; i++)
		{
			message.append('\1');
		}
		qint64 sizeToAppend = array_size;
		message.append(sizeToAppend);
	}
	while (!message.isEmpty())
	{
		process(message.left(512));
		message.remove(0, 512);
	}
}



void MD5::process(QByteArray message)
{
	//进行循环加密, 过程见MD5: figure1.
	quint32 a = h[0], b = h[1], c = h[2], d = h[3];
	quint32 f, g, temp;
	for (int i = 0; i < 64; i++)
	{
		switch (i /16 +1)
		{
		case 1:
			f = F(b, c, d);
			g = i;
		case 2:
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		case 3:
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		case 4:
			f = I(b, c, d);
			g = (7 * i) % 16;
		}
		int temp = d;
		d = c;
		c = b;
		b = ((a + f + message.toInt() + k[i]) << r[i]) + b;
		a = temp;
		qDebug() << a << b << c << d;
	}
	h[0] = h[0] + a;
	h[1] = h[1] + b;
	h[2] = h[2] + c;
	h[3] = h[3] + d;
}


QByteArray MD5::getMD5()
{
	QByteArray return_byteArray;
	qDebug() << h[0] << h[1] << h[2] << h[3];
	QString string_temp;
	return_byteArray.append(QString("%1").arg(h[0], 8, 16, QLatin1Char('0')));
	return_byteArray.append(QString("%1").arg(h[1], 8, 16, QLatin1Char('0')));
	return_byteArray.append(QString("%1").arg(h[2], 8, 16, QLatin1Char('0')));
	return_byteArray.append(QString("%1").arg(h[3], 8, 16, QLatin1Char('0')));
	//return_byteArray.append();
	//return_byteArray.append(static_cast<char>(h[1]));
	//return_byteArray.append(static_cast<char>(h[2]));
	//return_byteArray.append(static_cast<char>(h[3]));
	return return_byteArray;
}






