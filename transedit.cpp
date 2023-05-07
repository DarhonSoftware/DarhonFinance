#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "transedit.h"
#include "ui_transedit.h"
#include "calendar.h"
#include "exchange.h"
#include "global.h"
#include "splittrans.h"
#include "mainwindow0.h"
#include "signsymbol.h"
#include "dr_profile.h"


CTransEdit::CTransEdit(Action iAction, const QVariant& Id, QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CTransEdit)
{
  ui->setupUi(this);

  //Setup children
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";
  ui->pStatusWidget->setFrameStyle(CStatusWidget::FrameNo);
  ui->pLEAmount->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLERate->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLECheque->setInputMethodHints(Qt::ImhDigitsOnly);
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

  //Set Model
  m_pModelSplit=new QStandardItemModel(this);

  //Initiate Payee ComboBox
  m_pModelPayee=new QSqlTableModel(this);
  m_pModelPayee->setTable("payee");
  m_pModelPayee->setSort(1,Qt::AscendingOrder);
  m_pModelPayee->setFilter("idx>=0");
  ui->pCBPayee->setModel(m_pModelPayee);
  ui->pCBPayee->setModelColumn(1);

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Fill out the form for ActionEdit
  //If ActionEdit -> Id for table "trans"
  //If ActionAdd -> Id for table "account"
  m_iAction=iAction;
  if (iAction==ActionEdit)
  {
    QSqlTableModel Model;
    Model.setTable("trans");
    Model.setFilter("idx="+Id.toString());
    Model.select();

    //Set member variables
    m_IdTrans=Id;
    m_IdAcc=Model.record(0).value(6);
    int iCat=Model.record(0).value(7).toInt();

    //Set Comboboxes
    g_refreshCombobox(ui->pCBPayee,Model.record(0).value(5),false);
    g_refreshCombobox(ui->pCBClass,Model.record(0).value(4),false);
    g_refreshComboCategory(ui->pCBCategory,iCat,m_IdAcc,false);

    //Set controls
    ui->pStatusWidget->setState(Model.record(0).value(2).toInt());
    QDate Date=QDate::fromJulianDay(Model.record(0).value(3).toInt());
    ui->pPBDate->setText(Date.toString(m_sDateFormat));
    ui->pLEAmount->setText(QString::number(qAbs(Model.record(0).value(9).toDouble()),'f',2));
    ui->pSignSymbol->setSign(Model.record(0).value(9).toDouble());
    ui->pLECheque->setText(Model.record(0).value(8).toString());
    ui->pRichTextEditor->setHtml(Model.record(0).value(10).toString());
    ui->pLERate->setText(Model.record(0).value(13).toString());
    ui->pLERate->setEnabled(false);

    //Set controls in case this is a transfer
    if ((iCat<0)&&(iCat>=-ID_MAX))
    {
      ui->pCBPayee->setEnabled(false);
      ui->pCBPayee->lineEdit()->clear();

      bool bDiff;
      g_transferRate(m_IdAcc.toInt(),iCat*-1,&bDiff);
      if (bDiff)
        ui->pLERate->setEnabled(true);
    }

    //Set controls in case this is a split
    if (Model.record(0).value(7).toInt()==ID_SPLIT)
    {
      //Load childdren transactions
      QSqlTableModel ModelTrans;
      ModelTrans.setTable("trans");
      ModelTrans.setFilter("split="+QString::number(SPLIT+(Model.record(0).value(14).toInt())));
      ModelTrans.select();

      int iRowCount=g_rowCount(ModelTrans);
      for (int iRow=0;iRow<iRowCount;iRow++)
      {
        m_pModelSplit->setItem(iRow,0,new QStandardItem(ModelTrans.record(iRow).value(7).toString()));  //Category
        m_pModelSplit->setItem(iRow,1,new QStandardItem(ModelTrans.record(iRow).value(4).toString()));  //Class
        m_pModelSplit->setItem(iRow,2,new QStandardItem(ModelTrans.record(iRow).value(9).toString()));  //Amount
        m_pModelSplit->setItem(iRow,3,new QStandardItem(ModelTrans.record(iRow).value(10).toString())); //Note
        m_pModelSplit->setItem(iRow,4,new QStandardItem(ModelTrans.record(iRow).value(13).toString())); //Aux
      }
      ui->pPBSplit->setEnabled(true);
      ui->pCBClass->setEnabled(false);
    }
    else
      ui->pPBSplit->setEnabled(false);
  }
  if (iAction==ActionAdd)
  {
    //Set member variables
    m_IdTrans.clear();
    m_IdAcc=Id;

    //Set minimum data in controls
    ui->pStatusWidget->setState(0);
    QDate Date=QDate::currentDate();
    ui->pPBDate->setText(Date.toString(m_sDateFormat));
    ui->pSignSymbol->setSign(0);
    ui->pLERate->setEnabled(false);
    ui->pPBSplit->setEnabled(false);
  }

  //Connections
  connect(ui->pCBPayee, SIGNAL(textActivated(QString)), this, SLOT(autoFillParams(QString)));
}

