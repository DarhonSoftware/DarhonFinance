#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "scheduledtrans.h"
#include "ui_scheduledtrans.h"
#include "global.h"
#include "basedelegate.h"
#include "calendar.h"
#include "mainwindow0.h"
#include "splittrans.h"
#include "dr_profile.h"

CScheduledTrans::CScheduledTrans(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CScheduledTrans)
{
  ui->setupUi(this);
  g_updateFontSize(ui->pTableView);

  //Setup children
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";
  ui->pLEAmount->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLEFrequency->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLETimesLimit->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pSignSymbol->setSign(0);
  ui->pPBDateLimit->hide();
  ui->pLETimesLimit->hide();
#ifdef MOBILE_PLATFORM
  ui->pWidgetEdit->setVisible(false);
  ui->pRichTextEditor->setVisible(false);
#else
  ui->pPBList->hide();
  ui->pPBDetail->hide();
  ui->pPBCloseL->hide();
  ui->pPBMemo->hide();
#endif

  //Set members
  m_iScrollValue=0;

  //Block Schedule Timer
  g_pMainWindow->blockTimerSch(true);

  //Set de validators
  QIntValidator *pValidatorInt=new QIntValidator(this);
  pValidatorInt->setBottom(0);
  ui->pLEFrequency->setValidator(pValidatorInt);
  ui->pLETimesLimit->setValidator(pValidatorInt);
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  ui->pLEAmount->setValidator(pValidatorAmount);

  //Set ModelSplit
  m_pModelSplit=new QStandardItemModel(this);

  //Set model schedule for view
  m_pModel=new QSqlRelationalTableModel(this);
  m_pModel->setTable("schedule");
  m_pModel->setRelation(1,QSqlRelation("account","idx","name"));
  m_pModel->setRelation(2,QSqlRelation("payee","idx","name"));
  m_pModel->setSort(1,Qt::AscendingOrder);
  m_pModel->setHeaderData(1,Qt::Horizontal,tr("Account"));
  m_pModel->setHeaderData(2,Qt::Horizontal,tr("Payee"));
  m_pModel->setHeaderData(3,Qt::Horizontal,tr("Category"));
  m_pModel->setHeaderData(5,Qt::Horizontal,tr("Amount"));
  m_pModel->setHeaderData(6,Qt::Horizontal,tr("Date"));
  m_pModel->setHeaderData(8,Qt::Horizontal,tr("Freq"));
  m_pModel->setHeaderData(10,Qt::Horizontal,tr("Unit"));
  m_pModel->setFilter("split<"+QString::number(SPLIT));
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_pModel->select();

  //Set Delegate features
  CBaseDelegate* pDelegate=new CBaseDelegate(ui->pTableView);
  pDelegate->setNumberPrecision(2);
  pDelegate->updateCategoryData();
  pDelegate->setDateFormat(g_dateFormat());
  pDelegate->addColumnFormat(3,CBaseDelegate::FormatCategory);
  pDelegate->addColumnFormat(5,CBaseDelegate::FormatNumber);
  pDelegate->addColumnFormat(6,CBaseDelegate::FormatDate);
  pDelegate->addColumnFormat(8,CBaseDelegate::FormatFrequency);
  pDelegate->addColumnFormat(10,CBaseDelegate::FormatUnit);

  //Set TableView features
  ui->pTableView->setSortingEnabled(true);
  ui->pTableView->setItemDelegate(pDelegate);
  ui->pTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::NoPen);
  ui->pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->pTableView->setModel(m_pModel);
  ui->pTableView->setColumnHidden(0,true);
  ui->pTableView->setColumnHidden(4,true);
  ui->pTableView->setColumnHidden(7,true);
  ui->pTableView->setColumnHidden(9,true);
  ui->pTableView->setColumnHidden(11,true);
  ui->pTableView->setColumnHidden(12,true);
  ui->pTableView->resizeColumnsToContents();

  //Set HeaderView features
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionsClickable(true);
  pHeader->setSectionsMovable(true);
  pHeader->setSortIndicatorShown(true);
  pHeader->setSortIndicator(1,Qt::AscendingOrder);
  pHeader->setSectionResizeMode(QHeaderView::Interactive);
  pHeader->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(pHeader,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(menuColumnsChooser(QPoint)));
  ui->pTableView->verticalHeader()->hide();

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
  m_pModelPayee->setFilter("idx>=0");
  ui->pCBPayee->setModel(m_pModelPayee);
  ui->pCBPayee->setModelColumn(1);

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Update HEADER from database
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  //Load HorizontalHeader state
  QByteArray ByteArray=Table.record(0).value(11).toByteArray();
  ui->pTableView->horizontalHeader()->restoreState(ByteArray);

  //Connect selection model signals
  connect(ui->pTableView,SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));

  //Connect controls to detect editing
  connect(ui->pPBSplit,SIGNAL(clicked()),this,SLOT(setDirty()));
  connect(ui->pLEFrequency,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pCBPayee->lineEdit(),SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEAmount,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pSignSymbol,SIGNAL(signChanged()),this,SLOT(setDirty()));
  connect(ui->pCBUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pCBAccount, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pCBPayee, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pCBClass, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pCBCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pRichTextEditor,SIGNAL(textEdited()),this,SLOT(setDirty()));
  connect(ui->pPBDate,SIGNAL(clicked()),this,SLOT(setDirty()));
  connect(ui->pCBCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStatusControls()));
  connect(ui->pCBLimits, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pLETimesLimit,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pPBDateLimit,SIGNAL(clicked()),this,SLOT(setDirty()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
}

CScheduledTrans::~CScheduledTrans()
{
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  QSqlRecord Record=Table.record(0);
  Record.setValue(11,ui->pTableView->horizontalHeader()->saveState());

  Table.setRecord(0,Record);
  Table.submitAll();

  //Unblock Schedule Timer
  if (g_pMainWindow) g_pMainWindow->blockTimerSch(false);

  delete ui;
}

void CScheduledTrans::on_pPBApply_clicked()
{
  QModelIndex Index=ui->pTableView->currentIndex();
  if (!Index.isValid()) return;

  //Validate Payee
  if (ui->pCBPayee->lineEdit()->text().simplified()==TRANSFER_STR)
  {
    g_messageWarning(this,tr("Apply"),tr("The payee cannot use the reserved name ") + TRANSFER_QTD);
    return;
  }

  //Validate data
  if (ui->pLEFrequency->text().toInt()==0)
  {
    g_messageWarning(this,tr("Apply"),tr("The frequency cannot be zero"));
    return;
  }

  //Get Id from category combobox
  int iCat=ui->pCBCategory->itemData(ui->pCBCategory->currentIndex()).toInt();

  //Validate data
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

  //Update all fields
  QDate Date=QDate::fromString(ui->pPBDate->text(),m_sDateFormat);

  QSqlRecord Record=m_pModel->record(Index.row());
  Record.setValue(1,m_pModelAccount->record(ui->pCBAccount->currentIndex()).value(0));
  Record.setValue(2,iIdPayee);
  Record.setValue(3,iCat);
  Record.setValue(4,m_pModelClass->record(ui->pCBClass->currentIndex()).value(0));
  Record.setValue(5,ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign());
  Record.setValue(6,Date.toJulianDay());
  if (Date.daysInMonth()==Date.day())
    Record.setValue(7,31);
  else
    Record.setValue(7,Date.day());
  Record.setValue(8,ui->pLEFrequency->text().toInt());
  Record.setValue(9,ui->pRichTextEditor->toHtml());
  Record.setValue(10,ui->pCBUnit->currentIndex());

  switch (ui->pCBLimits->currentIndex()) {
    case 0:
      Record.setValue(13,"0#");
      break;
    case 1:
      Record.setValue(13,"1#"+QString::number(QDate::fromString(ui->pPBDateLimit->text(),m_sDateFormat).toJulianDay()));
      break;
    case 2:
      Record.setValue(13,"2#"+ui->pLETimesLimit->text());
      break;
    default:
      break;
  }

  m_pModel->setRecord(Index.row(),Record);
  m_pModel->submitAll();

  createSplitSch(Record.value(0));
  m_pModel->select();
  cleanForm();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(false);
}

void CScheduledTrans::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBClose->setVisible(!bChecked);
  ui->pPBList->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pRichTextEditor->setVisible(bChecked);
}

