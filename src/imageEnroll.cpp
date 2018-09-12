#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <unistd.h>

#include "imageCommon.h"
#include "imageEnroll.h"

#define ENROLL_NEED 8

/*
*初步思想
*取每个手指的前ENROLL_NEED张图像进行模版录入，将Feature保存到对应手指文件夹
*/
static void Enroll(unsigned char inData[][H*W],unsigned char* outData) {
    printf("Enroll Start\n");
}

void doEnroll(vector<string> config) {
    printf("doEnroll Start\n");
    unsigned char imageIn[H*W];
    unsigned char bmpHeader[OFFSET];
    unsigned char saveData[ENROLL_NEED][H*W];

    char fileName[10] = {0};
    char folderName[10] = {0};
    string fileRoot = "./";
    string fileFolder;
    string fileInPath;
    int featureCnt = 0;
        
    for (int peoCnt = 0; peoCnt < PEOPLE_NUM; peoCnt ++) {
        sprintf(folderName, "%04d%s", peoCnt, config[0].c_str());//获取POST文件夹中数据进行录入模版
        fileFolder = fileRoot + folderName +  "/";
        printf("%s\n",fileFolder.c_str());

        for (int fileCnt = 1; fileCnt <= FINGER_TOTAL_NUM; fileCnt ++) {
            if (fileCnt <= 3) {
                fileInPath = fileFolder + "L" + to_string(fileCnt) + "/";
            }
            else {
                fileInPath = fileFolder + "R" + to_string(fileCnt - 3) + "/";
            }

            for(int cnt = 0; cnt < ENROLL_NEED; cnt ++) {
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
                        saveData[cnt][k * W + j] = imageIn[(H-k-1) * W + j];//将需要进行录入模版数据保存到临时数据中
                    }
                }
            }

            //调用录入函数，获取feature数据
            unsigned char featureData[FEATURE_SIZE] = {0};
            Enroll(saveData, featureData);
            featureCnt ++;

            //保存feature数据
            FILE *fpOutFeature;
            string featureName = fileInPath  + "data.feature";
            printf("%s\n",featureName.c_str());
            if ((fpOutFeature = fopen(featureName.c_str(),"wb"))==0) {
                printf("Can not open output file %s\n", featureName.c_str());
                return;
            }

            fwrite(featureData,1,FEATURE_SIZE,fpOutFeature);
            fclose(fpOutFeature);
        }
    }
    printf("featureCnt = %d\n",featureCnt);
    printf("doEnroll End\n");
    return;
}