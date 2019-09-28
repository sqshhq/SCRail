#pragma once
#ifndef __CHECKINFO__
#define __CHECKINFO__
#include<QString>
class CheckInfo
{
public:
	CheckInfo();
	~CheckInfo();
public:
	int _lineName;
	int _rowDiff;
	int _station;
	int _track;
	int _mileage[7];
	bool _checkLocal;
	QString _targetOversize;
	int _targetOversizeIndex;
	int _verticalWear;
	int _sideWear;
	int _GQIValue;
	int _maxDiff;
	int _minDiff;
};

#endif

