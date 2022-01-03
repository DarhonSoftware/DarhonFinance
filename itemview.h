#ifndef ITEMVIEW_H
#define ITEMVIEW_H

#include <QModelIndex>
#include "formbase.h"

class QWidget;
class QSqlRelationalTableModel;
class QSqlTableModel;
class QShowEvent;
class QHideEvent;

namespace Ui
{
  class CItemView;
}

class CItemView : public CFormBase
{
  Q_OBJECT
public:
  CItemView(QWidget *pWidget = 0);
  ~CItemView();

protected:
  void showEvent( QShowEvent *pEvent );
  void hideEvent ( QHideEvent *pEvent );

private:
  Ui::CItemView *ui;
  QSqlRelationalTableModel *m_pModel;
  QSqlTableModel *m_pModelGroup;
  int m_iScrollValue;

private slots:
  void on_pCBGroup_currentIndexChanged(int iIndex);
  void on_pTableView_activated(QModelIndex Index);
  void on_pPBClose_clicked();
  void on_pPBGroups_clicked();

  void addItem();
  void deleteItem();
  void infoItem();
  void filterList();

private:
  void updateHeaders();
};

#endif // ITEMVIEW_H
