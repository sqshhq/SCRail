#ifndef ANALYSISINFOCHOOSE_H
#define ANALYSISINFOCHOOSE_H

#include <QDialog>

namespace Ui {
class analysisInfoChoose;
}

class analysisInfoChoose : public QDialog
{
    Q_OBJECT

public:
     analysisInfoChoose(QVector<QString> info,QWidget *parent = 0);
    ~analysisInfoChoose();
	QString inforeturn();
private slots:
    void on_sureful();
    void on_cancel();
private:
    Ui::analysisInfoChoose *ui;
	QStringList _info, _info1,_info2,_info3;
	bool state;
protected:
	bool eventFilter(QObject *watched, QEvent *event);
	
};

#endif // ANALYSISINFOCHOOSE_H
