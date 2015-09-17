#include "samplingwidget.h"
#include "ui_samplingwidget.h"

SamplingWidget::SamplingWidget(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::SamplingWidget)
{
	ui->setupUi(this);

	this->setFloating(true);

	connect(ui->spinBox, SIGNAL(valueChanged(int)), ui->horizontalSlider, SLOT(setValue(int)));
	connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), ui->spinBox, SLOT(setValue(int)));

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okButton()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButton()));
}

SamplingWidget::~SamplingWidget()
{
	delete ui;
}

void SamplingWidget::samplingStart(int boneSize, int minSampling)
{
	ui->spinBox->setMinimum(minSampling);
	ui->horizontalSlider->setMinimum(minSampling);

	this->setVisible(true);

	ui->spinBox->setValue(boneSize);
	ui->horizontalSlider->setValue(boneSize);
}

void SamplingWidget::cancelButton()
{
	this->hide();

	emit cancelButtonPressed();
}

void SamplingWidget::okButton()
{
	this->hide();

	emit okButtonPressed(ui->spinBox->value());
}
