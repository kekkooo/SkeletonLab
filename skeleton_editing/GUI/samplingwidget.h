#ifndef SAMPLINGWIDGET_H
#define SAMPLINGWIDGET_H

#include <QDockWidget>

namespace Ui {
	class SamplingWidget;
}

class SamplingWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit SamplingWidget(QWidget *parent = 0);
	~SamplingWidget();

private:
	Ui::SamplingWidget *ui;

signals:
	void cancelButtonPressed();
	void okButtonPressed(int);

private slots:
	void samplingStart(int boneSize, int minSampling);

	void cancelButton();
	void okButton();
};

#endif // SAMPLINGWIDGET_H
