#pragma once
#ifndef SAVE_INFO_H
#define SAVE_INFO_H

#include<cstring>
typedef struct CheckStatu{
	unsigned int version;//版本号 （当前是默认为1版本）
	bool right_left;// true: 右轨； false：左轨
	int rail_type;//轨型 0/60kg,1/75kg,2/50kg,3/43kg，4为60Dkg，5为60Nkg
	char railType[64];
	//QString checkTime = NULL;
	double current;//检测里程
	bool wayOrStation;//判断是线路还是车站，线路为true
	char line[64];//行别
	char way[64];//线路名
	char station[64];//车站
	char track[64];//股道
	float diffData[21]; //偏差数据
	float offsetX[66];
	float offsetY[66];
	double verticalHalf_wear;
	double verticalThirds_wear;
	double horizontal_wear;
	int GQI_value;
	double max_offset;
	double min_offset;
	char cantValue[20];

	CheckStatu()
	{
		version = 0;
		right_left = 0;// true: 右轨； false：左轨
		rail_type = 0;//轨型 0/60kg,1/75kg,2/50kg,3/43kg，4为60Dkg，5为60Nkg
		current = 0.0;
		wayOrStation = 0;//判断是线路还是车站，线路为true
		memset(line, 0, sizeof(line));//行别
		memset(way, 0, sizeof(way));//线路名
		memset(railType, 0, sizeof(railType));
		memset(station, 0, sizeof(station));//车站
		memset(track, 0, sizeof(track));//股道
		memset(diffData, 0, sizeof(diffData));
		memset(offsetX, 0, sizeof(offsetX));
		memset(offsetY, 0, sizeof(offsetX));
		memset(cantValue, 0, sizeof(cantValue));
		verticalHalf_wear = 0.0;
		verticalThirds_wear = 0.0;
		horizontal_wear = 0.0;
		GQI_value = 0;
		max_offset = 0.0;
		min_offset = 0.0;
	}
}CheckStatus;


typedef struct {
	float data_x, data_y; //坐标点
}data_dot;
typedef struct checkData{
	int dotcount;// 坐标点数
	data_dot datadot[1280]; //数据值
	int railState;

	checkData()
	{
		dotcount = 0;
		memset(datadot, 0, sizeof(datadot));
		railState = 0;
	}
}CheckData; //保存数据

#endif // SAVE_INFO_H
