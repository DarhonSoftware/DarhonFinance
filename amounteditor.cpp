#include <QtGui>
#include <QtWidgets>
#include "amounteditor.h"
#include "global.h"

CAmountEditor::CAmountEditor(QWidget *pWidget) :  QWidget(pWidget)
{
  QHBoxLayout *pLayout = new QHBoxLayout(this);
  pLayout->setContentsMargins(0,0,0,0);
  pLayout->setSpacing(0);

  m_pSignSymbol = new CSignSymbol(this);
  QSizePolicy SizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  SizePolicy.setHorizontalStretch(0);
  SizePolicy.setVerticalStretch(0);
  SizePolicy.setHeightForWidth(m_pSignSymbol->sizePolicy().hasHeightForWidth());
  m_pSignSymbol->setSizePolicy(SizePolicy);
  pLayout->addWidget(m_pSignSymbol);

  m_pLEAmount = new QLineEdit(this);
  pLayout->addWidget(m_pLEAmount);

  //Setup SignSymbol
  m_pSignSymbol->setSign(0);

  //Setup LineEdit
  m_pLEAmount->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  m_pLEAmount->setValidator(pValidatorAmount);
  m_pLEAmount->setFrame(false);

  //Show editor
  setFocusPolicy(Qt::StrongFocus);
  show();
}

void CAmountEditor::setAmount(double dAmount)
{
  m_pSignSymbol->setSign(dAmount);
  m_pLEAmount->setText(QString::number(qAbs(dAmount),'f',2));
}

double CAmountEditor::amount()
{
  return m_pLEAmount->text().toDouble()*m_pSignSymbol->sign();
}
