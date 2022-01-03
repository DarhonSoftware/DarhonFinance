#ifndef HELP_H
#define HELP_H

#include <QWidget>
#include "formbase.h"

namespace Ui
{
  class CHelp;
}

class CHelp : public CFormBase
{
  Q_OBJECT

public:
  explicit CHelp(QWidget *pWidget = 0);
  ~CHelp();

private slots:
  void on_pPBClose_clicked();
  void on_pPBScrollUp_clicked();
  void on_pPBScrollDown_clicked();

private:
  Ui::CHelp *ui;

protected:
  void showEvent( QShowEvent *pEvent );
};

#endif // HELP_H
