#ifndef CHOOSEPASTENODEWIDGET_H
#define CHOOSEPASTENODEWIDGET_H

#include <QDockWidget>

namespace Ui {
	class ChoosePasteNodeWidget;
}

class ChoosePasteNodeWidget : public QDockWidget
{
		Q_OBJECT

	public:
		explicit ChoosePasteNodeWidget(QWidget *parent = 0);
		~ChoosePasteNodeWidget();

	private:
		Ui::ChoosePasteNodeWidget *ui;

	signals:
		void selectButtonPressed();
		void helpButtonPressed();
		void cancelButtonPressed();

	public slots:
		void on_selectButton_clicked();
		void on_helpButton_clicked();
		void on_cancelButton_clicked();

		void widgetShow();
		void widgetHide();
};

#endif // CHOOSEPASTENODEWIDGET_H
