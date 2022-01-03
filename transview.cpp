#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "transview.h"
#include "ui_transview.h"
#include "mainwindow0.h"
#include "global.h"
#include "delegatetransview.h"
#include "dr_profile.h"
#include "filterparam.h"

//Global variable to set filters
CFilterParam g_FilterParam;

CTransView::CTransView(Status iStatus, Filters flFilter, const QVariant& Id1, const QVariant& Id2, const QVariant& IdCurr, QWidget *pWidget):
    CFormBase(pWidget),
    ui(new Ui::CTransView)
{
  ui->setupUi(this);
  m_flFilter=flFilter;
  m_Id1=Id1; //Id for Account/Payee/Category depending on the flFilter
  m_Id2=Id2; //Id for Class
  m_IdCurr=IdCurr;
  m_iScrollValue=0;
  g_updateFontSize(ui->pTableView);
#ifdef MOBILE_PLATFORM
  ui->pWidgetFilters->setVisible(false);
  ui->pPBFilters->setVisible(true);
#else
  ui->pWidgetFilters->setVisible(true);
  ui->pPBFilters->setVisible(false);
#endif

  //Set bottom line for the view for each case
  if (m_flFilter & FilterFromAcc)
  {
    ui->pWidgetBottom->hide();
  }
  if (m_flFilter & FilterFromRep)
  {
    ui->pLRangeCheques->hide();
  }
  if (m_flFilter & FilterFromBud)
  {
    ui->pLRangeAmounts->hide();
    ui->pLRangeCheques->hide();
  }
  if (m_flFilter & FilterFromSch)
  {
    ui->pLCurrency->hide();
    ui->pLTotal->hide();
  }

  //Set font size for bottom labels
#ifdef MOBILE_PLATFORM
  QFont Font=qApp->font();
  int iSize=QFontInfo(Font).pixelSize();
  Font.setPixelSize((double)iSize-(double)iSize/10.0);
  ui->pLRangeDates->setFont(Font);
  ui->pLRangeAmounts->setFont(Font);
  ui->pLRangeCheques->setFont(Font);
#endif

  //Set model transations for view
  m_pModel=new QSqlRelationalTableModel(this);
  m_pModel->setTable("trans");
  m_pModel->setRelation(1,QSqlRelation("category","idx","icon"));
  m_pModel->setRelation(4,QSqlRelation("class","idx","name"));
  m_pModel->setRelation(5,QSqlRelation("payee","idx","name"));
  m_pModel->setRelation(6,QSqlRelation("account","idx","name"));
  m_pModel->setSort(3,Qt::AscendingOrder);
  m_pModel->setHeaderData(1,Qt::Horizontal,tr("I"));
  m_pModel->setHeaderData(2,Qt::Horizontal,tr("S"));
  m_pModel->setHeaderData(3,Qt::Horizontal,tr("Date"));
  m_pModel->setHeaderData(4,Qt::Horizontal,tr("Class"));
  m_pModel->setHeaderData(5,Qt::Horizontal,tr("Payee"));
  m_pModel->setHeaderData(6,Qt::Horizontal,tr("Account"));
  m_pModel->setHeaderData(7,Qt::Horizontal,tr("Category"));
  m_pModel->setHeaderData(8,Qt::Horizontal,tr("Cheque"));
  m_pModel->setHeaderData(9,Qt::Horizontal,tr("Amount"));
  m_pModel->setHeaderData(11,Qt::Horizontal,tr("Balance"));
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_pModel->select();

  //Set Delegate features
  CDelegateTransView* pDelegate=new CDelegateTransView(this);
  pDelegate->setNumberPrecision(2);
  pDelegate->updateCategoryData();
  pDelegate->setDateFormat(g_dateFormat());
  pDelegate->addColumnFormat(1,CBaseDelegate::FormatIcon);
  pDelegate->addColumnFormat(2,CBaseDelegate::FormatCheck);
  pDelegate->addColumnFormat(3,CBaseDelegate::FormatDate);
  pDelegate->addColumnFormat(7,CBaseDelegate::FormatCategory);
  pDelegate->addColumnFormat(9,CBaseDelegate::FormatNumber);
  pDelegate->addColumnFormat(11,CBaseDelegate::FormatNumber);
  connect(pDelegate,SIGNAL(closeEditor(QWidget*)),this,SLOT(updateView()));

  //Set TableView features
  ui->pTableView->setSortingEnabled(true);
  ui->pTableView->setItemDelegate(pDelegate);
  ui->pTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::NoPen);
  ui->pTableView->setEditTriggers(QAbstractItemView::SelectedClicked);
  ui->pTableView->setModel(m_pModel);
  ui->pTableView->setColumnHidden(0,true);
  ui->pTableView->setColumnHidden(10,true);
  ui->pTableView->setColumnHidden(12,true);
  ui->pTableView->setColumnHidden(13,true);
  ui->pTableView->setColumnHidden(14,true);
  if (m_flFilter & FilterFromSch) ui->pTableView->setColumnHidden(11,true);
  ui->pTableView->resizeColumnsToContents();

  //Set HeaderView features
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionsClickable(true);
  pHeader->setSectionsMovable(true);
  pHeader->setSortIndicatorShown(true);
  pHeader->setSortIndicator(3,Qt::DescendingOrder);
  pHeader->setSectionResizeMode(QHeaderView::Interactive);
  pHeader->setSectionResizeMode(1,QHeaderView::Fixed);
  pHeader->setSectionResizeMode(2,QHeaderView::Fixed);
  pHeader->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(pHeader,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(menuColumnsChooser(QPoint)));
  connect(pHeader,SIGNAL(sectionClicked(int)),this,SLOT(updateColumnBalance()));
  ui->pTableView->verticalHeader()->hide();

  //Initiate Account ComboBox
  m_pModelAccount=new QSqlTableModel(this);
  m_pModelAccount->setTable("account");
  m_pModelAccount->setSort(3,Qt::AscendingOrder);
  ui->pCBAccount->setModel(m_pModelAccount);
  ui->pCBAccount->setModelColumn(3);
  if (m_flFilter & FilterAccount)
  {
    g_refreshCombobox(ui->pCBAccount,m_Id1,true);
    ui->pCBAccount->setEnabled(false);
    ui->pTableView->setColumnHidden(6,true);
  }

  //Initiate Payee ComboBox
  m_pModelPayee=new QSqlTableModel(this);
  m_pModelPayee->setTable("payee");
  m_pModelPayee->setSort(1,Qt::AscendingOrder);
  ui->pCBPayee->setModel(m_pModelPayee);
  ui->pCBPayee->setModelColumn(1);
  if (m_flFilter & FilterPayee)
  {
    g_refreshCombobox(ui->pCBPayee,m_Id1,true);
    ui->pCBPayee->setEnabled(false);
    ui->pTableView->setColumnHidden(5,true);
  }

  //Initiate Category ComboBox
  if (m_flFilter & FilterCategory)
  {
    g_refreshComboCategory(ui->pCBCategory,m_Id1,QVariant(),true);
    ui->pCBCategory->setEnabled(false);
    ui->pTableView->setColumnHidden(1,true);
    ui->pTableView->setColumnHidden(7,true);
  }

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);
  if (m_flFilter & FilterClass)
  {
    g_refreshCombobox(ui->pCBClass,m_Id2,true);
    ui->pCBClass->setEnabled(false);
    ui->pTableView->setColumnHidden(4,true);
  }

  //Initiate Status ComboBox
  g_initComboboxStatus(ui->pCBStatus,iStatus);
  if ((iStatus!=StatusAll)&&(m_flFilter & FilterFromRep)) ui->pCBStatus->setEnabled(false);

  //Update HEADER, FILTER combobox and CLEAR button from database
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  //Load HorizontalHeader state
  QByteArray ByteArray;
  if (m_flFilter & FilterAccount) ByteArray=Table.record(0).value(8).toByteArray();
  if (m_flFilter & FilterPayee) ByteArray=Table.record(0).value(9).toByteArray();
  if (m_flFilter & FilterCategory) ByteArray=Table.record(0).value(10).toByteArray();
  ui->pTableView->horizontalHeader()->restoreState(ByteArray);

  //Set connections
  connect(g_pMainWindow,SIGNAL(newTransaction()),this,SLOT(updateView()));
  connect(ui->pCBAccount,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBPayee,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBClass,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBCategory,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBStatus,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
  connect(g_pMainWindow,SIGNAL(clickInfo()),this,SLOT(infoItem()));
}

