#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "budgetrep.h"
#include "ui_budgetrep.h"
#include "basedelegate.h"
#include "global.h"
#include "dr_profile.h"
#include "mainwindow0.h"
#include "transview.h"

CBudgetRep::CBudgetRep(const QVariant& IdBudget, QWidget *pWidget) :
  CFormBase(pWidget),
  ui(new Ui::CBudgetRep)
{
  ui->setupUi(this);
  g_updateFontSize(ui->pTableView);

  //Set font size for bottom labels
#ifdef MOBILE_PLATFORM
  QFont Font=qApp->font();
  int iSize=QFontInfo(Font).pixelSize();
  Font.setPixelSize((double)iSize-(double)iSize/10.0);
  ui->pLRange->setFont(Font);
  ui->pLClass->setFont(Font);
#endif

  //Setup children
  QSqlTableModel ModelBudget;
  ModelBudget.setTable("budget");
  ModelBudget.setFilter("idx="+IdBudget.toString());
  ModelBudget.select();

  m_IdCurr=ModelBudget.record(0).value(5);
  m_IdClass=ModelBudget.record(0).value(6);
  m_IdBudget=IdBudget;

  //Set Model
  m_pModel=new QStandardItemModel(this);

  //Set Delegate features
  m_pDelegate=new CBaseDelegate(ui->pTableView);
  m_pDelegate->setNumberPrecision(2);
  m_pDelegate->addColumnFormat(2,CBaseDelegate::FormatNumber);
  m_pDelegate->addColumnFormat(3,CBaseDelegate::FormatNumber);
  m_pDelegate->addColumnFormat(4,CBaseDelegate::FormatNumber);

  //Set TableView features
  ui->pTableView->setItemDelegate(m_pDelegate);
  ui->pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::SolidLine);
  ui->pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->pTableView->setModel(m_pModel);
  ui->pTableView->verticalHeader()->hide();

  //Set HeaderView features
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionResizeMode(QHeaderView::Stretch);

  //Set connections
  connect(g_pMainWindow,SIGNAL(newTransaction()),this,SLOT(updateView()));
  connect(g_pMainWindow,SIGNAL(updateBudget()),this,SLOT(updateView()));
  connect(g_pMainWindow,SIGNAL(updateBudget()),this,SLOT(updateLabelInfo()));
}

CBudgetRep::~CBudgetRep()
{
  delete ui;
}

void CBudgetRep::on_pPBClose_clicked()
{
  close();
}

void CBudgetRep::on_pTableView_activated(QModelIndex Index)
{
  //Last row corresponding to the totals
  if (Index.row()==m_pModel->rowCount()-1) return;

  //When user click on the category only select row
  if (Index.column()==1) return;

  //Set Filter
  CTransView::Filters flFilter=CTransView::FilterFromBud | CTransView::FilterCategory;
  if (m_IdClass.toInt()!=ID_ANY) flFilter|=CTransView::FilterClass;

  //Get range of dates
  QSqlTableModel ModelBudget;
  ModelBudget.setTable("budget");
  ModelBudget.setFilter("idx="+m_IdBudget.toString());
  ModelBudget.select();

  QDate Date1=QDate::fromJulianDay(ModelBudget.record(0).value(2).toInt());
  QDate Date2=QDate::fromJulianDay(ModelBudget.record(0).value(3).toInt());

  //Prepare filtering parameters and open TransView
  g_FilterParam.setParams(Date1,Date2,"","","","","",ID_ANY,ID_ANY,ID_ANY,ID_ANY,0);
  g_pMainWindow->addForm("CTransView",m_pModel->item(Index.row(),0)->text(),m_IdClass,StatusAll,flFilter,m_IdCurr);
}

