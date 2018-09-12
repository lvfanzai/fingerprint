# 工程简介
本工程兼容性比较差，仅仅针对采集数据为如下格式的数据集
其中N为采集人员编号，可以为多人，一般从0开始例如0000，0001，0002， 0003
000N
|-----L1
|       |----0000.bmp
|       |----0001.bmp
|       |----……
|       |----0068.bmp
|       |----0069.bmp
|-----L2
|       |----0000.bmp
|       |----0001.bmp
|       |----……
|       |----0068.bmp
|       |----0069.bmp
|-----L3
|       |----0000.bmp
|       |----0001.bmp
|       |----……
|       |----0068.bmp
|       |----0069.bmp
|-----R1
|       |----0000.bmp
|       |----0001.bmp
|       |----……
|       |----0068.bmp
|       |----0069.bmp
|-----R2
|       |----0000.bmp
|       |----0001.bmp
|       |----……
|       |----0068.bmp
|       |----0069.bmp
|-----R3
|       |----0000.bmp
|       |----0001.bmp
|       |----……
|       |----0068.bmp
|       |----0069.bmp



## 代码架构

-1. config.csv文件，保存算法中可能需要遍历的参数

0. imageCommon.h中参数，主要设置一些常数
PEOPLE_NUM：当前采集人数，也就是数据文件夹个数，当前设置为10，根据实际进行修改
FINGER_TOTAL_NUM：每个人采集的手指数目，当前设置为6，左手三根手指，右手三根手指
FILE_NUM：表示当前每个手指采集的指纹图像个数，当前设置为70
OFFSET：bmp格式图片，header偏移，固定为1078
FEATURE_SIZE：模版数据大小，当前设定为10000
MULTI_NUM:多帧融合张数

1. main.cpp为程序主要入口：遍历csv中算法参数和阈值，传入到图像预处理模块，录入模块，FRR模块，FAR模块，筛选出最适合的算法参数

2. imagePreprocess.cpp 为图像预处理模块，读取数据文件中指纹数据，调用imageHence.cpp中doHence函数进行处理。
imageHence.cpp为图像增强模块

3. imageEnroll.cpp模块
根据ENROLL_NEED设定数据，选择单个手指ENROLL_NEED张图片，进行feature抽取，并保存到对应手指文件夹下data.feature文件中

4. imageFRR.cpp模块
根据每个手指录入的模版数据data.feature，统计计算FRR

5. imageFAR.cpp模块
根据每个手指录入的模版数据data.feature，进行识别统计计算FAR数据

6. imageIdentify.cpp模块
识别模块

## 开发环境
需要安装opencv
mac 下安装brew install opencv

将头文件路径设置为环境变量，在.bash_profile中添加
export PATH=/usr/local/opt/opencv@3/include/:$PATH


## 需要开发的部分
1.imageHence.cpp中doHence函数，图像增强

2.imageEnroll.cpp中doEnroll函数，抽feature

3.imageIdentify.cpp中，doIdentify函数，对比

4.其他需要模块可以自己需求增加

## 需要开发的部分
增加完成之后，运行./compile_run.sh即可得到当前算法的结果


