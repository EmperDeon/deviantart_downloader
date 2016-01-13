#include <QtCore>
#include <mtoken.h>
int main(){
	QStringList links;
	QStringList exist;

	loadFile("links_exist.txt", exist);
	loadFile("linksF.txt", links, exist);
	saveFile("linksF.txt", links);

	return 0;
}