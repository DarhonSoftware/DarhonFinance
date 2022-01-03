#ifndef BUDGETVIEW_H
#define BUDGETVIEW_H

#include <QWidget>
#include <QColor>
#include <QVariant>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QSqlRecord>
#include "formbase.h"

class QStandardItemModel;
class QSqlTableModel;
class CBaseDelegate;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
  class CBudgetView;
}

class CBudgetView : public CFormBase
{
  Q_OBJECT

private:
  Ui::CBudgetView *ui;
  QStandardItemModel *m_pModel;
  QSqlTableModel *m_pModelClass;
  CBaseDelegate *m_pDelegate;
  int m_iScrollValue;

public:
  explicit CBudgetView(QWidget *pWidget = 0);
  ~CBudgetView();

private:
  double totalBudget(const QVariant& IdBudget);
  double totalUsed(const QSqlRecord& Record);
  QColor colorBudget(int iPercentUsed, int iPercentElapsed);

protected:
  void showEvent( QShowEvent *pEvent );
  void hideEvent ( QHideEvent *pEvent );

private slots:
  void on_pTableView_activated(QModelIndex Index);
  void on_pCBClass_currentIndexChanged(int iIndex);
  void on_pPBBudgets_clicked();
  void on_pPBClose_clicked();

  void updateView();
};

#endif // BUDGETVIEW_H
