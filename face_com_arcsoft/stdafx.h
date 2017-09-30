#include <stdint.h>
#include<iostream>
#include <Windows.h>
#include<tchar.h>
#include<vector>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<fstream>
#include<time.h>
#include "inirw.h"
#include "inc/arcsoft_fsdk_face_recognition.h"
#include "inc/merror.h"
#include "inc/arcsoft_fsdk_face_detection.h"


#pragma comment(lib,"lib/libarcsoft_fsdk_face_detection.lib")
#pragma comment(lib,"lib/libarcsoft_fsdk_face_recognition.lib")

char APPID[256] = "4iYKQo9nEDx5CzyybYxtwoWDW8soLZgoGMMMgCT9skUa";
char SDKKey1[256] = "EDS9Eco96tpxXRbEqAh3AjMSPW3ceJU7zVXxxGFCBz5h";
char SDKKey2[256] = "EDS9Eco96tpxXRbEqAh3AjMZYuJkwN2wgeTEiqBsjhUH";