CTransView::~CTransView()
{
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  QSqlRecord Record=Table.record(0);
  if (m_flFilter & FilterAccount) Record.setValue(8,ui->pTableView->horizontalHeader()->saveState());
  if (m_flFilter & FilterPayee) Record.setValue(9,ui->pTableView->horizontalHeader()->saveState());
  if (m_flFilter & FilterCategory) Record.setValue(10,ui->pTableView->horizontalHeader()->saveState());
  Table.setRecord(0,Record);
  Table.submitAll();

  delete ui;
}

QTableView* CTransView::tableView()
{
  return ui->pTableView;
}

QString CTransView::rangeDates()
{
  return ui->pLRangeDates->text();
}

QString CTransView::rangeAmounts()
{
  return ui->pLRangeAmounts->text();
}

QString CTransView::currencyName()
{
  QString s=ui->pLCurrency->text();
  s.remove("<b>");
  s.remove("</b>");
  return s;
}

CTransView::Filters CTransView::filter()
{
  return m_flFilter;
}

QVariant CTransView::id1()
{
  return m_Id1;
}

QVariant CTransView::id2()
{
  return m_Id2;
}

void CTransView::adjustTotal(double dTotNew)
{
  //Get Record for Acc Model
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+m_Id1.toString());
  ModelAcc.select();
  QSqlRecord Record=ModelAcc.record(0);

  //Get Actual Balance and calculate New Balance
  double dInitBal=Record.value(8).toDouble()+dTotNew-m_dGeneralTotal;

  //Set new Initial Balance and update view
  Record.setValue(8,dInitBal);
  ModelAcc.setRecord(0,Record);
  ModelAcc.submitAll();
  updateColumnBalance();
  updateGeneralBalance();
}

