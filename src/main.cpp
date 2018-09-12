#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "imageCommon.h"
#include "imagePreprocess.h"
#include "imageHence.h"
#include "imageEnroll.h"
#include "imageFRR.h"
#include "imageFAR.h"

using namespace std;
const string cfgFile = "paramCfg.csv";
int main(int argc, char** argv) {
    
    vector<vector<string> > paramArray;

    int cfgNum = getCsvData(cfgFile, paramArray);

    //遍历配置参数，寻找最佳FARR效果参数和门限阈值
    for(int i = 1; i <= cfgNum; i ++) {
        printf("%s\n", paramArray[i][1].c_str());
        // //1.图像预处理
        // imagePreprocess(paramArray[i]);

        // //2.录入模版
        // doEnroll(paramArray[i]);

        // //3.FRR测试
        // doFRR(paramArray[i]);

        // //4.FAR测试
        // doFAR(paramArray[i]);
    }
    
    return 0;
}