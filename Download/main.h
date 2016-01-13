#ifndef DEVIANTART_D_MAIN_H
#define DEVIANTART_D_MAIN_H
#include <mtoken.h>
#include <QtWidgets>

class DownloadManager: public QWidget{
Q_OBJECT
public:
	DownloadManager();

	void append(QString url);
	QString saveFileName(const QUrl &url, bool l);

private slots:
	void startNextDownload();
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFinished();
	void downloadReadyRead();

private:
	QNetworkAccessManager manager;
	QQueue<QUrl> downloadQueue;
	QStringList* exists;
	QNetworkReply *currentDownload;
	QFile output;
	QTime downloadTime;
 MToken* token;

	int downloadedCount;
	int totalCount;

	bool running = true;

 QLabel* l;
	QProgressBar* p;
	QTextEdit* log;
};

#endif //DEVIANTART_D_MAIN_H
