#ifndef CEXCHANGE_H
#define CEXCHANGE_H

#include <QDialog>
#include <QString>

class QSqlTableModel;
class QWidget;

namespace Ui
{
  class CExchange;
}

class CExchange : public QDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(CExchange)

public:
  explicit CExchange(double dRate2, const QString &sCurrency2, QWidget *pWidget = 0);
  virtual ~CExchange();
  double calculatedAmount();

private:
  Ui::CExchange *ui;
  QSqlTableModel *m_pModelCurrency;
  QString m_sCurrency2;
  double m_dRate2;
  double m_dAmount2;

private slots:
  void on_pCBCurrency1_activated(int iIndex);
  void on_pPBCalc_clicked();
};

#endif // CEXCHANGE_H
