#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "budgetedit.h"
#include "ui_budgetedit.h"
#include "global.h"
#include "mainwindow0.h"
#include "calendar.h"
#include "delegatebudgetedit.h"
#include "dr_profile.h"


CBudgetEdit::CBudgetEdit(const QVariant& Id, QWidget *pWidget) :
  CFormBase(pWidget),
  ui(new Ui::CBudgetEdit)
{
  ui->setupUi(this);

  //Setup children
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";

#ifdef MOBILE_PLATFORM
  ui->pWidgetEdit->hide();
  ui->pTVBudgetCat->hide();
#else
  ui->pPBList->hide();
  ui->pPBDetail->hide();
  ui->pPBCloseL->hide();
  ui->pPBCategories->hide();
#endif

  //Set members
  m_iScrollValue=0;

  //BUDGET: Set up model and list view
  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("budget");
  m_pModel->setSort(1,Qt::AscendingOrder);
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_pModel->select();

  ui->pListView->setModel(m_pModel);
  ui->pListView->setModelColumn(1);
  ui->pListView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //BUDGETCAT: Set up model, table view and delegate
  m_pDelegate=new CDelegateBudgetEdit(ui->pTVBudgetCat);
  m_pDelegate->setNumberPrecision(2);
  m_pDelegate->addColumnFormat(2,CBaseDelegate::FormatNumber);

  m_pModelBudgetCat=new QStandardItemModel(this);
  ui->pTVBudgetCat->setItemDelegate(m_pDelegate);
  ui->pTVBudgetCat->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pTVBudgetCat->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTVBudgetCat->setGridStyle(Qt::NoPen);
  ui->pTVBudgetCat->setEditTriggers(QAbstractItemView::SelectedClicked);
  ui->pTVBudgetCat->setModel(m_pModelBudgetCat);
  ui->pTVBudgetCat->verticalHeader()->hide();

  QHeaderView * pHeader=ui->pTVBudgetCat->horizontalHeader();
  pHeader->setSectionResizeMode(QHeaderView::Stretch);

  //Initiate Currency ComboBox
  m_pModelCurrency=new QSqlTableModel(this);
  m_pModelCurrency->setTable("currency");
  ui->pCBCurrency->setModel(m_pModelCurrency);
  ui->pCBCurrency->setModelColumn(1);

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Initiate Recurrent ComboBox
  ui->pCBRecurrent->addItem(tr("None"));
  ui->pCBRecurrent->addItem(tr("Weekly"));
  ui->pCBRecurrent->addItem(tr("Fortnightly"));
  ui->pCBRecurrent->addItem(tr("Monthly"));

  //Connect selection model signals
  connect(ui->pListView, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));

  //Connect controls to detect editing
  connect(m_pDelegate,SIGNAL(commitData(QWidget*)),this,SLOT(setDirty()));
  connect(ui->pLEName,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pCBClass,SIGNAL(currentIndexChanged(int)),this,SLOT(setDirty()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
  connect(g_pMainWindow,SIGNAL(updateBudget()),this,SLOT(updateView()));

  //Select the initial budget in case Id is valid
  if (Id.isValid())
  {
    QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,0),Qt::DisplayRole,Id,1,Qt::MatchExactly);
    if (lstIndex.size()==0) return;

    QModelIndex Index=m_pModel->index(lstIndex.at(0).row(),1);

    ui->pListView->setCurrentIndex(Index);
    ui->pListView->scrollTo(Index);
  }
}

CBudgetEdit::~CBudgetEdit()
{
  delete ui;
}

