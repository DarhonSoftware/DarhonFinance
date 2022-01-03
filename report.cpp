#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "report.h"
#include "ui_report.h"
#include "basedelegate.h"
#include "global.h"
#include "calendar.h"
#include "transview.h"
#include "reportdrawing.h"
#include "mainwindow0.h"
#include "dr_profile.h"

CReport::CReport(Status iStatus, QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CReport)
{
  ui->setupUi(this);

  //Setup children
  if (g_dateFormat().left(2)=="dd")
    m_sDateFormat="dd.MM.yyyy";
  else
    m_sDateFormat="MM.dd.yyyy";
  m_iDraw=DrawNone;
  ui->pWTable->setVisible(false);
  ui->pScrollArea->setVisible(false);
  g_updateFontSize(ui->pTableView);

  //Set the icon sizes according to the resolution
  if (g_resolution()==ResSmall)
  {
    ui->pTBDrawHistogram->setIconSize(QSize(32,32));
    ui->pTBDrawPie->setIconSize(QSize(32,32));
    ui->pTBReport->setIconSize(QSize(32,32));
  }

  //Set Model
  m_pModel=new QStandardItemModel(this);

  //Set Delegate features
  m_pDelegate=new CBaseDelegate(ui->pTableView);
  m_pDelegate->setNumberPrecision(2);

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
  pHeader->setSectionResizeMode(QHeaderView::Interactive);

  //Initiate Amounts ComboBox
  g_initComboboxAmounts(ui->pCBAmtType,0);

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Initiate Currency ComboBox
  m_pModelCurrency=new QSqlTableModel(this);
  m_pModelCurrency->setTable("currency");
  ui->pCBCurrency->setModel(m_pModelCurrency);
  ui->pCBCurrency->setModelColumn(1);

  //Update controls to their last state from database
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  //Update Comboboxes
  ui->pCBType->setCurrentIndex(Table.record(0).value(12).toInt());
  g_refreshCombobox(ui->pCBClass,Table.record(0).value(13),true);
  g_refreshCombobox(ui->pCBCurrency,Table.record(0).value(23),false);
  ui->pCBColumns->setCurrentIndex(Table.record(0).value(14).toInt());
  ui->pCBAmtType->setCurrentIndex(Table.record(0).value(15).toInt());
  g_initComboboxStatus(ui->pCBStatus,iStatus);

  //Update Period combobox & Date buttons
  int iPeriod=Table.record(0).value(16).toInt();
  ui->pCBPeriod->blockSignals(true);
  ui->pCBPeriod->setCurrentIndex(iPeriod);
  ui->pCBPeriod->blockSignals(false);
  if (iPeriod==6)
  {
    QDate Date=QDate::fromJulianDay(Table.record(0).value(17).toInt());
    ui->pPBBegin->setText(Date.toString(m_sDateFormat));
    Date=QDate::fromJulianDay(Table.record(0).value(18).toInt());
    ui->pPBEnd->setText(Date.toString(m_sDateFormat));
  }
  else
    writeDates(iPeriod);

  //Set connections
  connect(ui->pViewport,SIGNAL(hideDrawing()),this,SLOT(viewControls()));
  connect(g_pMainWindow,SIGNAL(newTransaction()),this,SLOT(updateView()));
}

CReport::~CReport()
{
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  QSqlRecord Record=Table.record(0);
  Record.setValue(12,ui->pCBType->currentIndex());
  Record.setValue(13,m_pModelClass->record(ui->pCBClass->currentIndex()).value(0));
  Record.setValue(14,ui->pCBColumns->currentIndex());
  Record.setValue(15,ui->pCBAmtType->currentIndex());
  Record.setValue(16,ui->pCBPeriod->currentIndex());
  Record.setValue(17,QDate::fromString(ui->pPBBegin->text(),m_sDateFormat).toJulianDay());
  Record.setValue(18,QDate::fromString(ui->pPBEnd->text(),m_sDateFormat).toJulianDay());
  Record.setValue(23,m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(0));

  Table.setRecord(0,Record);
  Table.submitAll();

  delete ui;
}

void CReport::showEvent( QShowEvent */*pEvent*/ )
{
  g_refreshCombobox(ui->pCBClass,QVariant(),true);
  g_refreshCombobox(ui->pCBCurrency,QVariant(),false);
  g_updateFontSize(ui->pTableView);
  updateView();
}