void CScheduledTrans::on_pPBList_clicked()
{
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
}

void CScheduledTrans::on_pPBDetail_clicked()
{
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
}

void CScheduledTrans::on_pPBClose_clicked()
{
  close();
}

void CScheduledTrans::on_pPBCloseL_clicked()
{
  close();
}

void CScheduledTrans::on_pPBDate_clicked()
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

void CScheduledTrans::on_pPBDateLimit_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBDateLimit->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBDateLimit->setText(Date.toString(m_sDateFormat));
  }
}

void CScheduledTrans::on_pCBAccount_currentIndexChanged(int iIndex)
{
  QVariant IdAcc=m_pModelAccount->record(iIndex).value(0);
  g_refreshComboCategory(ui->pCBCategory,QVariant(),IdAcc,false);
  updateCurrencySymbol();
  m_pModelSplit->clear();
}

void CScheduledTrans::on_pCBLimits_currentIndexChanged(int iIndex)
{
  switch (iIndex) {
    case 0:
      ui->pPBDateLimit->setVisible(false);
      ui->pLETimesLimit->setVisible(false);
      break;
    case 1:
      ui->pPBDateLimit->setVisible(true);
      ui->pLETimesLimit->setVisible(false);
      break;
    case 2:
      ui->pPBDateLimit->setVisible(false);
      ui->pLETimesLimit->setVisible(true);
      break;
    default:
      break;
  }
}