void CBudgetRep::updateView()
{
  //Reset the Model
  m_pModel->clear();

  //Initiate Category model
  QSqlTableModel ModelCat;

  ModelCat.setTable("category");
  ModelCat.setSort(1,Qt::AscendingOrder);
  ModelCat.setFilter("idx!="+QString::number(ID_SPLIT));
  ModelCat.select();

  //Set number of rows and columns
  int iNumRows=g_rowCount(ModelCat)+1;
  m_pModel->setRowCount(iNumRows);
  m_pModel->setColumnCount(5);

  //Initialize items for columns 2->4
  for (int i=2;i<5;i++)
    initColumn(i);

  //Load model - column 0, 1
  for (int i=0;i<iNumRows-1;i++)
  {
    //Column 0
    QStandardItem *pItem;
    pItem=new QStandardItem(ModelCat.record(i).value(0).toString());
    m_pModel->setItem(i,0,pItem);

    //Column 1
    pItem=new QStandardItem(ModelCat.record(i).value(1).toString());
    pItem->setIcon(QIcon(ModelCat.record(i).value(2).toString()));
    m_pModel->setItem(i,1,pItem);
  }

  //Set Title name for column 1
  QStandardItem *pItem=new QStandardItem(tr("Category"));
  QFont Font=ui->pTableView->font();
  Font.setBold(true);
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(1,pItem);

  //Get parameters from currency table
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+m_IdCurr.toString());
  ModelCurr.select();

  QString sCurrSymbol=ModelCurr.record(0).value(3).toString();
  double dEndingRate=ModelCurr.record(0).value(2).toDouble();

  //Load column 2 and 3 and set title
  fillUsedColumn(sCurrSymbol,dEndingRate);
  fillBudgetColumn(sCurrSymbol);

  //Delete rows with budget=0
  int i=0;
  while (i<m_pModel->rowCount()-1)
  {
    if (m_pModel->item(i,3)->text().toDouble()==0)
      m_pModel->removeRow(i);
    else
      i++;
  }

  //Fill Total rows and columns
  fillTotalColumn(sCurrSymbol);
  fillTotalRow(sCurrSymbol);

  //Visualization
  ui->pTableView->hideColumn(0);
}

void CBudgetRep::showEvent( QShowEvent */*pEvent*/ )
{
  //Update bottom label info
  updateLabelInfo();

  //Update model
  g_updateFontSize(ui->pTableView);
  updateView();
}

void CBudgetRep::updateLabelInfo()
{
  //Create model budget
  QSqlTableModel ModelBudget;
  ModelBudget.setTable("budget");
  ModelBudget.setFilter("idx="+m_IdBudget.toString());
  ModelBudget.select();

  QDate Date1=QDate::fromJulianDay(ModelBudget.record(0).value(2).toInt());
  QDate Date2=QDate::fromJulianDay(ModelBudget.record(0).value(3).toInt());

  ui->pLName->setText("<b>"+ModelBudget.record(0).value(1).toString()+"</b>");
  ui->pLRange->setText(Date1.toString(g_dateFormat())+" - "+Date2.toString(g_dateFormat()));

  if (m_IdClass.toInt()==ID_ANY)
  {
    ui->pLClass->setText(tr("( All classes )"));
  }
  else
  {
    //Create model class
    QSqlTableModel ModelClass;
    ModelClass.setTable("class");
    ModelClass.setFilter("idx="+m_IdClass.toString());
    ModelClass.select();
    ui->pLClass->setText("( "+ModelClass.record(0).value(1).toString()+" )");
  }
}

void CBudgetRep::initColumn(int iCol)
{
  for (int i=0;i<m_pModel->rowCount();i++)
  {
    QStandardItem *pItem=new QStandardItem("0");
    pItem->setTextAlignment(Qt::AlignRight);
    m_pModel->setItem(i,iCol,pItem);
  }
}

