#ifndef SINGLEBRANCHINGNODEDELETIONWIDGET_H
#define SINGLEBRANCHINGNODEDELETIONWIDGET_H

#include <QDockWidget>

namespace Ui {
	class SingleBranchingNodeDeletionWidget;
}

class SingleBranchingNodeDeletionWidget : public QDockWidget
{
		Q_OBJECT

	public:
		explicit SingleBranchingNodeDeletionWidget(QWidget *parent = 0);
		~SingleBranchingNodeDeletionWidget();

	private:
		Ui::SingleBranchingNodeDeletionWidget *ui;

	signals:
		void transferButtonPressed();
		void deleteButtonPressed();
		void cancelButtonPressed();

	public slots:
		void on_trasferButton_clicked();
		void on_deleteButton_clicked();
		void on_cancelButton_clicked();

		void widgetShow();
		void widgetHide();
};

#endif // SINGLEBRANCHINGNODEDELETIONWIDGET_H
