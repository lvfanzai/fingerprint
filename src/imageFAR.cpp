#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <unistd.h>

#include "imageFAR.h"
#include "imageCommon.h"
#include "imageIdentify.h"

/*
*初步思想
*取每个手指的文件夹中的模版数据，使用其他手指处理后的数据进行比对统计对比成功的次数
*/
void doFAR(vector<string> config) {
    printf("doFAR Start\n");
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
    int cmpSucc = 0;
    
    FILE *fpFarResult;
    string fileFAR = fileRoot + config[0] + "FAR.txt";
    //以追加的方式打开结果文件
    if ((fpFarResult = fopen(fileFAR.c_str(),"ab"))==0) {
        printf("Can not open file %s\n", fileFAR.c_str());
    }

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

            //读取每个手指的Feature数据
            FILE *fpFeature;
            featureFileName = fileInPath + "data.feature";
            if ((fpFeature = fopen(featureFileName.c_str(),"rb"))==0) {
                printf("Can not open file %s\n", featureFileName.c_str());
            }
            fread(readFeature, 1, FEATURE_SIZE, fpFeature);
            fclose(fpFeature);

            //将读到当前的feature数据与其他手指指纹进行对比
            for (int peoCntCmp = 0; peoCntCmp < PEOPLE_NUM; peoCntCmp ++) {
                sprintf(folderName, "%04d%s", peoCntCmp, config[0].c_str());//获取POST文件夹中数据进行录入模版
                fileFolder = fileRoot + folderName +  "/";
                printf("%s\n",fileFolder.c_str());

                for (int fileCntCmp = 1; fileCntCmp <= FINGER_TOTAL_NUM; fileCntCmp ++) {
                    if (fileCntCmp <= 3) {
                        fileInPath = fileFolder + "L" + to_string(fileCntCmp) + "/";
                    }
                    else {
                        fileInPath = fileFolder + "R" + to_string(fileCntCmp - 3) + "/";
                    }
                
                    //如果当前读到的feature与需要对比的文件夹数据相同，则不比对，因为为同一手指，对比FA无意义
                    if((fileCntCmp == fileCnt)&&(peoCnt == peoCntCmp)) {
                        continue;
                    }

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
                        if (doIdentify(readFeature, imageOut)) {
                            cmpSucc ++;
                            char tmpName[100] = {0};
                            string cmpFile = featureFileName + "_cmpto_" + fileNameIn;
                            sprintf(tmpName, "%s\n",cmpFile.c_str());
                            fwrite(tmpName, 1, strlen(tmpName), fpFarResult);
                        }
                    }
                }
            }
        }
    }
    printf("cmpCnt = %d\n",cmpCnt);
    printf("cmpSucc = %d\n",cmpSucc);
    string faildRate = to_string(cmpSucc) + "_to_" + to_string(cmpCnt);
    fwrite(faildRate.c_str(), 1, faildRate.length(), fpFarResult);
    fclose(fpFarResult);
    printf("doFAR End\n");
    return;
}