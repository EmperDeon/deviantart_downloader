#include "main.h"

MToken* token;

int main(int argc, char** argv){
	QCoreApplication app(argc, argv);
	token = new MToken;

	QStringList users, folders;
	QUrlQuery q;
	q.addQueryItem("access_token", token->token);
	q.addQueryItem("ext_galleries", "true");

	loadFile("users.txt", users);int i = 0;
	for(QString s : users){
		QUrl c("https://www.deviantart.com/api/v1/oauth2/user/profile/" + s);

		c.setQuery(q);
		QJsonObject o = QJsonDocument::fromJson(token->GET(c)).object();
		for(QJsonValue v : o.value("galleries").toArray()){
			QJsonObject t = v.toObject();
			if(t["parent"].isNull()){
				folders << t["folderid"].toString() + ' ' + s;
			}
		}
		qDebug() << QString("%1/%2").arg(i++).arg(users.size());
	}
	qSort(folders);
 saveFile("folders.txt", folders);
	return 0;
}
