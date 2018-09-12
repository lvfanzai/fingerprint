#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "imageCommon.h"
#include "imageHence.h"
#include "imagePreprocess.h"

using namespace std;

void cvt2B(vector<unsigned char >& data, int w, int h, vector<unsigned char >& img) {
    uint8_t* dst = reinterpret_cast<uint8_t*>(img.data());
    uint8_t* src = reinterpret_cast<uint8_t*>(data.data());

    double sum = 0, max_val = 0;
    for (int i = 0; i < w * h; ++i) {
        sum += src[i];
        if (max_val < src[i])
            max_val = src[i];
    }
    
    double avg = sum / (max_val* w * h);
    double accum = 0.;
    for (int i = 0; i < w * h; ++i)
        accum += (avg - src[i] / max_val) * (avg - src[i] / max_val);

    double stdev = sqrt(accum / (w * h - 1));
    double e_value = std::max(std::exp(-6), stdev);
    for (int i = 0; i < w * h; ++i) {
        double d = src[i] / max_val;
        double result = 0.6 * d + 0.4 * ((d - avg) / e_value + 2) / 4;
        result = result < 0 ? 0 : (result > 1 ? 1 : result);
        dst[i] = (uint8_t)(int(result * 255));
    }
}

int imagePreprocess(vector<string> config) {
    unsigned char imageIn[H*W];
    unsigned char imageOutRaw[H*W];
	unsigned char imageOutBmp[H*W];
    unsigned char bmpHeader[OFFSET];
    unsigned char saveData[FILE_NUM][H*W];
    unsigned char NorData[FILE_NUM][H*W];
    double sumData[H*W];

    char fileName[10] = {0};
    char folderName[10] = {0};
    string fileRoot = "./";
    string fileFolder;
    string fileInPath;
    string fileMeanBmpPath;
    string filePostBmpPath;
    int fileExactNum = 0;
    int cntHence = 0;
        
    for (int peoCnt = 0; peoCnt < PEOPLE_NUM; peoCnt ++) {
        sprintf(folderName,"%04d",peoCnt);
        fileFolder = fileRoot + folderName + "/";

        for (int fileCnt = 1; fileCnt <= FINGER_TOTAL_NUM; fileCnt ++) {
            if (fileCnt <= 3) {
                fileInPath = fileFolder + "L" + to_string(fileCnt) + "/";
                //处理之后存图位置
                fileMeanBmpPath = fileRoot + folderName + "_MEAN/" + "L" + to_string(fileCnt) + "/";//原图-加权求平均数据存图
                filePostBmpPath = fileRoot + folderName + config[0] + "/L" + to_string(fileCnt) + "/";//后处理保存图
            }
            else {
                fileInPath = fileFolder + "R" + to_string(fileCnt - 3) + "/";
                //处理之后存图位置
                fileMeanBmpPath = fileRoot + folderName + "_MEAN/" + "R" + to_string(fileCnt - 3) + "/";//原图-加权求平均数据存图
                filePostBmpPath = fileRoot + folderName + config[0] + "/R" + to_string(fileCnt - 3) + "/";//后处理保存图
            }

            if (access(fileMeanBmpPath.c_str(), 0) == -1){
                printf("fileMeanBmpPath = %s\n",fileMeanBmpPath.c_str());  
                MKDIRS(fileMeanBmpPath);      
            } 

            if (access(filePostBmpPath.c_str(), 0) == -1) {
                printf("fileOutBmpPath = %s\n",filePostBmpPath.c_str());  
                MKDIRS(filePostBmpPath);      
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
                        imageOutRaw[k * W + j]=imageIn[(H-k-1) * W + j];
                        saveData[cnt][k * W + j] = imageIn[(H-k-1) * W + j];//将每个文件夹中图像数据先保存起来
                    }
                }

                cntHence ++;

                std::vector<unsigned char > inputRaw(imageOutRaw, imageOutRaw + H*W);
                std::vector<unsigned char > outputRaw(imageOutBmp, imageOutBmp + H*W);
                cvt2B(inputRaw, W, H, outputRaw);
                memcpy(&NorData[cnt], &imageOutRaw[0], H*W); 
                fileExactNum = cnt;      
            }

            //直接对RAW进行加权求平均
            for(int k = 0; k < H*W; k++) {
                for(int j = 0; j <= fileExactNum ;j++) {
                    sumData[k]+= saveData[j][k];//对每个像素点进行求和
                }
                sumData[k] = sumData[k]/fileExactNum;//对像素点值加权求平均
            }

            //使用每张图减去噪声值，保存对应数据bmp图
            unsigned char temp[H*W];
            for (int m = 0; m <= fileExactNum; m ++) {
                for (int i = 0; i < H * W; i++) {
                    temp[i] = saveData[m][i] - sumData[i]; //计算RAW图-平均底噪声图,需要送给cvProcess处理
                }

                sprintf(fileName, "%04d", m);
                //保存减去平均base的图
                string fileMeanBmp = fileMeanBmpPath + fileName + ".bmp";//保存减去平均值的图
                FILE *fpOutBmptmp;
                if ((fpOutBmptmp = fopen(fileMeanBmp.c_str(), "wb"))==0) {
                    return -1;
                }
                fwrite(bmpHeader, 1, OFFSET, fpOutBmptmp);
                fseek(fpOutBmptmp, OFFSET, 0);//
                fwrite(temp, 1, H*W, fpOutBmptmp);
                fclose(fpOutBmptmp); 
                //end 保存减去平均base的图

                //进行后处理，多帧融合处理
                vector<unsigned char> inputData(temp, temp + H*W);
                vector<vector<unsigned char> > multiData;

                multiData.push_back(inputData);

                if((m+1) % MULTI_NUM == 0) {
                    vector<unsigned char> outputBuffer;
                    
                    doHence(multiData, outputBuffer);//后处理

                    string filePostBmp = filePostBmpPath + fileName + ".bmp";//保存各种处理之后的图
                    FILE *fpPostOutBmptmp;
                    if ((fpPostOutBmptmp = fopen(filePostBmp.c_str(), "wb"))==0) {
                        return -1;
                    }

                    fwrite(bmpHeader, 1, OFFSET, fpPostOutBmptmp);
                    fseek(fpPostOutBmptmp, OFFSET, 0);//
                    fwrite(&outputBuffer[0], 1, H*W, fpPostOutBmptmp);
                    fclose(fpPostOutBmptmp); 
                    vector<vector<unsigned char> >(multiData).swap(multiData);
                }
            }
        }
    }
    printf("cntHence = %d\n",cntHence);
    return 0;
}