CTransEdit::~CTransEdit()
{
  delete ui;
}

void CTransEdit::showEvent( QShowEvent */*pEvent*/ )
{
  //Write the Title and Icon from Account
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+m_IdAcc.toString());
  ModelAcc.select();

  //Write the Currency symbol from Currency
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+ModelAcc.record(0).value(5).toString());
  ModelCurr.select();
  ui->pLCurrency->setText(ModelCurr.record(0).value(3).toString());

  //Refresh Payee/Class Comboboxes
  g_refreshCombobox(ui->pCBPayee,QVariant(),false);
  g_refreshCombobox(ui->pCBClass,QVariant(),false);
  g_refreshComboCategory(ui->pCBCategory,QVariant(),m_IdAcc,false);

  //Set focus in amount edit
  ui->pLEAmount->setFocus();
  ui->pLEAmount->selectAll();
}

void CTransEdit::on_pPBApply_clicked()
{
  //Get Id from category combobox
  int iCat=ui->pCBCategory->itemData(ui->pCBCategory->currentIndex()).toInt();

  //Validate Payee
  if (ui->pCBPayee->lineEdit()->text().simplified()==TRANSFER_STR)
  {
    g_messageWarning(this,tr("Apply"),tr("The payee cannot use the reserved name ") + TRANSFER_QTD);
    return;
  }

  //Validate rate
  if ((ui->pLERate->isEnabled())&&(ui->pLERate->text().toDouble()==0))
  {
    g_messageWarning(this,tr("Apply"),tr("The rate cannot be zero"));
    return;
  }

  //Validate data for Split transaction
  if (iCat==ID_SPLIT)
  {
    double dTotal=0.0;
    for (int iRow=0;iRow<m_pModelSplit->rowCount();iRow++)
      dTotal+=m_pModelSplit->item(iRow,2)->text().toDouble();
    if (QString::number(dTotal,'f',2)!=QString::number(ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign(),'f',2))
    {
      g_messageWarning(this,tr("Apply"),tr("The split transactions don't match the amount"));
      return;
    }
  }

  //Get iIdPayee for the transaction (New payee Or Selected existing payee Or Transfer)
  int iIdPayee;
  if ((iCat>=0)||(iCat==ID_SPLIT))
  {
    QString s=ui->pCBPayee->lineEdit()->text().simplified();
    int iIdx=ui->pCBPayee->findText(s);

    //Add payee to "payee" table if it is new
    if (iIdx==-1)
    {
      QSqlTableModel ModelPayee;
      ModelPayee.setTable("payee");
      ModelPayee.insertRow(0);
      iIdPayee=g_generateId("payee");
      ModelPayee.setData(ModelPayee.index(0,0),iIdPayee);
      ModelPayee.setData(ModelPayee.index(0,1),s);
      ModelPayee.submitAll();
    }
    //Get IdPayee for the selected payee in the combobox
    else
      iIdPayee=m_pModelPayee->record(iIdx).value(0).toInt();
  }
  //Set iIdPayee as 'Transfer"
  else
    iIdPayee=ID_TRANSFER;

  //Set model to apply changes
  QSqlTableModel ModelTrans;
  ModelTrans.setTable("trans");
  ModelTrans.select();
  QSqlRecord Record;

  if (m_iAction==ActionEdit)
  {
    ModelTrans.setFilter("idx="+m_IdTrans.toString());
  }
  if (m_iAction==ActionAdd)
  {
    int iId=g_generateId("trans");
    ModelTrans.insertRow(0);
    ModelTrans.setData(ModelTrans.index(0,0),iId);
    ModelTrans.setData(ModelTrans.index(0,12),0);
    ModelTrans.setData(ModelTrans.index(0,14),0);
    ModelTrans.submitAll();
    ModelTrans.setFilter("idx="+QString::number(iId));
  }

  Record=ModelTrans.record(0);

  //Set Icon based on category without considering [account]
  if ((iCat>=0)||(iCat==ID_SPLIT))
  {
    Record.setValue(1,iCat);
    Record.setValue(13,"");
  }
  else
  {
    Record.setValue(1,0);
    Record.setValue(13,QString::number(ui->pLERate->text().toDouble(),'f',RATE_PREC));
  }

  //Set rest of fields for transaction
  Record.setValue(2,ui->pStatusWidget->state());
  Record.setValue(3,QDate::fromString(ui->pPBDate->text(),m_sDateFormat).toJulianDay());
  Record.setValue(4,m_pModelClass->record(ui->pCBClass->currentIndex()).value(0));
  Record.setValue(5,iIdPayee);
  Record.setValue(6,m_IdAcc);
  Record.setValue(7,iCat);
  Record.setValue(8,ui->pLECheque->text().simplified());
  Record.setValue(9,ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign());
  Record.setValue(10,ui->pRichTextEditor->toHtml());
  ModelTrans.setRecord(0,Record);
  ModelTrans.submitAll();

  g_createSplitTrans(Record.value(0),m_pModelSplit);
  g_transfer(Record.value(0));
  close();
}

