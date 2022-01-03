#ifndef SPLITTRANS_H
#define SPLITTRANS_H

#include <QModelIndex>
#include <QVariant>
#include "formbase.h"

class QStandardItemModel;
class QSqlTableModel;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
	class CSplitTrans;
}

class CSplitTrans : public CFormBase
{
	Q_OBJECT
public:
  CSplitTrans(QStandardItemModel *pModel, const QVariant& IdAcc, double dAmountTrans, bool bRateEnable, bool bReadOnly, QWidget *pWidget = 0);
	~CSplitTrans();

private:
	Ui::CSplitTrans *ui;
	QStandardItemModel *m_pModel;
  QSqlTableModel *m_pModelClass;
  QVariant m_IdAcc;
	double m_dAmountTrans;
	bool m_bRateEnable;
	int m_iScrollValue;

private:
	void cleanForm();
  void enableForm(bool bEnable);

private slots:
  void on_pPBDetail_clicked();
  void on_pPBCloseL_clicked();
  void on_pPBList_clicked();
  void on_pPBMemo_toggled(bool bChecked);
  void on_pPBCalc_clicked();
  void on_pCBCategory_activated(int iIndex);
	void on_pPBApply_clicked();
	void on_pPBClose_clicked();

  void addItem();
  void deleteItem();
  void fillForm(QModelIndex Index);
	void setDirty();
	void updateTotals();

protected:
	void showEvent( QShowEvent *pEvent );
	void hideEvent ( QHideEvent *pEvent );
};

#endif // SPLITTRANS_H
