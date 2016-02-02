#include "main.h"

int main(int argc, char** argv){
	QApplication a(argc, argv);
	DownloadManager wg;
	return a.exec();
}

DownloadManager::DownloadManager() :downloadedCount(0), totalCount(0){
	token = new MToken;
	exists = new QStringList;
	loadFile("links_exist.txt", *exists);

	QVBoxLayout* vl = new QVBoxLayout;
	QHBoxLayout* hl = new QHBoxLayout, *rh = new QHBoxLayout;
	rbox = new QGroupBox("Select job type:");
	rb_1 = new QRadioButton("Download all");
	rb_2 = new QRadioButton("Check all");
	rb_3 = new QRadioButton("Resort all");
	rbstart = new QPushButton("Start job");
	b_sr = new QPushButton("Start download");
	b_st = new QPushButton("Stop download");
	p = new QProgressBar;
	log = new QTextEdit;

	rb_1->setChecked(true);
	rh->addWidget(rb_1);
	rh->addWidget(rb_2);
	rh->addWidget(rb_3);
	rbox->setLayout(rh);

	setWidgetsEnabled(true);

	vl->addWidget(rbox);
	vl->addWidget(rbstart);
	vl->addSpacing(15);
	hl->addWidget(b_sr);
	hl->addWidget(b_st);
	vl->addLayout(hl);
	vl->addWidget(p);
	vl->addSpacing(15);
	vl->addWidget(log);
	this->setLayout(vl);
	connect(rbstart, SIGNAL(clicked()), this, SLOT(jobStart()));
	connect(b_sr, SIGNAL(clicked()), this, SLOT(startDownload()));
	connect(b_st, SIGNAL(clicked()), this, SLOT(stopDownload()));

	icon = QIcon(QPixmap("icon.png"));

	tmenu = new QMenu;
	setButtonsEnabled(true);

	tray = new QSystemTrayIcon(icon);
	//	tray->setContextMenu(tmenu);
	tray->show();
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)));
	qApp->setWindowIcon(icon);
	this->setWindowIcon(icon);
	this->setWindowTitle("");
	this->resize(1000, 300);
	this->show();




	this->downloadTime = new QTime;
	this->totalCount = downloadQueue.size();
}

void DownloadManager::append(QString url){
	if (downloadQueue.isEmpty())
		tryStartNextDownload();

	downloadQueue.enqueue(QUrl(url));
}

bool isPic(QString s){ return s.endsWith(".png") || s.endsWith(".jpg");}
void getDir(QString &dir, int w, int h){
	if( w == 0 || h == 0){
		dir += "dother";
	}else	if(w > h){
		dir += "dwall/";
		if(w >= 3840 && h >= 2160){
			dir += "WUHD";
		}else if(w >= 1920 && h >= 1080){
			dir += "WHD";
		}else if(w >= 1366 && h >= 768){
			dir += "HD";
		}else{
			dir += "Low";
		}
	}else{
		dir += "dart";
	}
	dir += '/';
}
QString DownloadManager::saveFileName(QJsonObject o){
	QUrl url = o.value("src").toString();
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();
	QString dir;

	int f = basename.lastIndexOf("_by_") + 4;

	if(isPic(basename) && f > 3){
		if (basename.isEmpty())
			basename = "download";

		dir = "";
		getDir(dir, o["width"].toInt(0), o["height"].toInt(0));
		if(dir.startsWith("dother")) return dir + basename;

		int d = basename.indexOf("-d");
		if(basename.indexOf("-d", f) == -1) d = basename.indexOf(".", f);

		dir += QStringRef(&basename, f, d - f).toString() + "/";
		QDir().mkpath(dir);
		return dir + basename;
	}else{
		return "dother/"+basename;
	}
}
QString DownloadManager::saveFileName(QString s) {
	QString basename = QFileInfo(s).fileName();
	QString dir;

	int f = basename.lastIndexOf("_by_") + 4;

	if(isPic(basename) && f > 3){
		if (basename.isEmpty())
			basename = "download";

		// dir
		QImage o(s);
		dir = "";
		getDir(dir, o.width(), o.height());
		if(dir.startsWith("dother")) return dir + basename;
		int d = basename.indexOf("-d");
		if(basename.indexOf("-d", f) == -1) d = basename.indexOf(".", f);

		dir += QStringRef(&basename, f, d - f).toString() + "/";
		QDir().mkpath(dir);
		return dir + basename;
	}else{
		return "dother/"+basename;
	}
}

void DownloadManager::hideEvent(QHideEvent *){this->hide();}

