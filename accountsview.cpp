#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "accountsview.h"
#include "ui_accountsview.h"
#include "mainwindow0.h"
#include "login.h"
#include "basedelegate.h"
#include "global.h"
#include "dr_profile.h"

CAccountsView::CAccountsView(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CAccountsView)
{
  ui->setupUi(this);

  m_pModel=0;
  m_pModelType=0;
  m_pModelBank=0;
  m_pModelCurrency=0;
  m_iScrollValue=0;
#ifdef MOBILE_PLATFORM
  ui->pWidgetFilters->setVisible(false);
  ui->pPBFilters->setVisible(true);
#else
  ui->pWidgetFilters->setVisible(true);
  ui->pPBFilters->setVisible(false);
#endif

  //Set Delegate features
  CBaseDelegate* pDelegate=new CBaseDelegate(ui->pTableView);
  pDelegate->setNumberPrecision(2);
  pDelegate->addColumnFormat(1,CBaseDelegate::FormatIcon);
  pDelegate->addColumnFormat(10,CBaseDelegate::FormatNumber);

  //Set TableView features that are not affected by changing the Model
  ui->pTableView->setSortingEnabled(true);
  ui->pTableView->setItemDelegate(pDelegate);
  ui->pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::NoPen);
  ui->pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Set HeaderView features that are not affected by changing the Model
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionsClickable(true);
  pHeader->setSectionsMovable(true);
  pHeader->setSortIndicatorShown(true);
  pHeader->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(pHeader,SIGNAL(sectionClicked(int)),this,SLOT(updateColumnTotal()));
  connect(pHeader,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(menuColumnsChooser(QPoint)));
  ui->pTableView->verticalHeader()->hide();

  //Set connections
  connect(ui->pCBType,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBBank,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBCurrency,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBStatus,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(ui->pCBAccStatus,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateView()));
  connect(g_pMainWindow,SIGNAL(beforeChangeDatabase()),this,SLOT(saveViewState()));
  connect(g_pMainWindow,SIGNAL(afterChangeDatabase()),this,SLOT(updateViewState()));
  connect(g_pMainWindow,SIGNAL(newTransaction()),this,SLOT(updateView()));
  connect(g_pMainWindow,SIGNAL(clickReport()),this,SLOT(reportForm()));
  connect(g_pMainWindow,SIGNAL(clickInfo()),this,SLOT(infoItem()));
  connect(g_pMainWindow,SIGNAL(beforeCloseEvent()),this,SLOT(saveViewState()));

  //Initialize AccountView state from Database
  updateViewState();
}

CAccountsView::~CAccountsView()
{
  delete ui;
}

