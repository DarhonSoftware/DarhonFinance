#ifndef AMOUNTEDITOR_H
#define AMOUNTEDITOR_H

#include <QWidget>
#include "signsymbol.h"
#include <QLineEdit>

class CAmountEditor : public QWidget
{
  Q_OBJECT

  CSignSymbol *m_pSignSymbol;
  QLineEdit *m_pLEAmount;

public:
  explicit CAmountEditor(QWidget *pWidget = 0);
  void setAmount(double dAmount);
  double amount();

signals:
  
public slots:
  
};

#endif // AMOUNTEDITOR_H