void CBudgetRep::fillUsedColumn(const QString& sCurrSym, double dEndingRate)
{
  //Set up filter for class
  QString sFilter="";
  if (m_IdClass.toInt()!=ID_ANY) sFilter=" and trans.classid="+m_IdClass.toString();

  //Get range of dates
  QSqlTableModel ModelBudget;
  ModelBudget.setTable("budget");
  ModelBudget.setFilter("idx="+m_IdBudget.toString());
  ModelBudget.select();

  QDate Date1=QDate::fromJulianDay(ModelBudget.record(0).value(2).toInt());
  QDate Date2=QDate::fromJulianDay(ModelBudget.record(0).value(3).toInt());

  //Query with all the parameters set previously
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("SELECT trans.categoryid,SUM(1.0*"+QString::number(dEndingRate)+"*trans.amount/currency.rate) FROM trans,currency,account WHERE "+
             "trans.accountid=account.idx and account.currencyid=currency.idx"+sFilter+
             " and trans.date>="+QString::number(Date1.toJulianDay())+" and trans.date<="+QString::number(Date2.toJulianDay())+
             " GROUP BY trans.categoryid");

  while (Query.next())
  {
    if (!m_pModel->findItems(Query.value(0).toString()).isEmpty())
    {
      int iRow=m_pModel->findItems(Query.value(0).toString()).at(0)->row();
      m_pModel->item(iRow,2)->setText(QString::number(Query.value(1).toDouble(),'f',2));
    }
  }

  //Set Header
  QStandardItem *pItem=new QStandardItem(tr("Used")+" ["+sCurrSym+"]");
  QFont Font=ui->pTableView->font();
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(2,pItem);
}

void CBudgetRep::fillBudgetColumn(const QString& sCurrSym)
{
  //Query with all the parameters set previously
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("SELECT categoryid,amount FROM budgetcat WHERE budgetid="+m_IdBudget.toString());

  while (Query.next())
  {
    if (!m_pModel->findItems(Query.value(0).toString()).isEmpty())
    {
      int iRow=m_pModel->findItems(Query.value(0).toString()).at(0)->row();
      m_pModel->item(iRow,3)->setText(QString::number(Query.value(1).toDouble(),'f',2));
    }
  }

  //Set Header
  QStandardItem *pItem=new QStandardItem(tr("Budget")+" ["+sCurrSym+"]");
  QFont Font=ui->pTableView->font();
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(3,pItem);
}

void CBudgetRep::fillTotalColumn(const QString& sCurrSym)
{
  //Create Item for Title
  QStandardItem *pItem=new QStandardItem(tr("Balance")+" ["+sCurrSym+"]");
  QFont Font=ui->pTableView->font();
  Font.setBold(true);
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(4,pItem);

  //Set Items for Totalizings
  for (int i=0;i<m_pModel->rowCount()-1;i++)
  {
    double dTotal=0;
    if (m_pModel->item(i,3)->text().toDouble()>0)
      dTotal=m_pModel->item(i,3)->text().toDouble()-m_pModel->item(i,2)->text().toDouble();
    else
      dTotal=m_pModel->item(i,2)->text().toDouble()-m_pModel->item(i,3)->text().toDouble();
    m_pModel->item(i,4)->setText(QString::number(dTotal,'f',2));
    m_pModel->item(i,4)->setFont(Font);
  }
}

void CBudgetRep::fillTotalRow(const QString& sCurrSym)
{
  //Create Item for Title
  int iNumRows=m_pModel->rowCount();
  QStandardItem *pItem=new QStandardItem(tr("Total")+" ["+sCurrSym+"]");
  QFont Font=ui->pTableView->font();
  Font.setBold(true);
  pItem->setFont(Font);
  m_pModel->setItem(iNumRows-1,1,pItem);

  //Set Items for Totalizings
  for (int j=2;j<5;j++)
  {
    double dTotal=0;
    for (int i=0;i<iNumRows-1;i++)
      dTotal+=m_pModel->item(i,j)->text().toDouble();
    m_pModel->item(iNumRows-1,j)->setText(QString::number(dTotal,'f',2));
    m_pModel->item(iNumRows-1,j)->setFont(Font);
  }
}

