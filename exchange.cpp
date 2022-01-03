#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "exchange.h"
#include "ui_exchange.h"
#include "global.h"
#include "dr_profile.h"

CExchange::CExchange(double dRate2, const QString &sCurrency2,QWidget *pWidget) :
    QDialog(pWidget),
    ui(new Ui::CExchange)
{
  ui->setupUi(this);

  //Set Children
  ui->pLEAmount1->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLEAmount2->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLERate->setInputMethodHints(Qt::ImhFormattedNumbersOnly);

  //Set de validators
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  ui->pLEAmount1->setValidator(pValidatorAmount);
  ui->pLEAmount2->setValidator(pValidatorAmount);
  QDoubleValidator *pValidatorRate=new QDoubleValidator(0,MAX_AMOUNT,RATE_PREC,this);
  pValidatorRate->setNotation(QDoubleValidator::StandardNotation);
  ui->pLERate->setValidator(pValidatorRate);

  //Resize the window to optimaize the platform style
  adjustSize();

  m_dRate2=dRate2;
  m_sCurrency2=sCurrency2;
  m_dAmount2=0.0;

  m_pModelCurrency=new QSqlTableModel(this);
  m_pModelCurrency->setTable("currency");
  m_pModelCurrency->select();

  ui->pCBCurrency1->setModel(m_pModelCurrency);
  ui->pCBCurrency1->setModelColumn(1);
  ui->pCBCurrency1->setCurrentIndex(-1);

  ui->pLCurrency2->setText(m_sCurrency2);
}

CExchange::~CExchange()
{
  delete ui;
}

void CExchange::on_pPBCalc_clicked()
{
  double dAmount1=ui->pLEAmount1->text().toDouble();
  double dRate=ui->pLERate->text().toDouble();
  m_dAmount2=dAmount1*dRate;
  ui->pLEAmount2->setText(QString::number(m_dAmount2,'f',2));
}

void CExchange::on_pCBCurrency1_activated(int iIndex)
{
  double dRate1=m_pModelCurrency->record(iIndex).value(2).toDouble();
  double dRate=m_dRate2/dRate1;
  ui->pLERate->setText(QString::number(dRate,'f',RATE_PREC));
  ui->pLEAmount1->setFocus();
}

double CExchange::calculatedAmount()
{
  return m_dAmount2;
}