void CScheduledTrans::on_pPBSplit_clicked()
{
  QVariant IdAcc=m_pModelAccount->record(ui->pCBAccount->currentIndex()).value(0);
  g_pMainWindow->addFormSplit(m_pModelSplit,IdAcc,ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign(),false,false);
}

void CScheduledTrans::addItem()
{
  if (!isVisible()) return;

  QDate Date=QDate::currentDate();

  int iId=g_generateId("schedule");
  m_pModel->insertRow(0);
  m_pModel->setData(m_pModel->index(0,0),iId);
  m_pModel->setData(m_pModel->index(0,1),m_pModelAccount->record(0).value(0));
  m_pModel->setData(m_pModel->index(0,2),0);
  m_pModel->setData(m_pModel->index(0,3),0);
  m_pModel->setData(m_pModel->index(0,4),0);
  m_pModel->setData(m_pModel->index(0,5),0);
  m_pModel->setData(m_pModel->index(0,6),Date.toJulianDay());
  m_pModel->setData(m_pModel->index(0,7),Date.day());
  m_pModel->setData(m_pModel->index(0,8),1);
  m_pModel->setData(m_pModel->index(0,10),0);
  m_pModel->setData(m_pModel->index(0,12),0);
  m_pModel->setData(m_pModel->index(0,13),"0#");

  m_pModel->submitAll();

  QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,0),Qt::DisplayRole,iId,1,Qt::MatchExactly);

  ui->pTableView->setCurrentIndex(lstIndex.at(0));
  fillForm(lstIndex.at(0));
  ui->pLEFrequency->setFocus();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CScheduledTrans::deleteItem()
{
  if (!isVisible()) return;

  QModelIndexList lstIndex=ui->pTableView->selectionModel()->selectedRows();
  if (lstIndex.count()==0) return;
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete %n selected scheduled transaction(s)?",0,lstIndex.count()))==QMessageBox::No) return;

  ui->pTableView->setFocus();

  //Mark the records to be removed
  for (int i=0; i<lstIndex.count(); i++)
  {
    QSqlQuery Query;
    Query.setForwardOnly(true);
    QModelIndex Index = lstIndex.at(i);

    //In case the row is a split transaction, mark all children
    if (m_pModel->record(Index.row()).value(3).toInt()==ID_SPLIT)
    {
      int iSplit=m_pModel->record(Index.row()).value(12).toInt();
      Query.exec("UPDATE schedule SET aux='x' WHERE split="+QString::number(SPLIT+iSplit));
      Query.exec("UPDATE schedule SET aux='x' WHERE split="+QString::number(iSplit));
    }
    else
    {
      m_pModel->setData(m_pModel->index(Index.row(),11),"x");
    }
  }
  m_pModel->submitAll();

  //Removed the records
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("DELETE FROM schedule WHERE aux='x'");

  //Update TableView
  m_pModel->select();
  cleanForm();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(false);
}