void CTransView::purgeTransactions(Purge iPurge,const QDate& Date)
{
  //Save general total
  double dTot=m_dGeneralTotal;

  //Get and reset selection model
  QItemSelectionModel *pSelection=ui->pTableView->selectionModel();
  pSelection->reset();

  //Purge transactions before Date
  if (iPurge==PurgeDate)
  {
    int iDate=Date.toJulianDay();
    int iRowCount=g_rowCount(*m_pModel);
    for (int i=0;i<iRowCount;i++)
      if (m_pModel->record(i).value(3).toInt()<iDate)
        pSelection->select(QItemSelection(m_pModel->index(i,0),m_pModel->index(i,14)),QItemSelectionModel::Select);
    deleteItem();
  }

  //Purge cleared transactions
  if (iPurge==PurgeClear)
  {
    int iRowCount=g_rowCount(*m_pModel);
    for (int i=0;i<iRowCount;i++)
      if (m_pModel->record(i).value(2).toInt()>0)
        pSelection->select(QItemSelection(m_pModel->index(i,0),m_pModel->index(i,14)),QItemSelectionModel::Select);
    deleteItem();
  }

  //Purge reconciled transactions
  if (iPurge==PurgeRecon)
  {
    int iRowCount=g_rowCount(*m_pModel);
    for (int i=0;i<iRowCount;i++)
      if (m_pModel->record(i).value(2).toInt()==2)
        pSelection->select(QItemSelection(m_pModel->index(i,0),m_pModel->index(i,14)),QItemSelectionModel::Select);
    deleteItem();
  }

  //Adjust final balance to the original
  pSelection->reset();
  adjustTotal(dTot);
}

