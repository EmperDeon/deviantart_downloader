#ifndef DEVIANTART_D_MTOKEN_H
#define DEVIANTART_D_MTOKEN_H
#include <QtCore>
#include <QtNetwork>

class MToken : public QObject{
Q_OBJECT
	QNetworkAccessManager* managerT;
	QNetworkAccessManager* manager;
	QTimer* tokent;

	void getToken();
	bool testToken();
	QByteArray GETt(QUrl r);
public slots:
	void upd();

public:
	QString token;

	MToken();
	QByteArray GET(QUrl r);
};

void loadFile(QString f, QStringList& list);
void loadMFile(QString f, QMap<QString, QString>& map);
void loadFile(QString f, QStringList& list, QStringList check);
void saveFile(QString f, QStringList list);

#endif //DEVIANTART_D_MTOKEN_H
