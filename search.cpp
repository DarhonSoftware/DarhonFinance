#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "search.h"
#include "ui_search.h"
#include "global.h"
#include "calendar.h"
#include "mainwindow0.h"
#include "transview.h"

CSearch::CSearch(QWidget *pWidget) :
  CFormBase(pWidget),
  ui(new Ui::CSearch)
{
  ui->setupUi(this);

  //Reset global object
  g_FilterParam.clear();

  //Set date format
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";

  //Setup children
  ui->pLECheque1->setInputMethodHints(Qt::ImhDigitsOnly);
  ui->pLECheque2->setInputMethodHints(Qt::ImhDigitsOnly);
  ui->pLEAmount1->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLEAmount2->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pPBDate1->setText(QDate::currentDate().toString(m_sDateFormat));
  ui->pPBDate2->setText(QDate::currentDate().toString(m_sDateFormat));
  ui->pPBDate1->setEnabled(false);
  ui->pPBDate2->setEnabled(false);

  //Set de validators
  QIntValidator *pValidatorCheque=new QIntValidator(this);
  pValidatorCheque->setBottom(0);
  ui->pLECheque1->setValidator(pValidatorCheque);
  ui->pLECheque2->setValidator(pValidatorCheque);
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  ui->pLEAmount1->setValidator(pValidatorAmount);
  ui->pLEAmount2->setValidator(pValidatorAmount);

  //Initiate Amounts ComboBox
  g_initComboboxAmounts(ui->pCBAmtType,0);

  //Initiate Category ComboBox
  g_refreshComboCategory(ui->pCBCategory,QVariant(),QVariant(),true);

  //Initiate Account ComboBox
  m_pModelAccount=new QSqlTableModel(this);
  m_pModelAccount->setTable("account");
  m_pModelAccount->setSort(3,Qt::AscendingOrder);
  ui->pCBAccount->setModel(m_pModelAccount);
  ui->pCBAccount->setModelColumn(3);

  //Initiate Payee ComboBox
  m_pModelPayee=new QSqlTableModel(this);
  m_pModelPayee->setTable("payee");
  m_pModelPayee->setSort(1,Qt::AscendingOrder);
  ui->pCBPayee->setModel(m_pModelPayee);
  ui->pCBPayee->setModelColumn(1);

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);
}

CSearch::~CSearch()
{
  delete ui;
}

void CSearch::on_pPBApply_clicked()
{
  //Get Dates parameters and validate them
  if ((ui->pCkBDate1->isChecked()) && (ui->pCkBDate2->isChecked()))
  {
    QDate Date1=QDate::fromString(ui->pPBDate1->text(),m_sDateFormat);
    QDate Date2=QDate::fromString(ui->pPBDate2->text(),m_sDateFormat);
    if (Date1.daysTo(Date2)<0)
    {
      g_messageWarning(this,tr("Search"),tr("Range of dates are not correct"));
      return;
    }
  }

  //Validate Amount parameters
  if ((!ui->pLEAmount1->text().simplified().isEmpty()) && (!ui->pLEAmount2->text().simplified().isEmpty()))
    if (ui->pLEAmount1->text().simplified().toDouble()>ui->pLEAmount2->text().simplified().toDouble())
    {
      g_messageWarning(this,tr("Search"),tr("Range of amounts are not correct"));
      return;
    }

  //Validate Cheque parameters
  if ((!ui->pLECheque1->text().simplified().isEmpty()) && (!ui->pLECheque2->text().simplified().isEmpty()))
    if (ui->pLECheque1->text().simplified().toInt()>ui->pLECheque2->text().simplified().toInt())
    {
      g_messageWarning(this,tr("Search"),tr("Range of cheques are not correct"));
      return;
    }

  //Save Date1/Date2
  if (ui->pCkBDate1->isChecked())
    g_FilterParam.setDate1(QDate::fromString(ui->pPBDate1->text(),m_sDateFormat));
  else
    g_FilterParam.setDate1(QDate());

  if (ui->pCkBDate2->isChecked())
    g_FilterParam.setDate2(QDate::fromString(ui->pPBDate2->text(),m_sDateFormat));
  else
    g_FilterParam.setDate2(QDate());

  //Save Cheque1/Cheque2
  if (ui->pLECheque1->text().simplified().isEmpty())
    g_FilterParam.setCheque1("");
  else
    g_FilterParam.setCheque1(QString::number(ui->pLECheque1->text().simplified().toInt()));

  if (ui->pLECheque2->text().simplified().isEmpty())
    g_FilterParam.setCheque2("");
  else
    g_FilterParam.setCheque2(QString::number(ui->pLECheque2->text().simplified().toInt()));

  //Save Amount1/Amount2
  if (ui->pLEAmount1->text().simplified().isEmpty())
    g_FilterParam.setAmount1("");
  else
    g_FilterParam.setAmount1(QString::number(ui->pLEAmount1->text().simplified().toDouble(),'f',2));

  if (ui->pLEAmount2->text().simplified().isEmpty())
    g_FilterParam.setAmount2("");
  else
    g_FilterParam.setAmount2(QString::number(ui->pLEAmount2->text().simplified().toDouble(),'f',2));

  //Save Amount type
  g_FilterParam.setAmountType(ui->pCBAmtType->currentIndex());

  //Save Memo
  g_FilterParam.setMemo(ui->pLEMemo->text().simplified());

  //Save Category
  g_FilterParam.setCategoryid(ui->pCBCategory->itemData(ui->pCBCategory->currentIndex()).toInt());

  //Save Account
  g_FilterParam.setAccountid(m_pModelAccount->record(ui->pCBAccount->currentIndex()).value(0).toInt());

  //Save Payee
  g_FilterParam.setPayeeid(m_pModelPayee->record(ui->pCBPayee->currentIndex()).value(0).toInt());

  //Save Class
  g_FilterParam.setClassid(m_pModelClass->record(ui->pCBClass->currentIndex()).value(0).toInt());

  //Prepare filtering parameters and open TransView
  g_pMainWindow->addForm("CTransView",QVariant(),QVariant(),StatusAll,CTransView::FilterFromSch,0);
}

void CSearch::on_pPBCancel_clicked()
{
  close();
}

void CSearch::on_pPBDate1_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBDate1->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBDate1->setText(Date.toString(m_sDateFormat));
  }
}

void CSearch::on_pPBDate2_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBDate2->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBDate2->setText(Date.toString(m_sDateFormat));
  }
}

void CSearch::on_pCkBDate1_toggled(bool bChecked)
{
  ui->pPBDate1->setEnabled(bChecked);
}

void CSearch::on_pCkBDate2_toggled(bool bChecked)
{
  ui->pPBDate2->setEnabled(bChecked);
}

void CSearch::showEvent( QShowEvent */*pEvent*/ )
{
  g_refreshCombobox(ui->pCBAccount,QVariant(),true);
  g_refreshCombobox(ui->pCBPayee,QVariant(),true);
  g_refreshComboCategory(ui->pCBCategory,QVariant(),QVariant(),true);
  g_refreshCombobox(ui->pCBClass,QVariant(),true);
}
