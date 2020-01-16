#include <vector>
#include <string>
#include <map>


using namespace std;

void FileStorageInit();

class FileStorageManager{
public:
	FileStorageManager(void);
	~FileStorageManager();
private:
	int nKey;
	wstring m_wstrSharedPath;
	map<string, string> m_mapSharedList;				// Key, FilePath
public:
	void CollectFileList();
	void CreateUniqueKey();
	void SplitBuckets();
	void InsertFileToList(string strFileName, string strFullPath);
	BOOL HasFile(string strFileName, string &strFullPath);

	void Print();
};