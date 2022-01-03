#ifndef TRANSVIEW_H
#define TRANSVIEW_H

#include <QDate>
#include <QVariant>
#include <QString>
#include <QModelIndex>
#include <QPoint>
#include <QFlags>
#include "formbase.h"
#include "global.h"

class QSqlRelationalTableModel;
class QSqlTableModel;
class QTableView;
class QWidget;
class QShowEvent;
class QHideEvent;

namespace Ui
{
  class CTransView;
}

class CTransView : public CFormBase
{
  Q_OBJECT
public:
  enum Purge {PurgeDate,PurgeClear,PurgeRecon};
  enum Filter {FilterNone=0,FilterAccount=1,FilterPayee=2,FilterCategory=4,FilterClass=16,FilterFromAcc=32,FilterFromRep=64,
               FilterFromBud=128,FilterFromSch=256};
  Q_DECLARE_FLAGS(Filters, Filter);

  CTransView(Status iStatus, Filters flFilter, const QVariant& Id1, const QVariant& Id2, const QVariant& IdCurr, QWidget *pWidget = 0);
  ~CTransView();
  QTableView* tableView();
  QString rangeDates();
  QString rangeAmounts();
  QString currencyName();
  Filters filter();
  QVariant id1();
  QVariant id2();
  void adjustTotal(double dAmount);
  void purgeTransactions(Purge iPurge,const QDate& Date=QDate());

private:
  Ui::CTransView *ui;
  QSqlRelationalTableModel *m_pModel;
  QSqlTableModel *m_pModelAccount;
  QSqlTableModel *m_pModelPayee;
  QSqlTableModel *m_pModelClass;
  Filters m_flFilter;
  QVariant m_Id1;
  QVariant m_Id2;
  QVariant m_IdCurr;
  int m_iScrollValue;
  double m_dGeneralTotal;

private:
  void amendSortCriteria();
  double evaluateInitBalance();
  bool existInitBalance();

protected:
  void showEvent( QShowEvent *pEvent );
  void hideEvent ( QHideEvent *pEvent );

private slots:
  void on_pTableView_activated(QModelIndex Index);
  void on_pPBFilters_clicked();
  void on_pPBClose_clicked();

  void addItem();
  void deleteItem();
  void infoItem();
  void filterList();
  void menuColumnsChooser(QPoint Point);
  void updateColumnBalance();
  void updateGeneralBalance();
  void updateView();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CTransView::Filters);


#endif // TRANSVIEW_H