void CBudgetEdit::on_pPBApply_clicked()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (!Index.isValid()) return;

  //Validate budget name
  QString sName=ui->pLEName->text().simplified();
  if (sName.isEmpty()) return;
  QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
  if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
  {
    g_messageWarning(this,tr("Apply"),tr("The budget's name is already in use"));
    return;
  }

  //Validate dates
  QDate Date1=QDate::fromString(ui->pPBStart->text(),m_sDateFormat);
  QDate Date2=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);
  if (Date1.daysTo(Date2)<0)
  {
    g_messageWarning(this,tr("Budget"),tr("Range of dates are not correct"));
    return;
  }

  //BUDGET: Write data into database
  QSqlRecord Record=m_pModel->record(Index.row());
  Record.setValue(1,sName);
  Record.setValue(2,QDate::fromString(ui->pPBStart->text(),m_sDateFormat).toJulianDay());
  Record.setValue(3,QDate::fromString(ui->pPBEnd->text(),m_sDateFormat).toJulianDay());
  Record.setValue(4,ui->pCBRecurrent->currentIndex());
  Record.setValue(5,m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(0));
  Record.setValue(6,m_pModelClass->record(ui->pCBClass->currentIndex()).value(0));
  m_pModel->setRecord(Index.row(),Record);
  m_pModel->submitAll();

  //BUDGETCAT: Delete categories associated to this budget
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("DELETE FROM budgetcat WHERE budgetid="+Record.value(0).toString());
  Query.finish();

  //BUDGETCAT: Write data into database
  QSqlTableModel ModelBudgetCat;
  ModelBudgetCat.setTable("budgetcat");
  ModelBudgetCat.setEditStrategy(QSqlTableModel::OnManualSubmit);
  ModelBudgetCat.select();

  int iIdx=g_generateId("budgetcat");
  for (int i=0;i<m_pModelBudgetCat->rowCount();i++)
    if (m_pModelBudgetCat->item(i,2)->text().toDouble()!=0)
    {
      ModelBudgetCat.insertRow(0);
      ModelBudgetCat.setData(ModelBudgetCat.index(0,0),iIdx);
      ModelBudgetCat.submitAll();
      ModelBudgetCat.setFilter("idx="+QString::number(iIdx));

      ModelBudgetCat.setData(ModelBudgetCat.index(0,1),Record.value(0));
      ModelBudgetCat.setData(ModelBudgetCat.index(0,2),m_pModelBudgetCat->item(i,0)->text());
      ModelBudgetCat.setData(ModelBudgetCat.index(0,3),QString::number(m_pModelBudgetCat->item(i,2)->text().toDouble(),'f',2));
      iIdx++;
    }

  //Submit changes to budgetcat and clean form
  ModelBudgetCat.submitAll();
  cleanForm();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(false);
}

void CBudgetEdit::on_pPBCategories_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBClose->setVisible(!bChecked);
  ui->pPBList->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pTVBudgetCat->setVisible(bChecked);
}

void CBudgetEdit::on_pPBList_clicked()
{
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
}

void CBudgetEdit::on_pPBDetail_clicked()
{
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
}

void CBudgetEdit::on_pPBCloseL_clicked()
{
  close();
}

void CBudgetEdit::on_pPBClose_clicked()
{
  close();
}

void CBudgetEdit::on_pPBStart_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBStart->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBStart->setText(Date.toString(m_sDateFormat));
    if (ui->pCBRecurrent->currentIndex()>0)
      ui->pPBEnd->setText(g_endRecurrentDate(Date,static_cast<Recurrent>(ui->pCBRecurrent->currentIndex())).toString(m_sDateFormat));
    setDirty();
  }
}

void CBudgetEdit::on_pPBEnd_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBEnd->setText(Date.toString(m_sDateFormat));
    setDirty();
  }
}

void CBudgetEdit::on_pCBRecurrent_currentIndexChanged(int iIndex)
{
  QDate Date;

  switch (iIndex)
  {
    case 0:
      ui->pPBEnd->setEnabled(true);
      break;
    case 1:
      ui->pPBEnd->setEnabled(false);
      Date=QDate::fromString(ui->pPBStart->text(),m_sDateFormat);
      ui->pPBEnd->setText(g_endRecurrentDate(Date,RecurrentWeekly).toString(m_sDateFormat));
      break;
    case 2:
      ui->pPBEnd->setEnabled(false);
      Date=QDate::fromString(ui->pPBStart->text(),m_sDateFormat);
      ui->pPBEnd->setText(g_endRecurrentDate(Date,RecurrentFortnightly).toString(m_sDateFormat));
      break;
    case 3:
      ui->pPBEnd->setEnabled(false);
      Date=QDate::fromString(ui->pPBStart->text(),m_sDateFormat);
      ui->pPBEnd->setText(g_endRecurrentDate(Date,RecurrentMonthly).toString(m_sDateFormat));
      break;
    default:
      break;
  }
  setDirty();
}

