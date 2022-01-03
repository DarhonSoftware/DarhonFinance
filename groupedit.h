#ifndef GROUPEDIT_H
#define GROUPEDIT_H

#include <QModelIndex>
#include "formbase.h"

class QSqlTableModel;
class QShowEvent;
class QHideEvent;
class QWidget;

namespace Ui
{
  class CGroupEdit;
}

class CGroupEdit : public CFormBase
{
  Q_OBJECT
public:
  CGroupEdit(const QVariant& Id,QWidget *pWidget = 0);
  ~CGroupEdit();

private:
  Ui::CGroupEdit *ui;
  QSqlTableModel *m_pModel;
  int m_iScrollValue;

private:
  void cleanForm();
  void enableForm(bool bEnable);

private slots:
  void on_pPBDetail_clicked();
  void on_pPBCloseL_clicked();
  void on_pPBList_clicked();
  void on_pPBClose_clicked();
  void on_pPBApply_clicked();

  void addItem();
  void deleteItem();
  void fillForm(QModelIndex Index);
  void setDirty();

protected:
  void showEvent( QShowEvent *pEvent );
  void hideEvent ( QHideEvent *pEvent );
};

#endif // GROUPEDIT_H