void DownloadManager::nextDownload(){
	if (downloadQueue.isEmpty()) {
		log->append(QString("%1/%2 files downloaded successfully").arg(downloadedCount).arg(totalCount));
		jobEnd();
		return;
	}
	this->setWindowTitle(QString("Files to download: %1").arg(this->downloadQueue.size()));

	QUrl url = downloadQueue.dequeue();
	QString u = url.toString();
	QString filename;

	if(exists->contains(u)){
		++downloadedCount;
		tryStartNextDownload();
	}else{
		QJsonObject o;
		if(u.startsWith("##")){
			QUrl c("https://www.deviantart.com/api/v1/oauth2/deviation/download/"+QStringRef(&u, 2, u.length() - 2).toString());
			QUrlQuery q;
			q.addQueryItem("access_token", token->token);
			c.setQuery(q);
			o = QJsonDocument::fromJson(token->GET(c)).object();
			url = QUrl(o["src"].toString());
			filename = saveFileName(o);
		}else{
			filename = "dother2/"+url.fileName();
		}


		output.setFileName(filename);

		if(output.exists()) {
			QFile exist("links_exist.txt");
			exist.open(QFile::Append);
			exist.write(u.toUtf8() + '\n');
			exist.flush();
			exist.close();
			++downloadedCount;
			tryStartNextDownload();
			return;
		}

		log->append(QString("Downloading %1/%2 to %3").arg(downloadedCount+1).arg(totalCount).arg(filename));
		output.open(QFile::WriteOnly);
		QNetworkRequest request(url);
		currentDownload = manager.get(request);
		connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),	SLOT(downloadProgress(qint64,qint64)));
		connect(currentDownload, SIGNAL(finished()),			SLOT(downloadFinished()));
		connect(currentDownload, SIGNAL(readyRead()),		SLOT(downloadReadyRead()));

		downloadTime->start();
	}
}
void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal){
	p->setMaximum((int)bytesTotal);
	p->setValue((int)bytesReceived);

	// calculate the download speed
	double speed = bytesReceived * 1000.0 / downloadTime->elapsed();
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

	this->setWindowTitle(QString("Files to download: %1; Speed: %2 %3")
																						.arg(this->downloadQueue.size())
																						.arg(speed, 3, 'f', 1)
																						.arg(unit));
}
void DownloadManager::downloadFinished(){
	if(output.isOpen())	output.close();
	downloadTime->elapsed();

	if (currentDownload->error()) {
		log->append(QString("Failed: %1").arg(currentDownload->errorString()));
	} else {
		++downloadedCount;
	}

	currentDownload->deleteLater();
	tryStartNextDownload();
}
void DownloadManager::downloadReadyRead() {output.write(currentDownload->readAll());}

void DownloadManager::trayClick(QSystemTrayIcon::ActivationReason reason){
	if(reason == QSystemTrayIcon::Trigger){
		if (!isVisible()){
			show();
			if (isMinimized()) showNormal();
			this->show();
			qApp->setActiveWindow(this);
		}else{
			this->hide();
		}
	}
}

void DownloadManager::checkFiles(){
	downloadTime = new QTime(QTime::currentTime());
	ex.setFileName("c_exists.txt");
	ws.setFileName("c_wrong.txt");
	de.setFileName("c_todown.txt");

	if(ex.exists()){ ex.open(QFile::Append); loadFile("c_exists.txt", lex);}else ex.open(QFile::WriteOnly);
	if(ws.exists()){ ws.open(QFile::Append); loadFile("c_wrong.txt",  lws);}else ws.open(QFile::WriteOnly);
	if(de.exists()){ de.open(QFile::Append); loadFile("c_todown.txt", lde);}else de.open(QFile::WriteOnly);

	downloadedCount = 0;
	QStringList devs;
	loadFile("linksF.txt", devs);
	for(QString u : devs)
		this->downloadQueue.append(QUrl(u));
	totalCount = devs.size();
	tryStartNextFile();

	jobEnd();
}

