#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <QVariant>
#include "formbase.h"

class QWidget;
class QSqlTableModel;
class QShowEvent;

namespace Ui
{
  class CItemInfo;
}

class CItemInfo : public CFormBase
{
  Q_OBJECT
public:
  CItemInfo(const QVariant& Id, QWidget *pWidget = 0);
  ~CItemInfo();

private:
  Ui::CItemInfo *ui;
  QSqlTableModel *m_pModelGroup;
  QSqlTableModel *m_pModelItem;

protected:
  void showEvent( QShowEvent *pEvent );

private slots:
    void on_pPBClose_clicked();
};

#endif // ITEMINFO_H
