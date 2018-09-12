#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "imageCommon.h"

using namespace std;

int MKDIRS(string dir_path){
    int path_len = static_cast<int>(dir_path.length());
    if (path_len>MAX_PATH_LEN){
        return -1;
    }
    char tmp_dir_path[MAX_PATH_LEN]={0};
    for (int i = 0; i < path_len; ++i) {
        tmp_dir_path[i] = dir_path[i];
        if (dir_path[i] =='/' || dir_path[i] == '\\') {
            if (access(tmp_dir_path,F_OK) !=0){
                MKDIR(tmp_dir_path);
            }
        }
    }
    return 0;
}

int getCsvData(string csvFile, vector<vector<string> > &strArray) { 
	// 读文件
	ifstream inFile(csvFile.c_str(), ios::in);
	string lineStr;
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
		// cout << lineStr << endl;
		// 存成二维表结构
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		// 按照逗号分隔
		while (getline(ss, str, ',')) {
			lineArray.push_back(str);
		}
		strArray.push_back(lineArray);
	}
	return strArray.size() - 1; //返回总共有多少组不同的参数
}