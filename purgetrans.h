#ifndef PURGETRANS_H
#define PURGETRANS_H

#include <QDialog>
#include <QDate>

class QButtonGroup;
class QWidget;

namespace Ui
{
  class CPurgeTrans;
}

class CPurgeTrans : public QDialog
{
  Q_OBJECT
public:
  CPurgeTrans(QWidget *pWidget = 0);
  ~CPurgeTrans();
  int choice();
  QDate date();

private:
  Ui::CPurgeTrans *ui;
  QButtonGroup *m_pButtonGroup;
  int m_iChoice;
  QString m_sDateFormat;

private slots:
  void on_pPBDate_clicked();
  void setChoice(int iChoice);
};

#endif // PURGETRANS_H