void CReport::on_pCBPeriod_currentIndexChanged(int iIndex)
{
  if (iIndex<6) writeDates(iIndex);
}

void CReport::on_pPBBegin_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBBegin->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBBegin->setText(Date.toString(m_sDateFormat));
    ui->pCBPeriod->setCurrentIndex(6);
  }
}

void CReport::on_pPBEnd_clicked()
{
  CCalendar Calendar(this);
  QDate Date=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);
  Calendar.setDate(Date);
  if (Calendar.exec()==QDialog::Accepted)
  {
    Date=Calendar.date();
    ui->pPBEnd->setText(Date.toString(m_sDateFormat));
    ui->pCBPeriod->setCurrentIndex(6);
  }
}

void CReport::on_pTableView_activated(QModelIndex Index)
{
  if (Index.row()==m_pModel->rowCount()-1) return;

  //When user click on the category/payee/account only select row
  if (Index.column()==1) return;

  //Set Filter for category/payee/account based on CBType
  CTransView::Filters flFilter=CTransView::FilterFromRep;
  if (ui->pCBType->currentIndex()==0)
    flFilter|=CTransView::FilterCategory;
  if (ui->pCBType->currentIndex()==1)
    flFilter|=CTransView::FilterPayee;
  if (ui->pCBType->currentIndex()==2)
    flFilter|=CTransView::FilterAccount;

  //Set Filter for class based on CBClass
  int iClass=0;
  if (ui->pCBClass->currentIndex()!=0)
  {
    flFilter|=CTransView::FilterClass;
    iClass=m_pModelClass->record(ui->pCBClass->currentIndex()).value(0).toInt();
  }

  //Get date range
  QDate Date1=QDate::fromString(ui->pPBBegin->text(),m_sDateFormat);
  QDate Date2=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);

  //Get status
  Status iStatus=static_cast<Status>(ui->pCBStatus->currentIndex());

  //Prepare filtering parameters and open TransView
  g_FilterParam.setParams(Date1,Date2,"","","","","",ID_ANY,ID_ANY,ID_ANY,ID_ANY,ui->pCBAmtType->currentIndex());
  g_pMainWindow->addForm("CTransView",m_pModel->item(Index.row(),0)->text(),iClass,iStatus,flFilter,
                         m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(0));
}

void CReport::on_pTBReport_clicked()
{
  //Get Dates parameters and validate them
  QDate Date1=QDate::fromString(ui->pPBBegin->text(),m_sDateFormat);
  QDate Date2=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);
  if (Date1.daysTo(Date2)<0)
  {
    g_messageWarning(this,tr("Report"),tr("Range of dates are not correct"));
    return;
  }

  ui->pWTable->show();
  ui->pScrollArea->hide();
  ui->pWidgetControls->hide();
  m_iDraw=DrawNone;
  QCoreApplication::processEvents();
  updateView();
}

void CReport::on_pTBDrawHistogram_clicked()
{
  //Get Dates parameters and validate them
  QDate Date1=QDate::fromString(ui->pPBBegin->text(),m_sDateFormat);
  QDate Date2=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);
  if (Date1.daysTo(Date2)<0)
  {
    g_messageWarning(this,tr("Histogram"),tr("Range of dates are not correct"));
    return;
  }

  ui->pWTable->hide();
  ui->pScrollArea->show();
  ui->pWidgetControls->hide();
  m_iDraw=DrawHistogram;
  QCoreApplication::processEvents();
  updateView();
}

void CReport::on_pTBDrawPie_clicked()
{
  //Get Dates parameters and validate them
  QDate Date1=QDate::fromString(ui->pPBBegin->text(),m_sDateFormat);
  QDate Date2=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);
  if (Date1.daysTo(Date2)<0)
  {
    g_messageWarning(this,tr("Pie"),tr("Range of dates are not correct"));
    return;
  }

  ui->pWTable->hide();
  ui->pScrollArea->show();
  ui->pWidgetControls->hide();
  m_iDraw=DrawPie;
  QCoreApplication::processEvents();
  updateView();
}

void CReport::on_pPBReturn_clicked()
{
  viewControls();
}

