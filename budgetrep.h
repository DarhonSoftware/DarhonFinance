#ifndef BUDGETREP_H
#define BUDGETREP_H

#include <QWidget>
#include <QString>
#include <QModelIndex>
#include <QVariant>
#include <QDate>
#include "formbase.h"

class QStandardItemModel;
class QSqlTableModel;
class CBaseDelegate;
class QWidget;
class QShowEvent;

namespace Ui
{
  class CBudgetRep;
}

class CBudgetRep : public CFormBase
{
  Q_OBJECT

public:
  explicit CBudgetRep(const QVariant& IdBudget, QWidget *pWidget = 0);
  ~CBudgetRep();

private:
  Ui::CBudgetRep *ui;
  QStandardItemModel *m_pModel;
  CBaseDelegate* m_pDelegate;
  QVariant m_IdCurr;
  QVariant m_IdClass;
  QVariant m_IdBudget;

protected:
  void showEvent( QShowEvent *pEvent );

private:
  void initColumn(int iCol);
  void fillUsedColumn(const QString& sCurrSym, double dEndingRate);
  void fillBudgetColumn(const QString& sCurrSym);
  void fillTotalColumn(const QString& sCurrSym);
  void fillTotalRow(const QString& sCurrSym);

private slots:
  void on_pPBClose_clicked();
  void on_pTableView_activated(QModelIndex Index);
  void updateView();
  void updateLabelInfo();
};

#endif // BUDGETREP_H
