#ifndef DEVIANTART_D_MAIN_H
#define DEVIANTART_D_MAIN_H
#include <mtoken.h>
#include <QtWidgets>

class DownloadManager: public QWidget{
Q_OBJECT
public:
	DownloadManager();

	void append(QString url);
	QString saveFileName(QJsonObject o);
 QString saveFileName(QString s);

	void hideEvent(QHideEvent *);
private slots:
	void tryStartNextDownload();
	void stopDownload();
	void startDownload();
	void nextDownload();
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFinished();
	void downloadReadyRead();
	void trayClick(QSystemTrayIcon::ActivationReason reason);

	void checkFiles();
	void checkNextFile();
	void tryStartNextFile();

	void moveFiles();
	void printTime(int i, int t);

	void setButtonsEnabled(bool f);
	void setWidgetsEnabled(bool f);
	void jobStart();
	void jobEnd();
private:
	// Job type
	QGroupBox* rbox;
	QRadioButton* rb_1;
	QRadioButton* rb_2;
	QRadioButton* rb_3;
	QPushButton* rbstart;
	int jobType = 0;// 1-Download, 2-Check, 3-Move

	// Download-Check
	QNetworkAccessManager manager;
	QQueue<QUrl> downloadQueue;
	QStringList* exists;
	QNetworkReply *currentDownload;
	QFile output;
	QTime* downloadTime;
 MToken* token;

	// Check
	QFile ex, ws, de;
	int iex, iws, ide;
	QStringList lex,lws,lde;

	// Other
	int downloadedCount;
	int totalCount;

	bool running = true;

	// Logs-Buttons
	QProgressBar* p;
	QTextEdit* log;
	QPushButton* b_sr;
	QPushButton* b_st;

	// System
	QIcon icon;
	QSystemTrayIcon* tray;
	QMenu* tmenu;
};

#endif //DEVIANTART_D_MAIN_H