void CAccountsView::updateViewState()
{
  if (g_isActiveDatabase())
  {
    if (m_pModel) delete m_pModel;
    m_pModel=new QSqlRelationalTableModel(this);
    m_pModel->setTable("account");
    m_pModel->setRelation(2,QSqlRelation("type","idx","name"));
    m_pModel->setRelation(4,QSqlRelation("bank","idx","name"));
    m_pModel->setRelation(5,QSqlRelation("currency","idx","name"));
    m_pModel->setSort(3,Qt::AscendingOrder);
    m_pModel->setHeaderData(1,Qt::Horizontal,tr("I"));
    m_pModel->setHeaderData(2,Qt::Horizontal,tr("Type"));
    m_pModel->setHeaderData(3,Qt::Horizontal,tr("Name"));
    m_pModel->setHeaderData(4,Qt::Horizontal,tr("Bank"));
    m_pModel->setHeaderData(5,Qt::Horizontal,tr("Currency"));
    m_pModel->setHeaderData(10,Qt::Horizontal,tr("Total"));
    m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_pModel->select();

    ui->pTableView->setModel(m_pModel);
    ui->pTableView->setColumnHidden(0,true);
    ui->pTableView->setColumnHidden(6,true);
    ui->pTableView->setColumnHidden(7,true);
    ui->pTableView->setColumnHidden(8,true);
    ui->pTableView->setColumnHidden(9,true);
    ui->pTableView->setColumnHidden(11,true);
    ui->pTableView->resizeColumnsToContents();

    QHeaderView * pHeader=ui->pTableView->horizontalHeader();
    pHeader->setSortIndicator(3,Qt::AscendingOrder);
    pHeader->setSectionResizeMode(QHeaderView::Interactive);
    pHeader->setSectionResizeMode(1,QHeaderView::Fixed);

    //Initiate Type ComboBox
    if (m_pModelType) delete m_pModelType;
    m_pModelType=new QSqlTableModel(this);
    m_pModelType->setTable("type");
    m_pModelType->setSort(1,Qt::AscendingOrder);
    ui->pCBType->setModel(m_pModelType);
    ui->pCBType->setModelColumn(1);

    //Initiate Bank ComboBox
    if (m_pModelBank) delete m_pModelBank;
    m_pModelBank=new QSqlTableModel(this);
    m_pModelBank->setTable("bank");
    m_pModelBank->setSort(1,Qt::AscendingOrder);
    ui->pCBBank->setModel(m_pModelBank);
    ui->pCBBank->setModelColumn(1);

    //Initiate Currency ComboBox
    if (m_pModelCurrency) delete m_pModelCurrency;
    m_pModelCurrency=new QSqlTableModel(this);
    m_pModelCurrency->setTable("currency");
    ui->pCBCurrency->setModel(m_pModelCurrency);
    ui->pCBCurrency->setModelColumn(1);

    //Update HEADER, FILTER combobox and CLEAR button from database
    QSqlTableModel Table;
    Table.setTable("settings");
    Table.select();

    //Load HorizontalHeader state
    QByteArray ByteArray=Table.record(0).value(3).toByteArray();
    ui->pTableView->horizontalHeader()->restoreState(ByteArray);

    //Load ComboBox selection states
    g_refreshCombobox(ui->pCBType,Table.record(0).value(5),true);
    g_refreshCombobox(ui->pCBBank,Table.record(0).value(6),true);
    g_refreshCombobox(ui->pCBCurrency,Table.record(0).value(7),true);
    g_initComboboxStatus(ui->pCBStatus,static_cast<Status>(Table.record(0).value(4).toInt()));
    g_initComboboxAccStatus(ui->pCBAccStatus,1);

    //Update column total, table view and general balance and font
    g_updateFontSize(ui->pTableView);
    updateView();

    //Enable controls for the view
    ui->pCBType->setEnabled(true);
    ui->pCBBank->setEnabled(true);
    ui->pCBCurrency->setEnabled(true);
    ui->pCBStatus->setEnabled(true);
    ui->pCBAccStatus->setEnabled(true);
    ui->pPBAccounts->setEnabled(true);
    ui->pPBFilters->setEnabled(true);
  }
  else
  {
    ui->pCBType->blockSignals(true);
    ui->pCBBank->blockSignals(true);
    ui->pCBCurrency->blockSignals(true);
    ui->pCBStatus->blockSignals(true);
    ui->pCBAccStatus->blockSignals(true);

    ui->pTableView->setModel(0);
    ui->pCBType->setCurrentIndex(-1);
    ui->pCBBank->setCurrentIndex(-1);
    ui->pCBCurrency->setCurrentIndex(-1);
    ui->pCBStatus->setCurrentIndex(-1);
    ui->pCBAccStatus->setCurrentIndex(-1);

    ui->pCBType->blockSignals(false);
    ui->pCBBank->blockSignals(false);
    ui->pCBCurrency->blockSignals(false);
    ui->pCBStatus->blockSignals(false);
    ui->pCBAccStatus->blockSignals(false);

    //Enable controls for the view
    ui->pCBType->setEnabled(false);
    ui->pCBBank->setEnabled(false);
    ui->pCBCurrency->setEnabled(false);
    ui->pCBStatus->setEnabled(false);
    ui->pCBAccStatus->setEnabled(false);
    ui->pPBAccounts->setEnabled(false);
    ui->pPBFilters->setEnabled(false);
    ui->pLTotal->setText("");
  }
}

void CAccountsView::saveViewState()
{
  if (!g_isActiveDatabase()) return;

  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  QSqlRecord Record=Table.record(0);
  Record.setValue(3,ui->pTableView->horizontalHeader()->saveState());
  Record.setValue(4,ui->pCBStatus->currentIndex());
  Record.setValue(5,m_pModelType->record(ui->pCBType->currentIndex()).value(0));
  Record.setValue(6,m_pModelBank->record(ui->pCBBank->currentIndex()).value(0));
  Record.setValue(7,m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(0));

  Table.setRecord(0,Record);
  Table.submitAll();
}

void CAccountsView::showEvent( QShowEvent */*pEvent*/ )
{
  if (!g_isActiveDatabase()) return;

  //Update base currency symbol
  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.select();
  ui->pLCurrency->setText("<b>["+ModelCurr.record(0).value(1).toString()+"]</b>");

  //Update Comboxes
  g_refreshCombobox(ui->pCBBank,QVariant(),true);
  g_refreshCombobox(ui->pCBType,QVariant(),true);
  g_refreshCombobox(ui->pCBCurrency,QVariant(),true);

  //Update column total, table view and general balance and font
  g_updateFontSize(ui->pTableView);
  updateView();

  //Restore scrollbar value
  ui->pTableView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the TableView
  ui->pTableView->setFocus();
}