void CScheduledTrans::cleanForm()
{
  m_pModelPayee->select();
  ui->pLEAmount->clear();
  ui->pSignSymbol->setSign(0);
  ui->pLEFrequency->clear();
  ui->pCBAccount->setCurrentIndex(0);
  ui->pCBCategory->setCurrentIndex(0);
  ui->pCBClass->setCurrentIndex(0);
  ui->pCBPayee->setCurrentIndex(0);
  ui->pCBUnit->setCurrentIndex(0);
  ui->pRichTextEditor->clear();
  ui->pPBDate->setText("");
  ui->pCBLimits->setCurrentIndex(0);
  ui->pLETimesLimit->setText("");
  ui->pPBDateLimit->setText("");
  ui->pTableView->setCurrentIndex(QModelIndex());
}

void CScheduledTrans::enableForm(bool bEnable)
{
  ui->pLEAmount->setEnabled(bEnable);
  ui->pSignSymbol->setEnabled(bEnable);
  ui->pLEFrequency->setEnabled(bEnable);
  ui->pCBAccount->setEnabled(bEnable);
  ui->pCBCategory->setEnabled(bEnable);
  ui->pCBClass->setEnabled(bEnable);
  ui->pCBPayee->setEnabled(bEnable);
  ui->pCBUnit->setEnabled(bEnable);
  ui->pRichTextEditor->setEnabled(bEnable);
  ui->pPBDate->setEnabled(bEnable);
  ui->pCBLimits->setEnabled(bEnable);
  ui->pLETimesLimit->setEnabled(bEnable);
  ui->pPBDateLimit->setEnabled(bEnable);
}

void CScheduledTrans::updateCurrencySymbol()
{
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+m_pModelAccount->record(ui->pCBAccount->currentIndex()).value(5).toString());
  ModelCurr.select();
  ui->pLCurrency->setText(ModelCurr.record(0).value(3).toString());
}

void CScheduledTrans::createSplitSch(const QVariant& Id1)
{
  //Set Model for Record1
  QSqlTableModel ModelSch1;
  ModelSch1.setTable("schedule");
  ModelSch1.setFilter("idx="+Id1.toString());
  ModelSch1.select();
  QSqlRecord Record1=ModelSch1.record(0);
  int iSplit=Record1.value(12).toInt();

  //Delete any previous childdren records if exist
  if (iSplit>0)
  {
    QSqlQuery Query;
    Query.setForwardOnly(true);
    Query.exec("DELETE FROM schedule WHERE split="+QString::number(SPLIT+iSplit));
    Query.exec("DELETE FROM schedule WHERE split="+QString::number(iSplit)+" and idx!="+Id1.toString());
  }

  //If Category chosen is SPLIT
  if (Record1.value(3).toInt()==ID_SPLIT)
  {
    //Set split number in case is a new transaction
    if (iSplit==0)
    {
      QSqlQuery Query;
      Query.setForwardOnly(true);
      Query.exec("SELECT MAX(split) FROM schedule WHERE split<"+QString::number(SPLIT));
      if (Query.next())
      iSplit=Query.value(0).toInt()+1;
      if (iSplit==SPLIT) iSplit=SPLIT-1;

      Record1.setValue(12,iSplit);
      ModelSch1.setRecord(0,Record1);
      ModelSch1.submitAll();
    }

    //Create one transaction per row of the ModelSplit
    QSqlTableModel ModelSch2;
    ModelSch2.setTable("schedule");
    ModelSch2.select();
    for (int iRow=0;iRow<m_pModelSplit->rowCount();iRow++)
    {
      int iId=g_generateId("schedule");
      ModelSch2.insertRow(0);
      ModelSch2.setData(ModelSch2.index(0,0),iId);
      ModelSch2.submitAll();
      ModelSch2.setFilter("idx="+QString::number(iId));

      QSqlRecord Record2=ModelSch2.record(0);

      Record2.setValue(3,m_pModelSplit->item(iRow,0)->text().toInt()); //Category
      Record2.setValue(4,m_pModelSplit->item(iRow,1)->text().toInt()); //Class
      Record2.setValue(5,m_pModelSplit->item(iRow,2)->text().toDouble()); //Amount
      Record2.setValue(9,m_pModelSplit->item(iRow,3)->text()); //Note
      Record2.setValue(12,iSplit+SPLIT);

      ModelSch2.setRecord(0,Record2);
      ModelSch2.submitAll();
    }
  }
  else
  {
    Record1.setValue(12,0);
    ModelSch1.setRecord(0,Record1);
    ModelSch1.submitAll();
  }
}