bool CTransView::existInitBalance()
{
  return ((m_flFilter & FilterFromAcc)&&
          (ui->pCBPayee->currentIndex()==0)&&
          (ui->pCBClass->currentIndex()==0)&&
          (ui->pCBCategory->currentIndex()==0)&&
          ((ui->pCBStatus->currentIndex()==0)||(ui->pCBStatus->currentIndex()==1)||(ui->pCBStatus->currentIndex()==3)));
}

double CTransView::evaluateInitBalance()
{
  double dVal=0.0;

  if (existInitBalance())
  {
    QSqlTableModel ModelAcc;
    ModelAcc.setTable("account");
    ModelAcc.setFilter("idx="+m_Id1.toString());
    ModelAcc.select();
    dVal=ModelAcc.record(0).value(8).toDouble();
  }

  return dVal;
}

void CTransView::updateColumnBalance()
{
  //Add a secondary column for sort criteria to order records with same value in the main sort criteria
  amendSortCriteria();

  //Search view doen't need column balance
  if (m_flFilter & FilterFromSch) return;

  //Get the ending rate
  QVariant Val;
  g_findRecordFromTable("currency","idx",m_IdCurr.toString(),"rate",&Val);
  double dEndingRate=Val.toDouble();

  //Evaluate the column balance from last to first in case the Table is sort Descending by Date
  if ((ui->pTableView->horizontalHeader()->sortIndicatorSection()==3)&&
      (ui->pTableView->horizontalHeader()->sortIndicatorOrder()==Qt::DescendingOrder))
  {
    int iRowCount=g_rowCount(*m_pModel);
    for (int i=iRowCount-1;i>=0;i--)
    {
      //Get the rate for the transaction [i]
      double dRate;
      if (!(m_flFilter & FilterFromAcc))
      {
        g_findRecordFromTable("account","name",m_pModel->record(i).value(6).toString(),"currencyid",&Val);
        g_findRecordFromTable("currency","idx",Val.toString(),"rate",&Val);
        dRate=Val.toDouble()/dEndingRate;
      }
      else
       dRate=1.0;

      //Evaluate the balance for the transaction [i]
      double dBalance;
      if (i==iRowCount-1)
      {
        dBalance=m_pModel->record(i).value(9).toDouble()/dRate+evaluateInitBalance();
      }
      else
      {
        dBalance=m_pModel->record(i).value(9).toDouble()/dRate+
                 m_pModel->record(i+1).value(11).toDouble();
      }
      m_pModel->setData(m_pModel->index(i,11),dBalance);
    }
  }

  //Evaluate the column balance from first to last in any other case
  else
  {
    int iRowCount=g_rowCount(*m_pModel);
    for (int i=0;i<iRowCount;i++)
    {
      //Get the rate for the transaction [i]
      double dRate;
      if (!(m_flFilter & FilterFromAcc))
      {
        g_findRecordFromTable("account","name",m_pModel->record(i).value(6).toString(),"currencyid",&Val);
        g_findRecordFromTable("currency","idx",Val.toString(),"rate",&Val);
        dRate=Val.toDouble()/dEndingRate;
      }
      else
        dRate=1.0;

      //Evaluate the balance for the transaction [i]
      double dBalance;
      if (i==0)
      {
        dBalance=m_pModel->record(i).value(9).toDouble()/dRate+evaluateInitBalance();
      }
      else
      {
        dBalance=m_pModel->record(i).value(9).toDouble()/dRate+
                 m_pModel->record(i-1).value(11).toDouble();
      }
      m_pModel->setData(m_pModel->index(i,11),dBalance);
    }
  }
}

