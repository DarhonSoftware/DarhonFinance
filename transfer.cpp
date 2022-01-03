#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "transfer.h"
#include "ui_transfer.h"
#include "global.h"
#include "exchange.h"
#include "calendar.h"
#include "dr_profile.h"
#include "mainwindow0.h"


CTransfer::CTransfer(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CTransfer)
{
  ui->setupUi(this);

  //Setup children
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";
  ui->pLEAmount->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLERate->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLECheque->setInputMethodHints(Qt::ImhDigitsOnly);
  ui->pSignSymbol->setNegativeZero(false);
  ui->pSignSymbol->setSign(0);
#ifdef MOBILE_PLATFORM
  ui->pRichTextEditor->setVisible(false);
#else
  ui->pPBMemo->hide();
#endif

  //Set de validators
  QIntValidator *pValidatorCheque=new QIntValidator(this);
  pValidatorCheque->setBottom(0);
  ui->pLECheque->setValidator(pValidatorCheque);
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  ui->pLEAmount->setValidator(pValidatorAmount);
  QDoubleValidator *pValidatorRate=new QDoubleValidator(0,MAX_AMOUNT,RATE_PREC,this);
  pValidatorRate->setNotation(QDoubleValidator::StandardNotation);
  ui->pLERate->setValidator(pValidatorRate);

  //Initiate Account1 ComboBox
  m_pModelAccount1=new QSqlTableModel(this);
  m_pModelAccount1->setTable("account");
  m_pModelAccount1->setSort(3,Qt::AscendingOrder);
  ui->pCBAccount1->setModel(m_pModelAccount1);
  ui->pCBAccount1->setModelColumn(3);

  //Initiate Account2 ComboBox
  m_pModelAccount2=new QSqlTableModel(this);
  m_pModelAccount2->setTable("account");
  m_pModelAccount2->setSort(3,Qt::AscendingOrder);
  ui->pCBAccount2->setModel(m_pModelAccount2);
  ui->pCBAccount2->setModelColumn(3);

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Set minimum data in controls
  QDate Date=QDate::currentDate();
  ui->pPBDate->setText(Date.toString(m_sDateFormat));

  //Connections
  connect(ui->pCBAccount1,SIGNAL(activated(int)),this,SLOT(updateRate()));
  connect(ui->pCBAccount2,SIGNAL(activated(int)),this,SLOT(updateRate()));
}

CTransfer::~CTransfer()
{
    delete ui;
}

void CTransfer::on_pPBCalc_clicked()
{
  int iId1=m_pModelAccount1->record(ui->pCBAccount1->currentIndex()).value(0).toInt();

  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+QString::number(iId1));
  ModelAcc.select();

  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+ModelAcc.record(0).value(5).toString());
  ModelCurr.select();

  double dRate=ModelCurr.record(0).value(2).toDouble();
  QString sCurrency=ModelCurr.record(0).value(1).toString();

  CExchange Exchange(dRate,sCurrency,this);
  if (Exchange.exec()==QDialog::Accepted)
  {
    double dAmount=Exchange.calculatedAmount();
    ui->pLEAmount->setText(QString::number(dAmount,'f',2));
  }
}

void CTransfer::on_pPBDate_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBDate->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBDate->setText(Date.toString(m_sDateFormat));
  }
}

void CTransfer::on_pPBApply_clicked()
{
  if (ui->pCBAccount1->currentIndex()==ui->pCBAccount2->currentIndex()) return;

  //Validate rate
  if ((ui->pLERate->isEnabled())&&(ui->pLERate->text().toDouble()==0))
  {
    g_messageWarning(this,tr("Apply"),tr("The rate cannot be zero"));
    return;
  }

  //Set model to apply changes
  QSqlTableModel ModelTrans;
  ModelTrans.setTable("trans");
  ModelTrans.select();

  int iId=g_generateId("trans");
  ModelTrans.insertRow(0);
  ModelTrans.setData(ModelTrans.index(0,0),iId);
  ModelTrans.submitAll();
  ModelTrans.setFilter("idx="+QString::number(iId));

  QSqlRecord Record=ModelTrans.record(0);

  //Set fields for transaction
  Record.setValue(1,0);
  Record.setValue(2,0);
  Record.setValue(3,QDate::fromString(ui->pPBDate->text(),m_sDateFormat).toJulianDay());
  Record.setValue(4,m_pModelClass->record(ui->pCBClass->currentIndex()).value(0));
  Record.setValue(5,ID_TRANSFER);
  Record.setValue(6,m_pModelAccount1->record(ui->pCBAccount1->currentIndex()).value(0));
  Record.setValue(7,m_pModelAccount2->record(ui->pCBAccount2->currentIndex()).value(0).toInt()*-1);
  Record.setValue(8,ui->pLECheque->text().simplified());
  Record.setValue(9,ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign()*-1);
  Record.setValue(10,ui->pRichTextEditor->toHtml());
  Record.setValue(13,QString::number(ui->pLERate->text().toDouble(),'f',RATE_PREC));
  Record.setValue(12,0);
  Record.setValue(14,0);
  ModelTrans.setRecord(0,Record);
  ModelTrans.submitAll();

  g_transfer(Record.value(0));
  close();
}

void CTransfer::on_pPBCancel_clicked()
{
  close();
}

void CTransfer::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBCancel->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pRichTextEditor->setVisible(bChecked);
}

void CTransfer::updateRate()
{
  int iId1=m_pModelAccount1->record(ui->pCBAccount1->currentIndex()).value(0).toInt();
  int iId2=m_pModelAccount2->record(ui->pCBAccount2->currentIndex()).value(0).toInt();

  //Get the rate for the currency used in Account1 combobox
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+QString::number(iId1));
  ModelAcc.select();
  QString sCurr1=ModelAcc.record(0).value(5).toString();

  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+sCurr1);
  ModelCurr.select();
  double dRate1=ModelCurr.record(0).value(2).toDouble();

  //Update the currency symbol
  ui->pLCurrency->setText(ModelCurr.record(0).value(3).toString());

  //Get the rate for the currency used in Account2 combobox
  ModelAcc.setFilter("idx="+QString::number(iId2));
  ModelAcc.select();
  QString sCurr2=ModelAcc.record(0).value(5).toString();

  ModelCurr.setFilter("idx="+sCurr2);
  ModelCurr.select();
  double dRate2=ModelCurr.record(0).value(2).toDouble();

  ui->pLERate->setText(QString::number(dRate2/dRate1,'f',RATE_PREC));
    if (sCurr1==sCurr2)
    ui->pLERate->setEnabled(false);
  else
    ui->pLERate->setEnabled(true);
}

void CTransfer::showEvent( QShowEvent */*pEvent*/ )
{
  //Refresh Payee/Class/Accounts Comboboxes
  g_refreshCombobox(ui->pCBClass,QVariant(),false);
  g_refreshCombobox(ui->pCBAccount1,QVariant(),false);
  g_refreshCombobox(ui->pCBAccount2,QVariant(),false);

  updateRate();

  //Set focus in amount edit
  ui->pLEAmount->setFocus();
  ui->pLEAmount->selectAll();
}

