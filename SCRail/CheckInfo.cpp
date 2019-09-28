#include "CheckInfo.h"
#include<string.h>


CheckInfo::CheckInfo(): _lineName(0),
	  _rowDiff(0),
	  _station(0),
	  _track(0),
	  _checkLocal(false),
	  _verticalWear(0),
	_targetOversizeIndex(0),
	  _sideWear(0),
	  _GQIValue(0),
	  _maxDiff(0),
	  _minDiff(0)
{
	memset(_mileage, 0, sizeof(_mileage));
	_targetOversize = "";
}


CheckInfo::~CheckInfo()
{
}