void CTransView::updateGeneralBalance()
{
  //Search view doen't need general balance
  if (m_flFilter & FilterFromSch) return;

  //Get the ending rate
  QVariant Val;
  g_findRecordFromTable("currency","idx",m_IdCurr.toString(),"rate",&Val);
  double dEndingRate=Val.toDouble();

  //Sum the rows of the Model
  m_dGeneralTotal=0.0;
  int iRowCount=g_rowCount(*m_pModel);
  for (int i=0;i<iRowCount;i++)
  {
    double dRate;
    if (!(m_flFilter & FilterFromAcc))
    {
      g_findRecordFromTable("account","name",m_pModel->record(i).value(6).toString(),"currencyid",&Val);
      g_findRecordFromTable("currency","idx",Val.toString(),"rate",&Val);
      dRate=Val.toDouble()/dEndingRate;
    }
    else
      dRate=1.0;

    m_dGeneralTotal+=m_pModel->record(i).value(9).toDouble()/dRate;
  }

  //In case the view is filter per Account, it includes the IniBalance
  m_dGeneralTotal+=evaluateInitBalance();

  //Format the number to the Locale before presenting it
  QString sTotal;
  QLocale Locale;
  if (m_dGeneralTotal<0)
    sTotal="("+ Locale.toString(m_dGeneralTotal*-1.0,'f',2) + ")";
  else
    sTotal=Locale.toString(m_dGeneralTotal,'f',2);

  ui->pLTotal->setText(sTotal);
}

void CTransView::updateView()
{
  //Filter and update account model
  filterList();

  //Update the values for column Balance
  updateColumnBalance();
}

