#ifndef SKELVISUALIZATIONWIDGET_H
#define SKELVISUALIZATIONWIDGET_H

#include <QDockWidget>

namespace Ui {
class SkelVisualizationWidget;
}

class SkelVisualizationWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit SkelVisualizationWidget(QWidget *parent = 0);
	~SkelVisualizationWidget();

signals:
    void featurePointsChanged(          bool);
    void bonePointsChanged(             bool);
    void maxBallsChanged(               bool);
    void maxBallsBoneChanged(           bool);
    void defaultColor(                  bool);
    void boneColor(                     bool);
    void editModeChanged(               bool);
    void nodeFusionChanged(             bool);
    void meshLimitChanged(              bool);
    void constrainedTranslationChanged( bool);

public slots:
	void changeEditType(Qt::CheckState checkState);
	void changeNodeFusion(Qt::CheckState checkState);
	void changeMeshLimit(Qt::CheckState checkState);

private slots:

	void on_bonePointsCB_stateChanged(int arg1);

	void on_featurePointsCB_stateChanged(int arg1);

	void on_maxBallsBoneCB_stateChanged(int arg1);

	void on_maxBallsFPCB_stateChanged(int arg1);

	void on_standardRB_toggled(bool checked);

	void on_boneColorsRB_toggled(bool checked);

	void on_editTypeCB_stateChanged(int arg1);

	void on_nodeFusionCB_stateChanged(int arg1);

	void on_meshLimitCB_stateChanged(int arg1);

    void on_constrainedTranslationCB_stateChanged(int arg1);

private:
	Ui::SkelVisualizationWidget *ui;
};

#endif // SKELVISUALIZATIONWIDGET_H
