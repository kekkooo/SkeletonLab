#include "skelvisualizationwidget.h"
#include "ui_skelvisualizationwidget.h"
#include <assert.h>

SkelVisualizationWidget::SkelVisualizationWidget(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::SkelVisualizationWidget)
{
	ui->setupUi(this);
}

SkelVisualizationWidget::~SkelVisualizationWidget()
{
	delete ui;
}

void SkelVisualizationWidget::changeEditType(Qt::CheckState checkState)
{
	ui->editTypeCB->setCheckState(checkState);
}

void SkelVisualizationWidget::changeNodeFusion(Qt::CheckState checkState)
{
	ui->nodeFusionCB->setCheckState(checkState);
}

void SkelVisualizationWidget::changeMeshLimit(Qt::CheckState checkState)
{
	ui->nodeFusionCB->setCheckState(checkState);
}

void SkelVisualizationWidget::on_bonePointsCB_stateChanged(int arg1)
{
	emit this->bonePointsChanged( arg1 == (int)Qt::Checked );
}

void SkelVisualizationWidget::on_featurePointsCB_stateChanged(int arg1)
{
	emit this->featurePointsChanged( arg1 == (int)Qt::Checked );
}

void SkelVisualizationWidget::on_maxBallsBoneCB_stateChanged(int arg1)
{
	emit this->maxBallsBoneChanged( arg1 == (int)Qt::Checked );
}

void SkelVisualizationWidget::on_maxBallsFPCB_stateChanged(int arg1)
{
	emit this->maxBallsChanged( arg1 == (int)Qt::Checked );
}

void SkelVisualizationWidget::on_standardRB_toggled(bool checked)
{
	emit defaultColor(checked);
}

void SkelVisualizationWidget::on_boneColorsRB_toggled(bool checked)
{
	emit boneColor(checked);
}

void SkelVisualizationWidget::on_editTypeCB_stateChanged(int arg1)
{
	emit this->editModeChanged( arg1 == (int)Qt::Checked );
}

void SkelVisualizationWidget::on_meshLimitCB_stateChanged(int arg1)
{
	emit this->meshLimitChanged( arg1 == (int)Qt::Checked );
}

void SkelVisualizationWidget::on_nodeFusionCB_stateChanged(int arg1)
{
	emit this->nodeFusionChanged( arg1 == (int)Qt::Checked );
}