void CReport::viewControls()
{
  ui->pWTable->hide();
  ui->pScrollArea->hide();
  ui->pWidgetControls->show();
  m_iDraw=DrawNone;
}

void CReport::on_pPBClose_clicked()
{
  close();
}

void CReport::updateView()
{
  //Reset the Model & list of visible records
  m_pModel->clear();
  m_lstVisibleIDs.clear();

  //Get Dates parameters
  QDate Date1=QDate::fromString(ui->pPBBegin->text(),m_sDateFormat);
  QDate Date2=QDate::fromString(ui->pPBEnd->text(),m_sDateFormat);

  //Load the model for category/payee/account based on CBType
  QString sType;
  int iColSort=0;
  if (ui->pCBType->currentIndex()==0)
  {
    sType="category";
    iColSort=1;
  }
  if (ui->pCBType->currentIndex()==1)
  {
    sType="payee";
    iColSort=1;
  }
  if (ui->pCBType->currentIndex()==2)
  {
    sType="account";
    iColSort=3;
  }

  QSqlTableModel Model;
  Model.setTable(sType);
  Model.sort(iColSort,Qt::AscendingOrder);
  Model.select();
  if (sType=="category") Model.setFilter("idx!="+QString::number(ID_SPLIT));

  //Set number of rows for Model
  int iNumRows=g_rowCount(Model)+1;
  m_pModel->setRowCount(iNumRows);

  //Determine the number of columns according to CBColumns and Dates
  int iNumColumns=0;
  if (ui->pCBColumns->currentIndex()==0)
    iNumColumns=1;
  if (ui->pCBColumns->currentIndex()==1)
    iNumColumns=(Date2.month()-Date1.month()+1)+(Date2.year()-Date1.year())*12+1;
  if (ui->pCBColumns->currentIndex()==2)
    iNumColumns=(Date2.year()-Date1.year()+1)+1;
  m_pModel->setColumnCount(iNumColumns+2);
  for (int i=0;i<iNumColumns;i++)
    initColumn(i+2,iNumRows);

  //Load model: column 0 and 1
  for (int i=0;i<iNumRows-1;i++)
  {
    QStandardItem *pItem;
    pItem=new QStandardItem(Model.record(i).value(0).toString());
    m_pModel->setItem(i,0,pItem);

    if (sType=="category")
    {
      pItem=new QStandardItem(Model.record(i).value(1).toString());
      pItem->setIcon(QIcon(Model.record(i).value(2).toString()));
    }
    if (sType=="payee")
    {
      pItem=new QStandardItem(Model.record(i).value(1).toString());
    }
    if (sType=="account")
    {
      pItem=new QStandardItem(Model.record(i).value(3).toString());
      pItem->setIcon(QIcon(Model.record(i).value(1).toString()));
    }
    m_pModel->setItem(i,1,pItem);
  }

  //Set Title name for column 1
  if (sType=="category")
  {
    QStandardItem *pItem=new QStandardItem(tr("Category"));
    QFont Font=ui->pTableView->font();
    Font.setBold(true);
    pItem->setFont(Font);
    m_pModel->setHorizontalHeaderItem(1,pItem);
  }
  if (sType=="payee")
  {
    QStandardItem *pItem=new QStandardItem(tr("Payee"));
    QFont Font=ui->pTableView->font();
    Font.setBold(true);
    pItem->setFont(Font);
    m_pModel->setHorizontalHeaderItem(1,pItem);
  }
  if (sType=="account")
  {
    QStandardItem *pItem=new QStandardItem(tr("Account"));
    QFont Font=ui->pTableView->font();
    Font.setBold(true);
    pItem->setFont(Font);
    m_pModel->setHorizontalHeaderItem(1,pItem);
  }

  //Prepare sFilter variable from Class/InEx/Status status
  int iClass=m_pModelClass->record(ui->pCBClass->currentIndex()).value(0).toInt();
  int iAmtType=ui->pCBAmtType->currentIndex();

  QString sFilter="";
  if (iClass!=ID_ANY) sFilter="trans.classid=="+QString::number(iClass);
  if (iAmtType==1) //Income
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.amount>=0";
  }
  if (iAmtType==2) //Expense
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.amount<0";
  }
  g_filterStatus(ui->pCBStatus,sFilter);

  //Use selected currency to get ending rate
  double dEndingRate=m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(2).toDouble();

  //Load model for Columns=Total
  if (ui->pCBColumns->currentIndex()==0)
  {
    fillColumn(sType+"id",2,Date1.toJulianDay(),Date2.toJulianDay(),sFilter,tr("Global"),dEndingRate);
    fillTotalRow(iNumRows-1,iNumColumns);
  }

  //Load model for Columns=Month
  if (ui->pCBColumns->currentIndex()==1)
  {
    QDate DateB=Date1;
    QDate DateE;
    DateE.setDate(Date1.year(),Date1.month(),Date1.daysInMonth());
    if (DateE>Date2) DateE=Date2;

    for (int i=0;i<iNumColumns-1;i++)
    {
      fillColumn(sType+"id",i+2,DateB.toJulianDay(),DateE.toJulianDay(),sFilter,DateB.toString("MM.yyyy"),dEndingRate);
      DateB=DateE.addDays(1);
      DateE.setDate(DateB.year(),DateB.month(),DateB.daysInMonth());
      if (DateE>Date2) DateE=Date2;
    }
    fillTotalColumn(iNumColumns+1,iNumRows-1);
    fillTotalRow(iNumRows-1,iNumColumns);
  }

  //Load model for Columns=Year
  if (ui->pCBColumns->currentIndex()==2)
  {
    QDate DateB=Date1;
    QDate DateE;
    DateE.setDate(Date1.year(),12,31);
    if (DateE>Date2) DateE=Date2;

    for (int i=0;i<iNumColumns-1;i++)
    {
      fillColumn(sType+"id",i+2,DateB.toJulianDay(),DateE.toJulianDay(),sFilter,DateB.toString("yyyy"),dEndingRate);
      DateB=DateE.addDays(1);
      DateE.setDate(DateB.year(),12,31);
      if (DateE>Date2) DateE=Date2;
    }
    fillTotalColumn(iNumColumns+1,iNumRows-1);
    fillTotalRow(iNumRows-1,iNumColumns);
  }

  //Delete rows without data
  int i=0;
  while (i<m_pModel->rowCount()-1)
  {
    if (m_lstVisibleIDs.contains(m_pModel->item(i,0)->text()))
      i++;
    else
      m_pModel->removeRow(i);
  }

  //Set the TableView Delegate
  ui->pTableView->hideColumn(0);
  m_pDelegate->clear();
  for (int i=2;i<iNumColumns+2;i++)
    m_pDelegate->addColumnFormat(i,CBaseDelegate::FormatNumber);

  //Resize the table to fit the content
  ui->pTableView->resizeColumnsToContents();

  //Update drawing representation
  if (m_iDraw==DrawPie)
    ui->pViewport->updateModel(m_pModel,CReportDrawing::DrawPie);
  if (m_iDraw==DrawHistogram)
    ui->pViewport->updateModel(m_pModel,CReportDrawing::DrawHistogram);
}

