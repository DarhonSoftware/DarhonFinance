#ifndef SCHEDULEDTRANS_H
#define SCHEDULEDTRANS_H

#include <QVariant>
#include <QModelIndex>
#include <QPoint>
#include <QString>
#include "formbase.h"

class QSqlTableModel;
class QSqlRelationalTableModel;
class QStandardItemModel;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
  class CScheduledTrans;
}

class CScheduledTrans : public CFormBase
{
  Q_OBJECT

public:
  CScheduledTrans(QWidget *pWidget = 0);
  ~CScheduledTrans();

private:
  Ui::CScheduledTrans *ui;
  QSqlRelationalTableModel *m_pModel;
  QSqlTableModel *m_pModelAccount;
  QSqlTableModel *m_pModelPayee;
  QSqlTableModel *m_pModelClass;
  QStandardItemModel *m_pModelSplit;
  int m_iScrollValue;
  QString m_sDateFormat;

private:
  void cleanForm();
  void enableForm(bool bEnable);
  void updateCurrencySymbol();
  void createSplitSch(const QVariant& Id1);

private slots:
  void on_pPBDetail_clicked();
  void on_pPBCloseL_clicked();
  void on_pPBList_clicked();
  void on_pPBMemo_toggled(bool bChecked);
  void on_pPBSplit_clicked();
  void on_pCBAccount_currentIndexChanged(int iIndex);
  void on_pCBLimits_currentIndexChanged(int iIndex);
  void on_pPBDate_clicked();
  void on_pPBDateLimit_clicked();
  void on_pPBClose_clicked();
  void on_pPBApply_clicked();

  void addItem();
  void deleteItem();
  void fillForm(QModelIndex Index);
  void setDirty();
  void updateStatusControls();
  void menuColumnsChooser(QPoint Point);

protected:
  void showEvent( QShowEvent *pEvent );
  void hideEvent ( QHideEvent *pEvent );
};

#endif // SCHEDULEDTRANS_H