void CScheduledTrans::updateStatusControls()
{
  int iCat=ui->pCBCategory->itemData(ui->pCBCategory->currentIndex()).toInt();
  if ((iCat>=0)||(iCat==ID_SPLIT))
  {
    ui->pCBPayee->setEnabled(true);
  }
  else
  {
    ui->pCBPayee->setEnabled(false);
    ui->pCBPayee->lineEdit()->clear();
  }

  if (iCat==ID_SPLIT)
  {
    ui->pPBSplit->setEnabled(true);
    ui->pCBClass->setEnabled(false);
    ui->pCBClass->setCurrentIndex(0);
  }
  else
  {
    ui->pPBSplit->setEnabled(false);
    ui->pCBClass->setEnabled(true);
  }
}

void CScheduledTrans::showEvent( QShowEvent */*pEvent*/ )
{
  //Block the currentChanged signal from the SelectionModel to refresh the Model and set the current Index
  g_refreshView(ui->pTableView,false);

  //Update Comboxes
  g_refreshCombobox(ui->pCBPayee,QVariant(),false);
  g_refreshCombobox(ui->pCBClass,QVariant(),false);
  g_refreshCombobox(ui->pCBAccount,QVariant(),false);
  QVariant IdAcc=m_pModelAccount->record(ui->pCBAccount->currentIndex()).value(0);
  g_refreshComboCategory(ui->pCBCategory,QVariant(),IdAcc,false);
  updateCurrencySymbol();

  //Update FontSize
  g_updateFontSize(ui->pTableView);

  //Update Delegate for Category
  CBaseDelegate* pDelegate=dynamic_cast<CBaseDelegate*>(ui->pTableView->itemDelegate());
  pDelegate->updateCategoryData();
  pDelegate->setDateFormat(g_dateFormat());

  //Restore scrollbar value
  ui->pTableView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the TableView
  ui->pTableView->setFocus();
}

void CScheduledTrans::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  m_iScrollValue=ui->pTableView->verticalScrollBar()->value();
}

