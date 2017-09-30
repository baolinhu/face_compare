# Face_Compare

基于身份证的人脸对比，使用linkface的api。对应的[CSDN博客在这里](http://blog.csdn.net/baolinq/article/details/77140574)

input：两张人脸照片，可以是直接拍照的，也可以是身份证或者学生证图片等

functions：
 * face detect 人脸检测
 * face features detect 人脸特征点检测，43个点
 * face compare 人脸对比

output：
* 人脸检测 输出性别、年龄、人脸中心点坐标、人脸尺寸等信息
* 人脸特征点检测 标记和定位出人脸的43个特征点
* 输出两张人脸的相似度，百分数显示，越大相似度越高

作者信息 hu_nobuone@163.com 

软件版本：v1.0  Copyright(C)  2017  KB535

时间 2017.8.9
