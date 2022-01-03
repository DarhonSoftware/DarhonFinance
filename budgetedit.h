#ifndef BUDGETEDIT_H
#define BUDGETEDIT_H

#include <QVariant>
#include <QModelIndex>
#include <QString>
#include <QSqlRecord>
#include "formbase.h"

class QSqlTableModel;
class QWidget;
class QShowEvent;
class QHideEvent;
class QStandardItemModel;
class CDelegateBudgetEdit;

namespace Ui
{
  class CBudgetEdit;
}

class CBudgetEdit : public CFormBase
{
  Q_OBJECT

public:
  explicit CBudgetEdit(const QVariant& Id, QWidget *pWidget = 0);
  virtual ~CBudgetEdit();

private:
  Ui::CBudgetEdit *ui;
  QSqlTableModel *m_pModel;
  QSqlTableModel *m_pModelCurrency;
  QSqlTableModel *m_pModelClass;
  QStandardItemModel *m_pModelBudgetCat;
  CDelegateBudgetEdit* m_pDelegate;
  QString m_sDateFormat;
  int m_iScrollValue;

private:
  void cleanForm();
  void enableForm(bool bEnable);
  void cleanBudgetCat();
  void loadBudgetCat(const QSqlRecord& Record);
  void createModelBudgetCat();

private slots:
  void on_pPBClose_clicked();
  void on_pPBApply_clicked();
  void on_pPBCategories_toggled(bool bChecked);
  void on_pPBList_clicked();
  void on_pPBCloseL_clicked();
  void on_pPBDetail_clicked();
  void on_pPBStart_clicked();
  void on_pPBEnd_clicked();
  void on_pCBRecurrent_currentIndexChanged(int iIndex);
  void on_pCBCurrency_currentIndexChanged(int index);

  void addItem();
  void deleteItem();
  void fillForm(QModelIndex Index);
  void setDirty();
  void updateView();

protected:
  void showEvent( QShowEvent *pEvent );
  void hideEvent ( QHideEvent *pEvent );
};

#endif // BUDGETEDIT_H
