#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "budgetview.h"
#include "ui_budgetview.h"
#include "global.h"
#include "basedelegate.h"
#include "mainwindow0.h"

CBudgetView::CBudgetView(QWidget *pWidget) :
  CFormBase(pWidget),
  ui(new Ui::CBudgetView)
{
  ui->setupUi(this);
  m_iScrollValue=0;
  g_updateFontSize(ui->pTableView);

  //Set up model, table view and delegate
  m_pDelegate=new CBaseDelegate(ui->pTableView);
  m_pDelegate->setNumberPrecision(2);
  m_pDelegate->addColumnFormat(2,CBaseDelegate::FormatProgress);
  m_pDelegate->addColumnFormat(3,CBaseDelegate::FormatNumber);
  m_pDelegate->addColumnFormat(4,CBaseDelegate::FormatNumber);

  m_pModel=new QStandardItemModel(this);
  ui->pTableView->setItemDelegate(m_pDelegate);
  ui->pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::NoPen);
  ui->pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->pTableView->setModel(m_pModel);
  ui->pTableView->verticalHeader()->hide();

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Update CLASS combobox from database
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  //Load ComboBox selection states
  g_refreshCombobox(ui->pCBClass,Table.record(0).value(24),true);

  //Set connections
  connect(g_pMainWindow,SIGNAL(newTransaction()),this,SLOT(updateView()));
  connect(g_pMainWindow,SIGNAL(updateBudget()),this,SLOT(updateView()));
}

CBudgetView::~CBudgetView()
{
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  QSqlRecord Record=Table.record(0);
  int iIdx=ui->pCBClass->currentIndex();
  if (iIdx==-1)
    Record.setValue(24,ID_ANY);
  else
    Record.setValue(24,m_pModelClass->record(ui->pCBClass->currentIndex()).value(0));

  Table.setRecord(0,Record);
  Table.submitAll();

  delete ui;
}

void CBudgetView::on_pTableView_activated(QModelIndex Index)
{
  //When user click on the first column only select row
  if (Index.column()==1) return;

  QString sId=m_pModel->item(Index.row(),0)->text();
  g_pMainWindow->addForm("CBudgetRep",sId);
}

void CBudgetView::on_pCBClass_currentIndexChanged(int /*iIndex*/)
{
  updateView();
}

void CBudgetView::on_pPBBudgets_clicked()
{
  QVariant IdBudget;
  if (ui->pTableView->selectionModel()->selectedRows().size()==1)
    IdBudget=m_pModel->item(ui->pTableView->currentIndex().row(),0)->text();

  g_pMainWindow->addForm("CBudgetEdit",IdBudget);
}

void CBudgetView::on_pPBClose_clicked()
{
  close();
}

void CBudgetView::showEvent( QShowEvent */*pEvent*/ )
{
  //Update Combox
  g_refreshCombobox(ui->pCBClass,QVariant(),true);

  //Remake model
  g_updateFontSize(ui->pTableView);
  updateView();

  //Restore scrollbar value
  ui->pTableView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus to the TableView
  ui->pTableView->setFocus();
}

void CBudgetView::hideEvent ( QHideEvent */*pEvent*/ )
{
  //Save scrollbar value
  m_iScrollValue=ui->pTableView->verticalScrollBar()->value();
}