void CScheduledTrans::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  //Clean SplitModel
  m_pModelSplit->clear();

  //Select Comboboxes for fields with QSqlRelation
  ui->pCBAccount->blockSignals(true);
  ui->pCBAccount->setCurrentIndex(ui->pCBAccount->findText(m_pModel->record(Index.row()).value(1).toString()));
  ui->pCBAccount->blockSignals(false);
  ui->pCBPayee->setCurrentIndex(ui->pCBPayee->findText(m_pModel->record(Index.row()).value(2).toString()));

  //Select Comboboxes for fields without QSqlRelation
  g_refreshCombobox(ui->pCBClass,m_pModel->record(Index.row()).value(4),false);
  ui->pCBUnit->setCurrentIndex(m_pModel->record(Index.row()).value(10).toInt());
  QVariant IdAcc=m_pModelAccount->record(ui->pCBAccount->currentIndex()).value(0);
  g_refreshComboCategory(ui->pCBCategory,m_pModel->record(Index.row()).value(3),IdAcc,false);

  ui->pLEAmount->setText(QString::number(qAbs(m_pModel->record(Index.row()).value(5).toDouble()),'f',2));
  ui->pSignSymbol->setSign(m_pModel->record(Index.row()).value(5).toDouble());
  ui->pLEFrequency->setText(m_pModel->record(Index.row()).value(8).toString());
  ui->pRichTextEditor->setHtml(m_pModel->record(Index.row()).value(9).toString());
  ui->pPBDate->setText(QDate::fromJulianDay(m_pModel->record(Index.row()).value(6).toInt()).toString(m_sDateFormat));
  updateCurrencySymbol();

  //Read Limits
  QStringList lsts=m_pModel->record(Index.row()).value(13).toString().split('#');
  ui->pCBLimits->setCurrentIndex(lsts.at(0).toInt());
  switch (lsts.at(0).toInt()) {
    case 0:
      ui->pPBDateLimit->setText(QDate::currentDate().toString(m_sDateFormat));
      ui->pLETimesLimit->setText("");
      break;
    case 1:
      ui->pPBDateLimit->setText(QDate::fromJulianDay(lsts.at(1).toInt()).toString(m_sDateFormat));
      ui->pLETimesLimit->setText("");
      break;
    case 2:
      ui->pPBDateLimit->setText(QDate::currentDate().toString(m_sDateFormat));
      ui->pLETimesLimit->setText(lsts.at(1));
      break;
    default:
      break;
  }

  //Set the Split status
  if (m_pModel->record(Index.row()).value(3).toInt()==ID_SPLIT)
  {
    //Load childdren transactions
    QSqlTableModel ModelSch;
    ModelSch.setTable("schedule");
    ModelSch.setFilter("split="+QString::number(SPLIT+m_pModel->record(Index.row()).value(12).toInt()));
    ModelSch.select();

    int iRowCount=g_rowCount(ModelSch);
    for (int iRow=0;iRow<iRowCount;iRow++)
    {
      m_pModelSplit->setItem(iRow,0,new QStandardItem(ModelSch.record(iRow).value(3).toString())); //Category
      m_pModelSplit->setItem(iRow,1,new QStandardItem(ModelSch.record(iRow).value(4).toString())); //Class
      m_pModelSplit->setItem(iRow,2,new QStandardItem(ModelSch.record(iRow).value(5).toString())); //Amount
      m_pModelSplit->setItem(iRow,3,new QStandardItem(ModelSch.record(iRow).value(9).toString())); //Note
      m_pModelSplit->setItem(iRow,4,new QStandardItem(""));
    }
  }

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
  updateStatusControls();
}

void CScheduledTrans::setDirty()
{
  QModelIndex Index=ui->pTableView->currentIndex();
  if (Index.isValid())
    ui->pPBApply->setEnabled(true);
}

void CScheduledTrans::menuColumnsChooser(QPoint Point)
{
  QMenu Menu;
  QList<QAction*> lstActions;
  QAction *pAction;
  QHeaderView *pHeader=ui->pTableView->horizontalHeader();

  //Create context menu using the applicable columns from the Header
  for (int i=0;i<pHeader->count();i++)
  {
    if ((i!=0)&&(i!=4)&&(i!=7)&&(i!=9)&&(i!=11)&&(i!=12))
    {
      pAction=Menu.addAction(pHeader->model()->headerData(i,Qt::Horizontal).toString());
      pAction->setCheckable(true);
      pAction->setChecked(!pHeader->isSectionHidden(i));
    }
    else
      pAction=0;
    lstActions.append(pAction);
  }

#ifdef MOBILE_PLATFORM
  int iMenuWidth=Menu.sizeHint().width();
  if (Point.x()>ui->pTableView->viewport()->width()-1.5*iMenuWidth)
    Point.rx()-=1.5*iMenuWidth;
  else
    Point.rx()+=0.5*iMenuWidth;
  Menu.exec(mapToGlobal(QPoint(Point.x(),(g_pMainWindow->height()-Menu.sizeHint().height())/2)));
#else
  Point=ui->pTableView->horizontalHeader()->mapToGlobal(Point);
  Menu.exec(Point);
#endif

  //Set columns according to user's modification
  for (int i=0;i<lstActions.size();i++)
    if (lstActions.at(i)) pHeader->setSectionHidden(i,!lstActions.at(i)->isChecked());
}