void CTransView::showEvent( QShowEvent */*pEvent*/ )
{
  //Update base currency symbol
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.select();
  ModelCurr.setFilter("idx="+m_IdCurr.toString());
  ui->pLCurrency->setText("<b>["+ModelCurr.record(0).value(1).toString()+"]</b>");

  //Set bottom Date label
  QString sDateFormat=g_dateFormat();
  if ((g_FilterParam.date1().isValid())&&(g_FilterParam.date2().isValid()))
    ui->pLRangeDates->setText(g_FilterParam.date1().toString(sDateFormat)+" - "+g_FilterParam.date2().toString(sDateFormat));
  if ((g_FilterParam.date1().isValid())&&(!g_FilterParam.date2().isValid()))
    ui->pLRangeDates->setText(">="+g_FilterParam.date1().toString(sDateFormat));
  if ((!g_FilterParam.date1().isValid())&&(g_FilterParam.date2().isValid()))
    ui->pLRangeDates->setText("<="+g_FilterParam.date2().toString(sDateFormat));
  if ((!g_FilterParam.date1().isValid())&&(!g_FilterParam.date2().isValid()))
    ui->pLRangeDates->setText(tr("All dates"));

  //Set bottom Amount label
  if ((!g_FilterParam.amount1().isEmpty())&&(!g_FilterParam.amount2().isEmpty()))
    ui->pLRangeAmounts->setText(g_FilterParam.amount1()+" - "+g_FilterParam.amount2());
  if ((!g_FilterParam.amount1().isEmpty())&&(g_FilterParam.amount2().isEmpty()))
    ui->pLRangeAmounts->setText(">="+g_FilterParam.amount1());
  if ((g_FilterParam.amount1().isEmpty())&&(!g_FilterParam.amount2().isEmpty()))
    ui->pLRangeAmounts->setText("<="+g_FilterParam.amount2());
  if ((g_FilterParam.amount1().isEmpty())&&(g_FilterParam.amount2().isEmpty())&&(g_FilterParam.amountType()==0))
    ui->pLRangeAmounts->setText(tr("All amounts"));
  if ((g_FilterParam.amount1().isEmpty())&&(g_FilterParam.amount2().isEmpty())&&(g_FilterParam.amountType()==1))
    ui->pLRangeAmounts->setText(tr("Incomes"));
  if ((g_FilterParam.amount1().isEmpty())&&(g_FilterParam.amount2().isEmpty())&&(g_FilterParam.amountType()==2))
    ui->pLRangeAmounts->setText(tr("Expenses"));

  //Set bottom Cheque label
  if ((!g_FilterParam.cheque1().isEmpty())&&(!g_FilterParam.cheque2().isEmpty()))
    ui->pLRangeCheques->setText(g_FilterParam.cheque1()+" - "+g_FilterParam.cheque2());
  if ((!g_FilterParam.cheque1().isEmpty())&&(g_FilterParam.cheque2().isEmpty()))
    ui->pLRangeCheques->setText(">="+g_FilterParam.cheque1());
  if ((g_FilterParam.cheque1().isEmpty())&&(!g_FilterParam.cheque2().isEmpty()))
    ui->pLRangeCheques->setText("<="+g_FilterParam.cheque2());
  if ((g_FilterParam.cheque1().isEmpty())&&(g_FilterParam.cheque2().isEmpty()))
    ui->pLRangeCheques->setText(tr("All cheques"));

  //Update Comboxes
  if (m_flFilter & FilterFromSch)
  {
    g_refreshCombobox(ui->pCBAccount,g_FilterParam.accountid(),true);
    g_refreshCombobox(ui->pCBPayee,g_FilterParam.payeeid(),true);
    g_refreshComboCategory(ui->pCBCategory,g_FilterParam.categoryid(),QVariant(),true);
    g_refreshCombobox(ui->pCBClass,g_FilterParam.classid(),true);
  }
  else
  {
    g_refreshCombobox(ui->pCBAccount,QVariant(),true);
    g_refreshCombobox(ui->pCBPayee,QVariant(),true);
    g_refreshComboCategory(ui->pCBCategory,QVariant(),QVariant(),true);
    g_refreshCombobox(ui->pCBClass,QVariant(),true);
  }

  //Update Delegate for Category
  CDelegateTransView* pDelegate=qobject_cast<CDelegateTransView*>(ui->pTableView->itemDelegate());
  pDelegate->updateCategoryData();
  pDelegate->setDateFormat(sDateFormat);

  //Update column balance, table view and general balance
  g_updateFontSize(ui->pTableView);
  updateView();

  //Restore scrollbar value
  ui->pTableView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus to the TableView
  ui->pTableView->setFocus();
}