void CReport::writeDates(int iPeriod)
{
  QDate Date1,Date2;
  if (iPeriod==0) //last 30 days
  {
    Date2=QDate::currentDate();
    Date1=Date2.addMonths(-1);
  }
  if (iPeriod==1) //last 90 days
  {
    Date2=QDate::currentDate();
    Date1=Date2.addMonths(-3);
  }
  if (iPeriod==2) //last month
  {
    int iYear=QDate::currentDate().year();
    int iMonth=QDate::currentDate().month()-1;
    if (iMonth==0)
    {
      iMonth=12;
      iYear-=1;
    }
    Date1.setDate(iYear,iMonth,1);
    Date2=Date1.addDays(Date1.daysInMonth()-1);
  }
  if (iPeriod==3) //last year
  {
    Date1.setDate(QDate::currentDate().year()-1,1,1);
    Date2=Date1.addDays(Date1.daysInYear()-1);
  }
  if (iPeriod==4) //This Month
  {
    Date1.setDate(QDate::currentDate().year(),QDate::currentDate().month(),1);
    Date2=Date1.addDays(Date1.daysInMonth()-1);
  }
  if (iPeriod==5) //This year
  {
    Date1.setDate(QDate::currentDate().year(),1,1);
    Date2=Date1.addDays(Date1.daysInYear()-1);
  }
  ui->pPBBegin->setText(Date1.toString(m_sDateFormat));
  ui->pPBEnd->setText(Date2.toString(m_sDateFormat));
}

