#include <windows.h>


int main(){
	system("copy D:\\GameTmp\\dev\\Sorter.exe Sorter.exe /Y");
	system("copy D:\\GameTmp\\dev\\FoldersGet.exe FoldersGet.exe /Y");
	system("copy D:\\GameTmp\\dev\\DevsGet.exe DevsGet.exe /Y");
	system("copy D:\\GameTmp\\dev\\Download.exe Download.exe /Y");

	system("Download.exe");
	return 0;
}