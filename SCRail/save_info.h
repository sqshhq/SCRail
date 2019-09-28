#pragma once
#ifndef SAVE_INFO_H
#define SAVE_INFO_H

#include<cstring>
typedef struct CheckStatu{
	unsigned int version;//�汾�� ����ǰ��Ĭ��Ϊ1�汾��
	bool right_left;// true: �ҹ죻 false�����
	int rail_type;//���� 0/60kg,1/75kg,2/50kg,3/43kg��4Ϊ60Dkg��5Ϊ60Nkg
	char railType[64];
	//QString checkTime = NULL;
	double current;//������
	bool wayOrStation;//�ж�����·���ǳ�վ����·Ϊtrue
	char line[64];//�б�
	char way[64];//��·��
	char station[64];//��վ
	char track[64];//�ɵ�
	float diffData[21]; //ƫ������
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
		right_left = 0;// true: �ҹ죻 false�����
		rail_type = 0;//���� 0/60kg,1/75kg,2/50kg,3/43kg��4Ϊ60Dkg��5Ϊ60Nkg
		current = 0.0;
		wayOrStation = 0;//�ж�����·���ǳ�վ����·Ϊtrue
		memset(line, 0, sizeof(line));//�б�
		memset(way, 0, sizeof(way));//��·��
		memset(railType, 0, sizeof(railType));
		memset(station, 0, sizeof(station));//��վ
		memset(track, 0, sizeof(track));//�ɵ�
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
	float data_x, data_y; //�����
}data_dot;
typedef struct checkData{
	int dotcount;// �������
	data_dot datadot[1280]; //����ֵ
	int railState;

	checkData()
	{
		dotcount = 0;
		memset(datadot, 0, sizeof(datadot));
		railState = 0;
	}
}CheckData; //��������

#endif // SAVE_INFO_H
