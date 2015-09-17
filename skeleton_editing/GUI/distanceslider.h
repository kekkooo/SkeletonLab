#ifndef DISTANCESLIDER_H
#define DISTANCESLIDER_H

#include <QWidget>
#include "ui_distanceslider.h"

class distanceslider : public QWidget
{
	Q_OBJECT

public:
	distanceslider(QWidget *parent = 0);
	~distanceslider();

private:
	Ui::distanceslider ui;
};

#endif // DISTANCESLIDER_H
