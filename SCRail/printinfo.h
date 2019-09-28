#ifndef PRINTINFO_H
#define PRINTINFO_H

#include <QDialog>

namespace Ui {
class PrintInfo;
}

class PrintInfo : public QDialog
{
    Q_OBJECT

public:
	explicit PrintInfo(QWidget *parent, QString name,QString type, QString typeName, QString track,QString oversize, QString mileage,int oversizeStat, QString halfWear, QString thirdsWear, QString horWear, QString totalWear,QString maxOffset,QString minOffset,QString GQI,QString *path,float *diff);
    ~PrintInfo();

private:
    Ui::PrintInfo *ui;
	QString _lineName;
	QString _type;
	QString _typeName;
	QString _track;
	QString _oversize;
	QString _checkMileage;
	int _oversizeState;
	QString _verticalHalfWear;
	QString _verticalThirdsWear;
	QString _horizontalWear;
	QString _totalWear;
	QString _maxOffset;
	QString _minOffset;
	QString _GQI;
	float diffValue[21] = {0};
protected:
	bool eventFilter(QObject *watched, QEvent *event);
private slots:
	void on_close();
	void on_print();
	void on_saveWith();
	void set_table();
};

#endif // PRINTINFO_H
