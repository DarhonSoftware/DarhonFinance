#ifndef TRANSFER_H
#define TRANSFER_H

#include "formbase.h"
#include <QString>

class QSqlTableModel;
class QWidget;
class QShowEvent;

namespace Ui
{
	class CTransfer;
}

class CTransfer : public CFormBase
{
	Q_OBJECT
public:
	CTransfer(QWidget *pWidget = 0);
	~CTransfer();

private:
	Ui::CTransfer *ui;
	QSqlTableModel *m_pModelClass;
	QSqlTableModel *m_pModelAccount1;
	QSqlTableModel *m_pModelAccount2;
  QString m_sDateFormat;

protected:
	void showEvent( QShowEvent *pEvent );

private slots:
  void on_pPBMemo_toggled(bool checked);
 void on_pPBCancel_clicked();
	void on_pPBApply_clicked();
	void on_pPBDate_clicked();
	void on_pPBCalc_clicked();
	void updateRate();
};

#endif // TRANSFER_H
