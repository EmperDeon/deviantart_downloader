#include "mtoken.h"

MToken::MToken() {
	manager = new QNetworkAccessManager;
	managerT = new QNetworkAccessManager;
	tokent = new QTimer();
	getToken();
	connect(tokent, SIGNAL(timeout()), this, SLOT(upd()));
	tokent->start(30000);
}

QByteArray MToken::GET(QUrl r) {
	QNetworkReply* reply = manager->get(QNetworkRequest (r));
	QEventLoop wait;
	connect(manager, SIGNAL(finished(QNetworkReply*)),&wait, SLOT(quit()));
	QTimer::singleShot(10000, &wait,SLOT(quit()));
	wait.exec();
	QByteArray answer = reply->readAll();
	reply->deleteLater();
	return answer;
}

QByteArray MToken::GETt(QUrl r) {
	QNetworkReply* reply = managerT->get(QNetworkRequest (r));
	QEventLoop wait;
	connect(managerT, SIGNAL(finished(QNetworkReply*)),&wait, SLOT(quit()));
	QTimer::singleShot(10000, &wait,SLOT(quit()));
	wait.exec();
	QByteArray answer = reply->readAll();
	reply->deleteLater();
	return answer;
}

void MToken::getToken() {
	QUrl current("https://www.deviantart.com/oauth2/token");
	QUrlQuery par;
	par.addQueryItem("grant_type", "client_credentials");
	QStringList ids;
	loadFile("backup/secret.txt", ids);
	// Add your id and secret to run
	par.addQueryItem("client_id", ids[0]);
	par.addQueryItem("client_secret", ids[1]);
	current.setQuery(par);
	token = QJsonDocument::fromJson(GETt(current)).object().value("access_token").toString();
	qDebug() << "Token: " << token;
}

bool MToken::testToken() {
	QUrl current("https://www.deviantart.com/api/v1/oauth2/placebo");
	QUrlQuery par;
	par.addQueryItem("access_token", token);
	current.setQuery(par);
	QByteArray o = GETt(current);
	qDebug() << "Status:" << o;
	return QJsonDocument::fromJson(o).object().value("status").toString() == "success";
}

void MToken::upd() { if(!testToken()) getToken();}


void loadFile(QString f, QStringList& list){
	QFile i(f);QString t;
	i.open(QFile::ReadOnly | QFile::Text);
	while(!i.atEnd()){
		t = i.readLine();
		t = QStringRef(&t, 0, t.length()-1).toString();
		list << t;
	}
	i.close();
}

void loadMFile(QString f, QMap<QString, QString>& map){
	QFile i(f);QString t, n;
	i.open(QFile::ReadOnly | QFile::Text);
	while(!i.atEnd()){
		t = i.readLine();
		t = QStringRef(&t, 0, t.length()-1).toString();
		n = QStringRef(&t, t.indexOf(" ")+1, t.length()-t.indexOf(" ")).toString();
		t = QStringRef(&t, 0, t.indexOf(" ")-1).toString();

		map.insert(t, n);
	}
	i.close();
}

void loadFile(QString f, QStringList& list, QStringList check){
	QFile i(f);QString t;
	i.open(QFile::ReadOnly | QFile::Text);
	while(!i.atEnd()){
		t = i.readLine();
		t = QStringRef(&t, 0, t.length()-1).toString();
		if(!check.contains(t))
			list << t;
	}
	i.close();
}

void saveFile(QString f, QStringList list){
	QFile i(f);
	i.open(QFile::WriteOnly | QFile::Text);
	for(QString s : list)
		i.write(s.toUtf8() + "\n");
	i.close();
}