void CTransEdit::on_pPBCancel_clicked()
{
  close();
}

void CTransEdit::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBCancel->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pRichTextEditor->setVisible(bChecked);
}

void CTransEdit::on_pCBCategory_activated(int iIndex)
{
  int iCat=ui->pCBCategory->itemData(iIndex).toInt();
  if ((iCat>=0)||(iCat==ID_SPLIT))
  {
    ui->pLERate->clear();
    ui->pLERate->setEnabled(false);
    ui->pCBPayee->setEnabled(true);
    ui->pCBClass->setEnabled(true);
    if (iCat==ID_SPLIT)
    {
      ui->pCBClass->setEnabled(false);
      ui->pCBClass->setCurrentIndex(0);
    }
  }
  else
  {
    bool bDiff;
    double dRate=g_transferRate(m_IdAcc.toInt(),iCat*-1,&bDiff);
    ui->pLERate->setText(QString::number(dRate,'f',RATE_PREC));
    if (bDiff)
      ui->pLERate->setEnabled(true);
    else
      ui->pLERate->setEnabled(false);

    ui->pCBPayee->setEnabled(false);
    ui->pCBPayee->lineEdit()->clear();
  }

  if (iCat==ID_SPLIT)
    ui->pPBSplit->setEnabled(true);
  else
    ui->pPBSplit->setEnabled(false);
}

void CTransEdit::on_pPBDate_clicked()
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

void CTransEdit::on_pPBCalc_clicked()
{
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+m_IdAcc.toString());
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

void CTransEdit::on_pPBSplit_clicked()
{
  g_pMainWindow->addFormSplit(m_pModelSplit,m_IdAcc,ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign(),true,false);
}

void CTransEdit::autoFillParams(const QString& sPayee)
{
  QString sPayee1=sPayee;
  sPayee1.replace(QString("'"),QString("''"));

  //Search if sPayee exists
  QSqlTableModel ModelPayee;
  ModelPayee.setTable("payee");
  ModelPayee.setFilter("name='"+sPayee1+"'");
  ModelPayee.select();

  //Return in case sPayee doesn't exist
  if (ModelPayee.rowCount()==0) return;

  //Search if sPayee was previously used
  QSqlTableModel ModelTrans;
  ModelTrans.setTable("trans");
  ModelTrans.setFilter("payeeid="+ModelPayee.record(0).value(0).toString());
  ModelTrans.setSort(3,Qt::DescendingOrder);
  ModelTrans.select();

  //Return in case sPayee wasn't previously used
  if (ModelTrans.rowCount()==0) return;

  //Auto select CLASS, CATEGORY and AMOUNT based on last used of sPayee
  g_refreshCombobox(ui->pCBClass,ModelTrans.record(0).value(4),false);
  g_refreshComboCategory(ui->pCBCategory,ModelTrans.record(0).value(7),m_IdAcc,false);
  if (ui->pLEAmount->text().isEmpty())
  {
    ui->pLEAmount->setText(QString::number(qAbs(ModelTrans.record(0).value(9).toDouble()),'f',2));
    ui->pSignSymbol->setSign(ModelTrans.record(0).value(9).toDouble());
  }

  //Update controls status
  on_pCBCategory_activated(ui->pCBCategory->currentIndex());
}
