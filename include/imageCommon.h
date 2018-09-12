#ifndef IMAGE_COMMON_H
#define IMAGE_COMMON_H

#include <iostream>
#include <stdio.h>

using namespace std;

#define H 208
#define W 160

#define OFFSET 1078
#define FILE_NUM 70
#define PEOPLE_NUM 10
#define FINGER_TOTAL_NUM 6
#define FEATURE_SIZE 10000
#define MULTI_NUM 1

#define MAX_PATH_LEN 256
#define MKDIR(path) mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

int MKDIRS(string dir_path);
int getCsvData(string csvFile, vector<vector<string> > &strArray);

#endif