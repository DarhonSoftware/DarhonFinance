#ifndef TRANSEDIT_H
#define TRANSEDIT_H

#include <QVariant>
#include "formbase.h"
#include "global.h"

class QSqlTableModel;
class QStandardItemModel;
class QWidget;
class QShowEvent;

namespace Ui
{
	class CTransEdit;
}

class CTransEdit : public CFormBase
{
	Q_OBJECT

public:
	CTransEdit(Action iAcction, const QVariant& Id, QWidget *pWidget = 0);
	~CTransEdit();

private:
	Ui::CTransEdit *ui;
	QSqlTableModel *m_pModelPayee;
	QSqlTableModel *m_pModelCategory;
	QSqlTableModel *m_pModelClass;
	QStandardItemModel *m_pModelSplit;
	Action m_iAction;
	QVariant m_IdTrans;
	QVariant m_IdAcc;
  QString m_sDateFormat;

protected:
	void showEvent( QShowEvent *pEvent );

private slots:
  void on_pPBMemo_toggled(bool bChecked);
  void on_pCBCategory_activated(int index);
	void on_pPBSplit_clicked();
	void on_pPBCalc_clicked();
	void on_pPBDate_clicked();
	void on_pPBCancel_clicked();
	void on_pPBApply_clicked();
  void autoFillParams(const QString& sPayee);
};

#endif // TRANSEDIT_H
