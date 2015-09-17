#include "singlebranchingnodedeletionwidget.h"
#include "ui_singlebranchingnodedeletionwidget.h"

SingleBranchingNodeDeletionWidget::SingleBranchingNodeDeletionWidget(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::SingleBranchingNodeDeletionWidget)
{
	ui->setupUi(this);
}

SingleBranchingNodeDeletionWidget::~SingleBranchingNodeDeletionWidget()
{
	delete ui;
}

void SingleBranchingNodeDeletionWidget::on_trasferButton_clicked()
{
//	this->hide();

	emit transferButtonPressed();
}

void SingleBranchingNodeDeletionWidget::on_deleteButton_clicked()
{
//	this->hide();

	emit deleteButtonPressed();
}

void SingleBranchingNodeDeletionWidget::on_cancelButton_clicked()
{
	this->hide();

	emit cancelButtonPressed();
}

void SingleBranchingNodeDeletionWidget::widgetShow()
{
	this->show();
}

void SingleBranchingNodeDeletionWidget::widgetHide()
{
	this->hide();
}