void CTransView::filterList()
{
  QString sFilter;

  //Set filter to show 'main split' or 'children split' depending on the Filter
  if (m_flFilter & FilterAccount)
    sFilter="split<"+QString::number(SPLIT);
  if (m_flFilter & FilterPayee)
    sFilter="categoryid!="+QString::number(ID_SPLIT);
  if (m_flFilter & FilterCategory)
    sFilter="";

  //Filter based on comboboxes
  g_filterText(ui->pCBAccount,"accountid",sFilter,true);
  g_filterText(ui->pCBPayee,"payeeid",sFilter,true);
  g_filterText(ui->pCBCategory,"categoryid",sFilter,false);
  g_filterText(ui->pCBClass,"classid",sFilter,true);
  g_filterStatus(ui->pCBStatus,sFilter);

  //Filter based on Date1/Date2 to limit the transactions between the 2 dates
  if (g_FilterParam.date1().isValid())
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="date>="+QString::number(g_FilterParam.date1().toJulianDay());
  }
  if (g_FilterParam.date2().isValid())
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="date<="+QString::number(g_FilterParam.date2().toJulianDay());
  }

  //Filter based on Amount1/Amount2
  if ((!g_FilterParam.amount1().isEmpty()) && (g_FilterParam.amountType()==0))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="abs(amount)>="+g_FilterParam.amount1();
  }
  if ((!g_FilterParam.amount1().isEmpty()) && (g_FilterParam.amountType()==1))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="abs(amount)>="+g_FilterParam.amount1()+" and amount>=0";
  }
  if ((!g_FilterParam.amount1().isEmpty()) && (g_FilterParam.amountType()==2))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="abs(amount)>="+g_FilterParam.amount1()+" and amount<0";
  }
  if ((!g_FilterParam.amount2().isEmpty()) && (g_FilterParam.amountType()==0))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="abs(amount)<="+g_FilterParam.amount2();
  }
  if ((!g_FilterParam.amount2().isEmpty()) && (g_FilterParam.amountType()==1))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="abs(amount)<="+g_FilterParam.amount2()+" and amount>=0";
  }
  if ((!g_FilterParam.amount2().isEmpty()) && (g_FilterParam.amountType()==2))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="abs(amount)<="+g_FilterParam.amount2()+" and amount<0";
  }
  if ((g_FilterParam.amount1().isEmpty()) && (g_FilterParam.amount2().isEmpty()) && (g_FilterParam.amountType()==1))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="amount>=0";
  }
  if ((g_FilterParam.amount1().isEmpty()) && (g_FilterParam.amount2().isEmpty()) && (g_FilterParam.amountType()==2))
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="amount<0";
  }

  //Filter based on Cheque1/Cheque2
  if (!g_FilterParam.cheque1().isEmpty())
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="(cheque>="+g_FilterParam.cheque1()+" and cheque!='')";
  }
  if (!g_FilterParam.cheque2().isEmpty())
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="(cheque<="+g_FilterParam.cheque2()+" and cheque!='')";
  }

  //Filter based on g_FilterParam.sMemo (search parameters)
  if (!g_FilterParam.memo().isEmpty())
  {
    QString s=g_FilterParam.memo();
    s.replace('|',"||");
    s.replace('%',"|%");
    s.replace('_',"|_");
    s.append('%');
    s.prepend('%');

    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="(trans.note like '"+s+"' ESCAPE '|')";
  }

  //Refresh TableView
  g_refreshView(ui->pTableView,true,sFilter);

  //Update General Balance
  updateGeneralBalance();

  //Set the menu enable status only if the window is currently visible
  if (isVisible())
  {
    if (m_flFilter & FilterFromAcc)
    {
      if (existInitBalance())
        g_pMainWindow->filterMenusByForm("CTransViewAd1PA1Pr1");
      else
        g_pMainWindow->filterMenusByForm("CTransViewAd1PA0Pr1");
    }
    else if (m_flFilter & FilterFromRep)
    {
      g_pMainWindow->filterMenusByForm("CTransViewAd0PA0Pr1");
    }
    else if (m_flFilter & FilterFromBud)
    {
      g_pMainWindow->filterMenusByForm("CTransViewAd0PA0Pr1");
    }
    else if (m_flFilter & FilterFromSch)
    {
      g_pMainWindow->filterMenusByForm("CTransViewAd0PA0Pr0");
    }
  }
}

void CTransView::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  m_iScrollValue=ui->pTableView->verticalScrollBar()->value();
}

void CTransView::infoItem()
{
  if (!isVisible()) return;

  if (ui->pTableView->selectionModel()->selectedRows().size()==1)
  {
    QVariant TransId;
    TransId=m_pModel->record(ui->pTableView->currentIndex().row()).value(0);
    g_pMainWindow->addForm("CTransInfo",TransId);
  }
}

void CTransView::addItem()
{
  if (!isVisible()) return;

  g_pMainWindow->addForm("CTransEdit",m_Id1,ActionAdd);
}

