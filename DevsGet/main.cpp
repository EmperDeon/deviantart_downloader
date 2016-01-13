#include "main.h"

MToken* token;

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	token = new MToken;

	QMap<QString, QString> folders;
	QStringList devs, links, llinks;

	loadMFile("folders.txt", folders);int i = 0;
	for(QString s : folders.keys()){
		int of = 0;
		do{
			QUrl c("https://www.deviantart.com/api/v1/oauth2/gallery/" + s);
			QUrlQuery q;
			q.addQueryItem("access_token", token->token);
			q.addQueryItem("username", folders.value(s));
			q.addQueryItem("mode", "newest");
			q.addQueryItem("offset", QString::number(of));
			c.setQuery(q);
			QJsonObject o = QJsonDocument::fromJson(token->GET(c)).object();
			for(QJsonValue v : o.value("results").toArray()){
				QJsonObject t;
				if(v.toObject().value("is_downloadable").toBool(false)){
					devs << v.toObject().value("deviationid").toString();
				}else{
					t = v.toObject().value("content").toObject();
					if((t["height"].toInt() > 500) && (t["width"].toInt() > 1000))
						links << t.value("src").toString();
					else
						llinks << t.value("src").toString();
				}
			}
			qDebug() << QString("%1/%2 : %3").arg(i).arg(folders.size()).arg(of);
			if(!o.value("has_more").toBool()) break;
			of += 10;
		}while(true);
		i++;
	}

	qSort(devs);
	qSort(links);
	qSort(llinks);

	saveFile("nlinksF_1.txt", devs);
	saveFile("nlinks.txt", links);
	saveFile("nlinks_low.txt", llinks);
	return 0;
}
