#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include "formbase.h"

class QSqlTableModel;

namespace Ui
{
  class CSearch;
}

class CSearch : public CFormBase
{
  Q_OBJECT

public:
  explicit CSearch(QWidget *pWidget = 0);
  ~CSearch();

private slots:
  void on_pPBApply_clicked();
  void on_pPBCancel_clicked();
  void on_pPBDate1_clicked();
  void on_pPBDate2_clicked();
  void on_pCkBDate1_toggled(bool bChecked);
  void on_pCkBDate2_toggled(bool bChecked);

private:
  Ui::CSearch *ui;
  QString m_sDateFormat;
  QSqlTableModel *m_pModelAccount;
  QSqlTableModel *m_pModelPayee;
  QSqlTableModel *m_pModelClass;

protected:
  void showEvent( QShowEvent *pEvent );
};

#endif // SEARCH_H