void CBudgetEdit::on_pCBCurrency_currentIndexChanged(int iIndex)
{
  setDirty();

  //Write currency symbol
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+m_pModelCurrency->record(iIndex).value(0).toString());
  ModelCurr.select();
  m_pModelBudgetCat->horizontalHeaderItem(2)->setText(tr("Budget ")+"["+ModelCurr.record(0).value(3).toString()+"]");
}

void CBudgetEdit::addItem()
{
  if (!isVisible()) return;

  int iId=g_generateId("budget");
  m_pModel->insertRow(0);
  m_pModel->setData(m_pModel->index(0,0),iId);
  m_pModel->setData(m_pModel->index(0,2),QDate::currentDate().toJulianDay());
  m_pModel->setData(m_pModel->index(0,3),g_endRecurrentDate(QDate::currentDate(),RecurrentMonthly).toJulianDay());
  m_pModel->setData(m_pModel->index(0,4),0);
  m_pModel->setData(m_pModel->index(0,5),0);
  m_pModel->setData(m_pModel->index(0,6),ID_ANY);
  m_pModel->submitAll();

  ui->pListView->setCurrentIndex(m_pModel->index(0,1));
  fillForm(m_pModel->index(0,1));
  ui->pLEName->setFocus();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CBudgetEdit::deleteItem()
{
  if (!isVisible()) return;

  ui->pListView->setFocus();

  //Obtain the selected account
  QModelIndexList lstIndex=ui->pListView->selectionModel()->selectedIndexes();
  if (lstIndex.count()!=1) return;
  QModelIndex Index = lstIndex.at(0);

  //Confirm deletion
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete selected budget?"))==QMessageBox::No) return;

  //Delete categories associated to this budget
  QSqlQuery Query;
  QSqlRecord Record = m_pModel->record(Index.row());
  Query.setForwardOnly(true);
  Query.exec("DELETE FROM budgetcat WHERE budgetid="+Record.value(0).toString());

  //Delete account from table and reset form
  m_pModel->removeRow(Index.row());
  m_pModel->submitAll();
  cleanForm();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(false);
}

void CBudgetEdit::showEvent( QShowEvent */*pEvent*/ )
{
  //Update combo boxes
  g_refreshCombobox(ui->pCBCurrency,QVariant(),false);
  g_refreshCombobox(ui->pCBClass,QVariant(),true);

  //Update TVBudgetCat
  createModelBudgetCat();

  //Update ListView
  updateView();

  //Restore scrollbar value
  ui->pListView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the ListView
  ui->pListView->setFocus();
}

void CBudgetEdit::hideEvent ( QHideEvent */*pEvent*/ )
{
  //Save scrollbar value
  m_iScrollValue=ui->pListView->verticalScrollBar()->value();
}

void CBudgetEdit::updateView()
{
  //Refresh ListView
  g_refreshView(ui->pListView,false);

  //Load Table View if there is a budget selected
  if (ui->pListView->currentIndex().isValid())
  {
    fillForm(ui->pListView->currentIndex());
  }
}

void CBudgetEdit::createModelBudgetCat()
{
  //Load category model
  QSqlTableModel ModelCat;
  ModelCat.setTable("category");
  ModelCat.select();
  ModelCat.setFilter("idx!="+QString::number(ID_SPLIT));

  //Create budgetcat model
  int iNumRows=g_rowCount(ModelCat);
  m_pModelBudgetCat->clear();
  m_pModelBudgetCat->setRowCount(iNumRows);
  m_pModelBudgetCat->setColumnCount(3);

  //Fill out columns
  for (int i=0;i<iNumRows;i++)
  {
    //Column 0: category.idx
    QStandardItem *pItem;
    pItem=new QStandardItem(ModelCat.record(i).value(0).toString());
    m_pModelBudgetCat->setItem(i,0,pItem);

    //Column 1: category.name
    pItem=new QStandardItem(ModelCat.record(i).value(1).toString());
    pItem->setIcon(QIcon(ModelCat.record(i).value(2).toString()));
    pItem->setEditable(false);
    m_pModelBudgetCat->setItem(i,1,pItem);

    //Column 2: 0.00
    pItem=new QStandardItem("0");
    pItem->setEditable(true);
    m_pModelBudgetCat->setItem(i,2,pItem);
  }
  m_pModelBudgetCat->sort(1);

  //Set Title name for column 1
  QStandardItem *pItem=new QStandardItem(tr("Category"));
  QFont Font=ui->pTVBudgetCat->font();
  pItem->setFont(Font);
  m_pModelBudgetCat->setHorizontalHeaderItem(1,pItem);

  //Set Title name for column 2
  pItem=new QStandardItem(tr("Budget"));
  pItem->setFont(Font);
  m_pModelBudgetCat->setHorizontalHeaderItem(2,pItem);

  //Hide column 0
  ui->pTVBudgetCat->hideColumn(0);
}

void CBudgetEdit::cleanBudgetCat()
{
  for (int i=0; i<m_pModelBudgetCat->rowCount(); i++)
    m_pModelBudgetCat->item(i,2)->setText("0");
  m_pModelBudgetCat->horizontalHeaderItem(2)->setText(tr("Budget"));
}

void CBudgetEdit::loadBudgetCat(const QSqlRecord& Record)
{
  QString sFilter="budgetid="+Record.value(0).toString();

  for (int i=0; i<m_pModelBudgetCat->rowCount(); i++)
  {
    QVariant Val;
    if (g_findRecordFromTable("budgetcat","categoryid",m_pModelBudgetCat->item(i,0)->text(),"amount",&Val,sFilter))
      m_pModelBudgetCat->item(i,2)->setText(Val.toString());
    else
      m_pModelBudgetCat->item(i,2)->setText("0");
  }

  //Write currency symbol
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+Record.value(5).toString());
  ModelCurr.select();
  m_pModelBudgetCat->horizontalHeaderItem(2)->setText(tr("Budget ")+"["+ModelCurr.record(0).value(3).toString()+"]");
}