void CTransView::deleteItem()
{
  if (!isVisible()) return;

  QModelIndexList lstIndex=ui->pTableView->selectionModel()->selectedRows();
  if (lstIndex.count()==0) return;
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete %n selected transaction(s)?",0,lstIndex.count()))==QMessageBox::No) return;

  ui->pTableView->setFocus();

  bool bSplitTrans=false;
  QSqlQuery Query;
  Query.setForwardOnly(true);
  for (int i=0; i<lstIndex.count(); i++)
  {
    QModelIndex Index = lstIndex.at(i);
    QSqlRecord Record=m_pModel->record(Index.row());

    //In case the row is a split transaction, mark all children
    if (Record.value(7).toInt()==ID_SPLIT)
    {
      int iSplit=Record.value(14).toInt();
      Query.exec("UPDATE trans SET aux='x' WHERE split="+QString::number(SPLIT+iSplit));
      Query.exec("UPDATE trans SET aux='x' WHERE split="+QString::number(iSplit));
    }
    //In case the row is coming from a split transaction, do not delete
    else if (Record.value(14).toInt()>0)
    {
      bSplitTrans=true;
    }
    else
    {
      Query.exec("UPDATE trans SET aux='x' WHERE idx="+Record.value(0).toString());
      int iTransfer=Record.value(12).toInt();
      if (iTransfer>0)
        Query.exec("UPDATE trans SET aux='x' WHERE transfer="+QString::number(iTransfer)+
                   " and "+"idx!="+Record.value(0).toString());
    }
  }
  Query.exec("DELETE FROM trans WHERE aux='x'");

  //Update column balance, table view and general balance
  updateView();

  if (bSplitTrans)
    g_messageWarning(this,tr("Split transaction"),tr("Items coming from a split transactions cannot be deleted."));
}

void CTransView::on_pTableView_activated(QModelIndex Index)
{
  //When user click on the icon or status only select row
  if ((Index.column()==1)||(Index.column()==2)) return;

  //In case the row is coming from a split transaction, do not edit
  if ((m_pModel->record(Index.row()).value(14).toInt()>0) &&
      (m_pModel->record(Index.row()).value(7).toInt()!=ID_SPLIT))
  {
    g_messageWarning(this,tr("Split transaction"),tr("Items coming from a split transactions cannot be edited."));
    return;
  }

  QVariant TransId;
  TransId=m_pModel->record(Index.row()).value(0);
  g_pMainWindow->addForm("CTransEdit",TransId,ActionEdit);
}

void CTransView::on_pPBFilters_clicked()
{
  if (ui->pWidgetFilters->isVisible())
    ui->pWidgetFilters->hide();
  else
    ui->pWidgetFilters->show();
}

void CTransView::on_pPBClose_clicked()
{
  close();
}

void CTransView::menuColumnsChooser(QPoint Point)
{
  QMenu Menu;
  QList<QAction*> lstActions;
  QAction *pAction;
  QHeaderView *pHeader=ui->pTableView->horizontalHeader();

  //Create context menu using the applicable columns from the Header
  for (int i=0;i<pHeader->count();i++)
  {
    if ((i!=0)&&(i!=10)&&(i!=12)&&(i!=13)&&(i!=14)&&
        (((m_flFilter & FilterAccount)&&(i!=6))||
        ((m_flFilter & FilterPayee)&&(i!=5))||
        ((m_flFilter & FilterCategory)&&(i!=1)&&(i!=7))))
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

void CTransView::amendSortCriteria()
{
  //Save current index
  QModelIndex Index=ui->pTableView->selectionModel()->currentIndex();

  //Add column idx as secondary criteria for sort order
  QString sQuery=m_pModel->query().lastQuery();
  if (sQuery.endsWith("DESC"))
    sQuery+=", trans.idx DESC";
  else
    sQuery+=", trans.idx ASC";

  qobject_cast<QSqlQueryModel*>(m_pModel)->setQuery(QSqlQuery(sQuery));

  //Restore current index
  if (Index.isValid())
  {
    ui->pTableView->selectionModel()->setCurrentIndex(Index,QItemSelectionModel::NoUpdate);
    ui->pTableView->selectionModel()->select(QItemSelection(m_pModel->index(Index.row(),0),m_pModel->index(Index.row(),m_pModel->columnCount()-1)),
                                             QItemSelectionModel::Select);
  }
}


