#ifndef ITEMEDIT_H
#define ITEMEDIT_H

#include <QVariant>
#include "formbase.h"
#include "global.h"

class QShowEvent;
class QWidget;

namespace Ui
{
  class CItemEdit;
}

class CItemEdit : public CFormBase
{
  Q_OBJECT
public:
  CItemEdit(Action iAcction, const QVariant& Id, QWidget *pWidget = 0);
  ~CItemEdit();

private:
  Ui::CItemEdit *ui;
  Action m_iAction;
  QVariant m_IdGroup;
  QVariant m_IdItem;

protected:
  void showEvent( QShowEvent *pEvent );

private slots:
  void on_pPBCancel_clicked();
  void on_pPBApply_clicked();
};

#endif // ITEMEDIT_H
