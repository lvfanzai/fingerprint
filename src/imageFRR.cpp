#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <unistd.h>

#include "imageFRR.h"
#include "imageCommon.h"
#include "imageIdentify.h"

/*
*初步思想
*取每个手指的文件夹中的模版数据，使用相同手指的所有图像数据进行比对，统计失败次数
*/

void doFRR(vector<string> config) {
    printf("doFRR Start\n");
    unsigned char imageIn[H*W];
    unsigned char imageOut[H*W];
    unsigned char bmpHeader[OFFSET];
    unsigned char readFeature[FEATURE_SIZE] = {0};

    char fileName[10] = {0};
    char folderName[10] = {0};
    string fileRoot = "./";
    string fileFolder;
    string fileInPath;
    string featureFileName;
    int cmpCnt = 0;
    int cmpFailed = 0;
    
    FILE *fpFrrResult;
    string fileFRR = fileRoot + config[0] + "FRR.txt";
    //以追加的方式打开结果文件
    if ((fpFrrResult = fopen(fileFRR.c_str(),"ab"))==0) {
        printf("Can not open file %s\n", fileFRR.c_str());
    }

    for (int peoCnt = 0; peoCnt < PEOPLE_NUM; peoCnt ++) {
        sprintf(folderName, "%04d%s", peoCnt, config[0].c_str());//获取config[0]文件夹中数据进行录入模版
        fileFolder = fileRoot + folderName +  "/";
        printf("%s\n",fileFolder.c_str());

        for (int fileCnt = 1; fileCnt <= FINGER_TOTAL_NUM; fileCnt ++) {
            if (fileCnt <= 3) {
                fileInPath = fileFolder + "L" + to_string(fileCnt) + "/";
            }
            else {
                fileInPath = fileFolder + "R" + to_string(fileCnt - 3) + "/";
            }

            //读取每个手指的Feature数据
            FILE *fpFeature;
            featureFileName = fileInPath + "data.feature";
            if ((fpFeature = fopen(featureFileName.c_str(),"rb"))==0) {
                printf("Can not open file %s\n", featureFileName.c_str());
            }
            fread(readFeature, 1, FEATURE_SIZE, fpFeature);
            fclose(fpFeature);

            for(int cnt = 0; cnt < FILE_NUM; cnt ++) {
                sprintf(fileName,"%04d",cnt);
                string fileNameIn = fileInPath + fileName + ".bmp";

                FILE *fpIn;
                if ((fpIn = fopen(fileNameIn.c_str(),"rb"))==0) {
                    printf("Can not open file %s\n", fileNameIn.c_str());
                    continue;
                }
                fread(bmpHeader, 1, OFFSET, fpIn);
                fseek(fpIn, OFFSET, 0);//
                fread(imageIn, 1, H*W, fpIn);
                fclose(fpIn);

                for (int k = 0;k < H; k ++) {
                    for (int j = 0;j < W; j ++){
                        imageOut[k * W + j] = imageIn[(H-k-1) * W + j];//将需要进行录入模版数据保存到临时数据中
                    }
                }
                cmpCnt ++;
                //调用认证函数，进行认证获取feature数据
                if (!doIdentify(readFeature, imageOut)) {
                    cmpFailed ++;
                    char tmpName[100] = {0};
                    string cmpFile = featureFileName + "_cmpto_" + fileNameIn;
                    sprintf(tmpName, "%s\n",cmpFile.c_str());
                    fwrite(tmpName, 1, strlen(tmpName), fpFrrResult);
                }
            }
        }
    }
    printf("cmpCnt = %d\n",cmpCnt);
    printf("cmpFailed = %d\n",cmpFailed);
    string faildRate = to_string(cmpFailed) + "_to_" + to_string(cmpCnt);
    fwrite(faildRate.c_str(), 1, faildRate.length(), fpFrrResult);
    fclose(fpFrrResult);
    printf("doFRR End\n");
    return;
}