void CBudgetView::updateView()
{
  //Filter model based on combobox
  QString sFilter;
  g_filterText(ui->pCBClass,"classid",sFilter,true);

  //Set model for table "budget"
  QSqlTableModel ModelBudget;
  ModelBudget.setTable("budget");
  ModelBudget.setFilter(sFilter);
  ModelBudget.select();

  //Set structure for model
  int iNumRows=g_rowCount(ModelBudget);
  m_pModel->clear();
  m_pModel->setRowCount(iNumRows);
  m_pModel->setColumnCount(5);

  //Fill out columns
  for (int i=0;i<iNumRows;i++)
  {
    //Get paramenters
    QSqlRecord Record=ModelBudget.record(i);
    QVariant IdBudget=Record.value(0);
    double dUsed=totalUsed(Record);
    double dBudget=totalBudget(IdBudget);

    //Evaluate elapsed days
    int iDate1=Record.value(2).toInt();
    int iDate2=Record.value(3).toInt();
    int iElapsed=QDate::currentDate().toJulianDay()-iDate1;
    if (iElapsed < 0) iElapsed=0;
    if (iElapsed > (iDate2-iDate1)) iElapsed=iDate2-iDate1;

    //Evaluate progress percentages and get color for progress
    int iPercentUsed=100*dUsed/dBudget;
    int iPercentElapsed=100*iElapsed/(iDate2-iDate1);
    QColor Color=colorBudget(iPercentUsed,iPercentElapsed);

    //Column 0: budget.idx
    QStandardItem *pItem;
    pItem=new QStandardItem(IdBudget.toString());
    m_pModel->setItem(i,0,pItem);

    //Column 1: budget.name
    pItem=new QStandardItem(Record.value(1).toString());
    m_pModel->setItem(i,1,pItem);

    //Column 2: progress
    QString s=QString("%1,%2,%3,%4,%5").arg(iPercentElapsed).arg(Color.red()).arg(Color.green()).arg(Color.blue()).arg(Color.alpha());
    pItem=new QStandardItem(s);
    m_pModel->setItem(i,2,pItem);

    //Column 3: used
    pItem=new QStandardItem(QString::number(dUsed));
    m_pModel->setItem(i,3,pItem);

    //Column 4: budget
    pItem=new QStandardItem(QString::number(dBudget));
    m_pModel->setItem(i,4,pItem);
  }
  m_pModel->sort(1);

  //Set Title name for column 1
  QStandardItem *pItem=new QStandardItem(tr("Name"));
  QFont Font=ui->pTableView->font();
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(1,pItem);

  //Set Title name for column 2
  pItem=new QStandardItem(tr("Elapsed time"));
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(2,pItem);

  //Set Title name for column 3
  pItem=new QStandardItem(tr("Used"));
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(3,pItem);

  //Set Title name for column 4
  pItem=new QStandardItem(tr("Budget"));
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(4,pItem);

  //Hide column 0
  ui->pTableView->hideColumn(0);

  //Set header for view
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionResizeMode(1,QHeaderView::ResizeToContents);
  pHeader->setSectionResizeMode(2,QHeaderView::Stretch);
  pHeader->setSectionResizeMode(3,QHeaderView::ResizeToContents);
  pHeader->setSectionResizeMode(4,QHeaderView::ResizeToContents);
}

double CBudgetView::totalBudget(const QVariant& IdBudget)
{
  double dTotal=0.0;

  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("SELECT SUM(amount) FROM budgetcat WHERE budgetid="+IdBudget.toString());
  if(Query.next()) dTotal=Query.value(0).toDouble();
  return dTotal;
}

double CBudgetView::totalUsed(const QSqlRecord& Record)
{
  //Set Categories parameters for filtering query
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("SELECT categoryid FROM budgetcat WHERE budgetid="+Record.value(0).toString());

  QString sFilter;
  while(Query.next())
  {
    if (Query.at()==0)
      sFilter="(trans.categoryid="+Query.value(0).toString();
    else
      sFilter+=" or trans.categoryid="+Query.value(0).toString();
  }
  if (sFilter.contains("(")) sFilter+=")";

  //If categories are not set in budget return 0
  if (sFilter.isEmpty()) return 0.0;

  //Set Class parameters for filtering query
  if (Record.value(6).toInt()!=ID_ANY) sFilter+=" and trans.classid="+Record.value(6).toString();

  //Set Date parameters for filtering query
  QString sDate1=Record.value(2).toString();
  QString sDate2=Record.value(3).toString();

  //Set Currency parameters for evaluation query
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+Record.value(5).toString());
  ModelCurr.select();
  double dEndingRate=ModelCurr.record(0).value(2).toDouble();

  //Query with all the parameters set previously
  Query.exec("SELECT SUM(1.0*"+QString::number(dEndingRate)+"*trans.amount/currency.rate) FROM trans,currency,account WHERE "+
             "trans.accountid=account.idx and account.currencyid=currency.idx"+
             " and trans.date>="+sDate1+" and trans.date<="+sDate2+" and "+sFilter);

  double dTotal=0.0;
  if(Query.next()) dTotal=Query.value(0).toDouble();
  return dTotal;
}

QColor CBudgetView::colorBudget(int iPercentUsed, int iPercentElapsed)
{
  //YELLOW range
  QColor Color;
  if (qAbs(iPercentUsed-iPercentElapsed)<15)
  {
    Color.setRgb(255,255,0,255);
    return Color;
  }

  //ORANGE range
  if ((iPercentUsed>iPercentElapsed) && (iPercentUsed-iPercentElapsed)<30)
  {
    Color.setRgb(255,149,14,255);
    return Color;
  }

  //GREEN/YELLOW range
  if ((iPercentElapsed>iPercentUsed) && (iPercentElapsed-iPercentUsed)<30)
  {
    Color.setRgb(170,250,0,255);
    return Color;
  }

  //RED range
  if (iPercentUsed>iPercentElapsed)
  {
    Color.setRgb(255,0,0,255);
    return Color;
  }

  //GREEN range
  Color.setRgb(0,255,0,255);
  return Color;
}