void CBudgetEdit::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  //Load Comboboxes
  g_refreshCombobox(ui->pCBCurrency,m_pModel->record(Index.row()).value(5),false);
  g_refreshCombobox(ui->pCBClass,m_pModel->record(Index.row()).value(6),true);

  //Load controls
  ui->pLEName->setText(m_pModel->record(Index.row()).value(1).toString());
  ui->pCBRecurrent->setCurrentIndex(m_pModel->record(Index.row()).value(4).toInt());
  ui->pPBStart->setText(QDate::fromJulianDay(m_pModel->record(Index.row()).value(2).toInt()).toString(m_sDateFormat));
  ui->pPBEnd->setText(QDate::fromJulianDay(m_pModel->record(Index.row()).value(3).toInt()).toString(m_sDateFormat));

  //Load budgetcat table view
  loadBudgetCat(m_pModel->record(Index.row()));

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CBudgetEdit::setDirty()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (Index.isValid())
    ui->pPBApply->setEnabled(true);
}

void CBudgetEdit::cleanForm()
{
  ui->pLEName->clear();
  ui->pPBStart->setText("");
  ui->pPBEnd->setText("");
  ui->pCBRecurrent->setCurrentIndex(0);
  ui->pCBCurrency->setCurrentIndex(0);
  ui->pCBClass->setCurrentIndex(0);
  ui->pListView->setCurrentIndex(QModelIndex());
  cleanBudgetCat();
}

void CBudgetEdit::enableForm(bool bEnable)
{
  ui->pLEName->setEnabled(bEnable);
  ui->pPBStart->setEnabled(bEnable);
  ui->pPBEnd->setEnabled(bEnable && (ui->pCBRecurrent->currentIndex()==0));
  ui->pCBRecurrent->setEnabled(bEnable);
  ui->pCBCurrency->setEnabled(bEnable);
  ui->pCBClass->setEnabled(bEnable);
  ui->pTVBudgetCat->setEnabled(bEnable);
}