void DownloadManager::checkNextFile(){
	if(downloadQueue.empty()){
		ex.flush();
		ws.flush();
		de.flush();
		ex.close();
		ws.close();
		de.close();
	}

	QJsonObject o;
	QString filename;
	QString u = downloadQueue.dequeue().toString();
	downloadedCount++;

	if(lex.contains(u) || lws.contains(u) || lde.contains(u)) {
		this->setWindowTitle(QString("Left: %1").arg(totalCount - downloadedCount));
		if(running)
			QTimer::singleShot(0, this, SLOT(checkNextFile()));
		else {
			setButtonsEnabled(false);
			log->append("Check stopped");
		}
		return;
	}
	if(token->isErrors()) {log->append("Too many requets, stopped"); stopDownload(); return;}

	QUrl c("https://www.deviantart.com/api/v1/oauth2/deviation/download/"+QStringRef(&u, 2, u.length() - 2).toString());
	QUrlQuery q;
	q.addQueryItem("access_token", token->token);
	c.setQuery(q);
	o = QJsonDocument::fromJson(token->GET(c)).object();
	filename = saveFileName(o);

	output.setFileName(filename);

	if(output.exists()) {
		if(output.size() != o["filesize"].toInt(0)){
			ws.write(u.toUtf8()+'\n');iws++;
		}else{
			ex.write(u.toUtf8()+'\n');iex++;
		}
	}else{
		de.write(u.toUtf8()+'\n');ide++;
	}
	if(downloadedCount % 10 == 0)
		this->log->append(QString("Checked: %1 (To download: %2; Wrong size: %3; Exists: %4)").arg(downloadedCount).arg(ide).arg(iws).arg(iex));
	printTime(downloadedCount, totalCount);

	if(downloadedCount % 100 == 0){
		ex.flush();
		ws.flush();
		de.flush();
	}
	tryStartNextFile();
}

void DownloadManager::tryStartNextFile(){
	if(running)
		QTimer::singleShot(100, this, SLOT(checkNextFile()));
	else {
		setButtonsEnabled(false);
		log->append("Check stopped");
	}
}

void DownloadManager::moveFiles(){
	downloadTime = new QTime;

	QStringList list = QDir("down_tosort/").entryList({"*.jpg", "*.png"}, QDir::Files, QDir::Name);int i = 0;
	p->setMaximum(list.size());
	log->append(QString("moving %1 files").arg(list.size()));
	for(QString s : list){
		p->setValue(i++);
		if(i % 10 == 0) log->append(QString("%1/%2 moved").arg(i).arg(list.size()));
		printTime(i, list.size());

		QFile f("down_tosort/"+s);
		f.open(QFile::ReadOnly);
		QByteArray a = f.readAll();
		f.close();

		f.setFileName(saveFileName("down_tosort/"+s));
		f.open(QFile::WriteOnly);
		f.write(a);
		f.flush();
		f.close();

		qApp->processEvents();
	}
	log->append("Resorting finished");

	jobEnd();
}

void DownloadManager::printTime(int i, int t){
	float sec = downloadTime->secsTo(QTime::currentTime());
	float speed = i/ (sec/60);
	int secs = (sec * t) / i;
	int hr = secs/3600, mn = (secs%3600)/60, sc = secs - hr*3600 - mn*60;

	this->setWindowTitle(QString("Left: %1; Speed: %2 e/m; End in %3:%4:%5")
																						.arg( t - i )
																						.arg(speed, 3, 'f', 2)
																						.arg(hr, 2)
																						.arg(mn, 2)
																						.arg(sc, 2)
																						);
}

void DownloadManager::stopDownload() {
	this->running = false;
}

void DownloadManager::startDownload() {
	this->running = true;
	setButtonsEnabled(true);
	switch(jobType){
	case 1: tryStartNextDownload();break;
	case 2: tryStartNextFile(); break;
	case 3:
	case 0:
	default: break;
	}

}

void DownloadManager::tryStartNextDownload() {
	if(running)
		QTimer::singleShot(0, this, SLOT(nextDownload()));
	else {
		setButtonsEnabled(false);
		log->append("Download stopped");
	}
}

void DownloadManager::setButtonsEnabled(bool f){
	b_sr->setEnabled(!f);
	b_st->setEnabled(f);
}

void DownloadManager::setWidgetsEnabled(bool f){
	rbox->setEnabled(f);
	rb_1->setEnabled(f);
	rb_2->setEnabled(f);
	rb_3->setEnabled(f);
	rbstart->setEnabled(f);
	rbox->setVisible(f);

	b_sr->setEnabled(!f);
	b_st->setEnabled(!f);
	p->setEnabled(!f);
}

void DownloadManager::jobStart(){
	setWidgetsEnabled(false);
	if(rb_1->isChecked()){
		jobType = 1;
		QStringList links;
		loadFile("linksF.txt", links);
		for(QString s : links)
			append(s);

	}else if(rb_2->isChecked()){
		jobType = 2;
		checkFiles();
	}else if(rb_3->isChecked()){
		jobType = 3;
		moveFiles();
	}else{
		jobType = 0;
		setWidgetsEnabled(true);
	}
}

void DownloadManager::jobEnd(){
	jobType = 0;
	setWidgetsEnabled(true);
}
