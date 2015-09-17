#include "choosepastenodewidget.h"
#include "ui_choosepastenodewidget.h"

#include <QMessageBox>

ChoosePasteNodeWidget::ChoosePasteNodeWidget(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::ChoosePasteNodeWidget)
{
	ui->setupUi(this);
}

ChoosePasteNodeWidget::~ChoosePasteNodeWidget()
{
	delete ui;
}

void ChoosePasteNodeWidget::on_selectButton_clicked()
{
//	this->hide();

	emit selectButtonPressed();
}

void ChoosePasteNodeWidget::on_helpButton_clicked()
{
//	this->hide();

	emit helpButtonPressed();
}

void ChoosePasteNodeWidget::on_cancelButton_clicked()
{
	this->hide();

	emit cancelButtonPressed();
}

void ChoosePasteNodeWidget::widgetShow()
{
	this->show();
}

void ChoosePasteNodeWidget::widgetHide()
{
	this->hide();
}