void CAccountsView::filterList()
{
  QString sFilter="";
  g_filterText(ui->pCBBank,"bankid",sFilter,true);
  g_filterText(ui->pCBCurrency,"currencyid",sFilter,true);
  g_filterText(ui->pCBType,"typeid",sFilter,true);

  if (!sFilter.isEmpty()) sFilter+=" and ";
  sFilter+="status="+QString::number(ui->pCBAccStatus->currentIndex());

  g_refreshView(ui->pTableView,true,sFilter);
}

void CAccountsView::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  m_iScrollValue=ui->pTableView->verticalScrollBar()->value();
}

void CAccountsView::on_pTableView_activated(QModelIndex Index)
{
  //When user click on the icon only select row
  if (Index.column()==1) return;

  //Get account Id
  QVariant IdAcc=m_pModel->record(Index.row()).value(0);

  //Get currency Id
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.select();
  ModelAcc.setFilter("idx="+IdAcc.toString());
  QVariant IdCurr=ModelAcc.record(0).value(5);

  //Prepare filtering parameters and open TransView
  g_FilterParam.setParams(QDate(),QDate(),"","","","","",ID_ANY,ID_ANY,ID_ANY,ID_ANY,0);
  g_pMainWindow->addForm("CTransView",IdAcc,0,static_cast<Status>(ui->pCBStatus->currentIndex()),
                         CTransView::FilterAccount|CTransView::FilterFromAcc,IdCurr);
}

void CAccountsView::on_pPBFilters_clicked()
{
  if (ui->pWidgetFilters->isVisible())
    ui->pWidgetFilters->hide();
  else
    ui->pWidgetFilters->show();
}

void CAccountsView::on_pPBAccounts_clicked()
{
  QVariant IdAcc;
  if (ui->pTableView->selectionModel()->selectedRows().size()==1)
    IdAcc=m_pModel->record(ui->pTableView->currentIndex().row()).value(0);
  g_pMainWindow->addForm("CAccountEdit",IdAcc);
}

void CAccountsView::infoItem()
{
  if (!isVisible()) return;

  if (ui->pTableView->selectionModel()->selectedRows().size()==1)
  {
    QVariant IdAcc;
    IdAcc=m_pModel->record(ui->pTableView->currentIndex().row()).value(0);
    g_pMainWindow->addForm("CAccountInfo",IdAcc);
  }
}

void CAccountsView::reportForm()
{
  if (!isVisible()) return;

  g_pMainWindow->addForm("CReport",0,0,static_cast<Status>(ui->pCBStatus->currentIndex()));
}

void CAccountsView::menuColumnsChooser(QPoint Point)
{
  QMenu Menu;
  QList<QAction*> lstActions;
  QAction *pAction;
  QHeaderView *pHeader=ui->pTableView->horizontalHeader();

  //Create context menu using the applicable columns from the Header
  for (int i=0;i<pHeader->count();i++)
  {
    if ((i!=0)&&(i!=6)&&(i!=7)&&(i!=8)&&(i!=9))
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

void CAccountsView::updateView()
{
  //Filter and update account model
  filterList();

  //Update the values for column Total
  updateColumnTotal();

  //Update General Total
  updateGeneralTotal();
}


void CAccountsView::updateColumnTotal()
{
  int iRowCount=g_rowCount(*m_pModel);
  for (int i=0;i<iRowCount;i++)
  {
    QSqlQuery Query;
    Query.setForwardOnly(true);
    QSqlRecord Record=m_pModel->record(i);
    QString sId=Record.value(0).toString();

    //Prepare filter based on Status combobox
    QString sFilter="";
    g_filterStatus(ui->pCBStatus,sFilter);

    //Query Database and get the Total
    Query.exec("SELECT SUM(amount) FROM trans WHERE "+sFilter+" and accountid="+sId+" and split<"+QString::number(SPLIT));
    double dTotal=0.0;
    if(Query.next()) dTotal=Query.value(0).toDouble();

    //Add the beginning balance only for certain Status options
    int iStatus=ui->pCBStatus->currentIndex();
    if ((iStatus==0)||(iStatus==1)||(iStatus==3))
      dTotal+=Record.value(8).toDouble();

    m_pModel->setData(m_pModel->index(i,10),dTotal);
  }
}

void CAccountsView::updateGeneralTotal()
{
  //Sum the rows of the Model
  double dTotal=0.0;
  int iRowCount=g_rowCount(*m_pModel);
  for (int i=0;i<iRowCount;i++)
  {
    QVariant Val;
    g_findRecordFromTable("currency","name",m_pModel->record(i).value(5).toString(),"rate",&Val);
    dTotal+=m_pModel->record(i).value(10).toDouble()/Val.toDouble();
  }

  //Format the number to the Locale before presenting it
  QString sTotal;
  QLocale Locale;
  if (dTotal<0)
    sTotal="("+ Locale.toString(dTotal*-1.0,'f',2) + ")";
  else
    sTotal=Locale.toString(dTotal,'f',2);

  ui->pLTotal->setText(sTotal);
}