void CReport::initColumn(int iCol, int iNumRows)
{
  for (int i=0;i<iNumRows;i++)
  {
    QStandardItem *pItem=new QStandardItem("0");
    pItem->setTextAlignment(Qt::AlignRight);
    m_pModel->setItem(i,iCol,pItem);
  }
}

void CReport::fillColumn(const QString& sField,int iCol, int iDateB, int iDateE,const QString& sFilter,const QString& sTitle, double dEndingRate)
{
  QSqlQuery Query;
  Query.setForwardOnly(true);

  //Set filter to show 'main split' or 'children split' depending on the sField
  QString sFinalFilter=sFilter;
  if (sField=="accountid")
  {
    if (!sFinalFilter.isEmpty()) sFinalFilter+=" and ";
    sFinalFilter+="trans.split<"+QString::number(SPLIT);
  }
  if (sField=="payeeid")
  {
    if (!sFinalFilter.isEmpty()) sFinalFilter+=" and ";
    sFinalFilter+="trans.categoryid!="+QString::number(ID_SPLIT);
  }
  if (!sFinalFilter.isEmpty()) sFinalFilter.append(" and ");

  //Query with all the parameters set previously
  Query.exec("SELECT trans."+sField+",SUM(1.0*"+QString::number(dEndingRate)+"*trans.amount/currency.rate) FROM trans,currency,account WHERE "+sFinalFilter+
             "trans.accountid=account.idx and account.currencyid=currency.idx"+
             " and trans.date>="+QString::number(iDateB) +" and trans.date<="+ QString::number(iDateE)+
             " GROUP BY trans."+sField);

  //Go through the query and place results on the model
  while (Query.next())
  {
    if (!m_pModel->findItems(Query.value(0).toString()).isEmpty())
    {
      int iRow=m_pModel->findItems(Query.value(0).toString()).at(0)->row();
      m_pModel->item(iRow,iCol)->setText(QString::number(Query.value(1).toDouble(),'f',2));
      //Mark record as visible in the report
      m_lstVisibleIDs.append(m_pModel->item(iRow,0)->text());
    }
  }

  //Set Header
  QStandardItem *pItem=new QStandardItem(sTitle);
  QFont Font=ui->pTableView->font();
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(iCol,pItem);
}

void CReport::fillTotalColumn(int iCol, int iNumRows)
{
  //Get selected currency symbol
  QString sCurrSym=m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(3).toString();

  //Create Item for Title
  QStandardItem *pItem=new QStandardItem(tr("Total")+" ["+sCurrSym+"]");
  QFont Font=ui->pTableView->font();
  Font.setBold(true);
  pItem->setFont(Font);
  m_pModel->setHorizontalHeaderItem(iCol,pItem);

  //Create Items for Totalizings
  for (int i=0;i<iNumRows;i++)
  {
    double dTotal=0;
    for (int j=2;j<iCol;j++)
      dTotal+=m_pModel->item(i,j)->text().toDouble();
    m_pModel->item(i,iCol)->setText(QString::number(dTotal,'f',2));
    m_pModel->item(i,iCol)->setFont(Font);
  }
}

void CReport::fillTotalRow(int iRow, int iNumColumns)
{
  //Get selected currency symbol
  QString sCurrSym=m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(3).toString();

  //Create Item for Title
  QStandardItem *pItem=new QStandardItem(tr("Total")+" ["+sCurrSym+"]");
  QFont Font=ui->pTableView->font();
  Font.setBold(true);
  pItem->setFont(Font);
  m_pModel->setItem(iRow,1,pItem);

  //Create Items for Totalizings
  for (int j=2;j<iNumColumns+2;j++)
  {
    double dTotal=0;
    for (int i=0;i<iRow;i++)
      dTotal+=m_pModel->item(i,j)->text().toDouble();
    m_pModel->item(iRow,j)->setText(QString::number(dTotal,'f',2));
    m_pModel->item(iRow,j)->setFont(Font);
  }
}

