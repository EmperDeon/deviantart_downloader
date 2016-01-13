#include "main.h"

int main(int argc, char** argv){
	QApplication a(argc, argv);
	DownloadManager wg;
	wg.show();
	a.exec();
}

DownloadManager::DownloadManager() :downloadedCount(0), totalCount(0){
	token = new MToken;
	exists = new QStringList;
	loadFile("links_exist.txt", *exists);

	QVBoxLayout* vl = new QVBoxLayout;
	l = new QLabel("");
	p = new QProgressBar;
	log = new QTextEdit;
	vl->addWidget(l);
	vl->addWidget(p);
	vl->addSpacing(15);
	vl->addWidget(log);
	this->setLayout(vl);

	QStringList links;
	loadFile("linksF.txt", links);
	for(QString s : links)
		append(s);

	this->totalCount = downloadQueue.size();
}

void DownloadManager::append(QString url){
	if (downloadQueue.isEmpty())
		QTimer::singleShot(0, this, SLOT(startNextDownload()));

	downloadQueue.enqueue(QUrl(url));
}

bool isPic(QString s){ return s.endsWith(".png") || s.endsWith(".jpg");}

QString DownloadManager::saveFileName(const QUrl &url, bool l){
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();
	int f = basename.indexOf("_by_") + 4;

	if(isPic(basename) || f < 5){
		if (basename.isEmpty())
			basename = "download";

		QString dir = l ? "downloaded_low/":"downloaded/";

		int d = basename.indexOf("-d");
		if(basename.indexOf("-d", f) == -1) d = basename.indexOf(".", f);
		path = QStringRef(&basename, f, d - f).toString();
		QDir().mkpath(dir + path + "/");
		return dir + path + "/" + basename;
	}else{
		return "down_other/"+basename;
	}
}

void DownloadManager::startNextDownload(){
	if (downloadQueue.isEmpty()) {
		log->append(QString("%1/%2 files downloaded successfully").arg(downloadedCount).arg(totalCount));
		return;
	}

	QUrl url = downloadQueue.dequeue();
	bool l = false;

	QString u = url.toString();
	if(exists->contains(u)){
		++downloadedCount;
		startNextDownload();
	}else{
		if(u.startsWith("##")){
			QUrl c("https://www.deviantart.com/api/v1/oauth2/deviation/download/"+QStringRef(&u, 2, u.length() - 2).toString());
			QUrlQuery q;
			q.addQueryItem("access_token", token->token);
			c.setQuery(q);
			QJsonObject o = QJsonDocument::fromJson(token->GET(c)).object();
			url.setUrl(o.value("src").toString());
			//qDebug() << o;
			l = (o["width"].toInt() < 1000) || (o["height"].toInt() < 500);
		}

		QString filename = saveFileName(url, l);
		log->append(QString("Downloading %1/%2 to %3").arg(downloadedCount+1).arg(totalCount).arg(filename));
		output.setFileName(filename);

		if(output.exists()){
			QFile exist("links_exist.txt");
			exist.open(QFile::Append);
			exist.write(u.toUtf8() + '\n');
			exist.flush();
			exist.close();
			++downloadedCount;
			startNextDownload();
		}else{
			output.open(QFile::WriteOnly);
			QNetworkRequest request(url);
			currentDownload = manager.get(request);
			connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),	SLOT(downloadProgress(qint64,qint64)));
			connect(currentDownload, SIGNAL(finished()),			SLOT(downloadFinished()));
			connect(currentDownload, SIGNAL(readyRead()),		SLOT(downloadReadyRead()));

			downloadTime.start();
		}
	}
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal){
	p->setMaximum((int)bytesTotal);
	p->setValue((int)bytesReceived);

	// calculate the download speed
	double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
	QString unit;
	if (speed < 1024) {
		unit = "B/s";
	} else if (speed < 1024*1024) {
		speed /= 1024;
		unit = "KB/s";
	} else {
		speed /= 1024*1024;
		unit = "MB/s";
	}

	l->setText(QString("Files to download: %1; Speed: %2 %3")
		           .arg(this->downloadQueue.size())
		           .arg(speed, 3, 'f', 1)
		           .arg(unit));
}

void DownloadManager::downloadFinished(){
	if(output.isOpen())	output.close();
	downloadTime.elapsed();

	if (currentDownload->error()) {
		log->append(QString("Failed: %1").arg(currentDownload->errorString()));
	} else {
		++downloadedCount;
	}

	currentDownload->deleteLater();
	startNextDownload();
}
void DownloadManager::downloadReadyRead() {output.write(currentDownload->readAll());}