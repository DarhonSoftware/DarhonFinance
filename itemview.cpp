#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "itemview.h"
#include "ui_itemview.h"
#include "global.h"
#include "groupedit.h"
#include "itemedit.h"
#include "iteminfo.h"
#include "mainwindow0.h"


CItemView::CItemView(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CItemView)
{
  ui->setupUi(this);
  m_iScrollValue=0;
  g_updateFontSize(ui->pTableView);

  //Set model transations for view
  m_pModel=new QSqlRelationalTableModel(this);
  m_pModel->setTable("sbitem");
  m_pModel->setRelation(1,QSqlRelation("sbgroup","idx","name"));
  m_pModel->setSort(2,Qt::AscendingOrder);
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_pModel->select();

  //Set TableView features
  ui->pTableView->setSortingEnabled(true);
  ui->pTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::NoPen);
  ui->pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->pTableView->setModel(m_pModel);
  ui->pTableView->setColumnHidden(0,true);
  ui->pTableView->setColumnHidden(1,true);
  ui->pTableView->setColumnHidden(10,true);

  //Set HeaderView features
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionsClickable(true);
  pHeader->setSectionsMovable(false);
  pHeader->setSortIndicatorShown(true);
  pHeader->setSortIndicator(2,Qt::AscendingOrder);
  pHeader->setSectionResizeMode(QHeaderView::Interactive);
  ui->pTableView->verticalHeader()->hide();

  //Initiate Group ComboBox
  m_pModelGroup=new QSqlTableModel(this);
  m_pModelGroup->setTable("sbgroup");
  m_pModelGroup->setSort(1,Qt::AscendingOrder);
  ui->pCBGroup->setModel(m_pModelGroup);
  ui->pCBGroup->setModelColumn(1);

  //Update GROUP combobox from database
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  //Load ComboBox selection states
  g_refreshCombobox(ui->pCBGroup,Table.record(0).value(22),false);

  //Set connections
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
  connect(g_pMainWindow,SIGNAL(clickInfo()),this,SLOT(infoItem()));
}

CItemView::~CItemView()
{
  QSqlTableModel Table;
  Table.setTable("settings");
  Table.select();

  QSqlRecord Record=Table.record(0);
  int iIdx=ui->pCBGroup->currentIndex();
  if (iIdx==-1)
    Record.setValue(22,0);
  else
    Record.setValue(22,m_pModelGroup->record(ui->pCBGroup->currentIndex()).value(0));

  Table.setRecord(0,Record);
  Table.submitAll();

  delete ui;
}

void CItemView::on_pCBGroup_currentIndexChanged(int /*iIndex*/)
{
  //Reset sort status
  m_pModel->setSort(2,Qt::AscendingOrder);
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSortIndicator(2,Qt::AscendingOrder);

  //Filter TableView
  filterList();
}

void CItemView::on_pPBClose_clicked()
{
  close();
}

void CItemView::on_pTableView_activated(QModelIndex Index)
{
  //When user click on the first column only select row
  if (Index.column()==2) return;

  QVariant ItemId;
  ItemId=m_pModel->record(Index.row()).value(0);
  g_pMainWindow->addForm("CItemEdit",ItemId,ActionEdit);
}

void CItemView::on_pPBGroups_clicked()
{
  //Get current group
  QVariant IdGroup=QVariant();
  int iIdx=ui->pCBGroup->currentIndex();
  if (iIdx!=-1) IdGroup=m_pModelGroup->record(iIdx).value(0);

  g_pMainWindow->addForm("CGroupEdit",IdGroup);
}

void CItemView::addItem()
{
  if (!isVisible()) return;
  if (ui->pCBGroup->currentIndex()==-1) return;

  QVariant IdGroup=m_pModelGroup->record(ui->pCBGroup->currentIndex()).value(0);
  g_pMainWindow->addForm("CItemEdit",IdGroup,ActionAdd);
}

void CItemView::deleteItem()
{
  if (!isVisible()) return;

  QModelIndexList lstIndex=ui->pTableView->selectionModel()->selectedRows();
  if (lstIndex.count()==0) return;
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete %n selected item(s)?",0,lstIndex.count()))==QMessageBox::No) return;

  ui->pTableView->setFocus();

  //Mark records to be deleted
  for (int i=0; i<lstIndex.count(); i++)
  {
    QModelIndex Index = lstIndex.at(i);
    m_pModel->setData(m_pModel->index(Index.row(),10),"x");
  }
  m_pModel->submitAll();

  //Delete marked records
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("DELETE FROM sbitem WHERE aux='x'");

  //Update Model
  m_pModel->select();
}

void CItemView::infoItem()
{
  if (!isVisible()) return;

  if (ui->pTableView->selectionModel()->selectedRows().size()==1)
  {
    QVariant InfoId;
    InfoId=m_pModel->record(ui->pTableView->currentIndex().row()).value(0);
    g_pMainWindow->addForm("CItemInfo",InfoId);
  }
}

void CItemView::filterList()
{
  QString sFilter="";

  //Get filter from comboboxes
  g_filterText(ui->pCBGroup,"groupid",sFilter,true);

  //Refresh TableView & Headers
  g_refreshView(ui->pTableView,true,sFilter);
  updateHeaders();
  ui->pTableView->resizeColumnsToContents();
}

void CItemView::updateHeaders()
{
  int iIdx=ui->pCBGroup->currentIndex();
  if (iIdx==-1)
  {
    ui->pTableView->setColumnHidden(2,true);
    ui->pTableView->setColumnHidden(3,true);
    ui->pTableView->setColumnHidden(4,true);
    ui->pTableView->setColumnHidden(5,true);
    ui->pTableView->setColumnHidden(6,true);
    ui->pTableView->setColumnHidden(7,true);
    ui->pTableView->setColumnHidden(8,true);
    ui->pTableView->setColumnHidden(9,true);
  }
  else
  {
    QSqlTableModel Table;
    Table.setTable("sbgroup");
    Table.setFilter("idx="+m_pModelGroup->record(iIdx).value(0).toString());
    Table.select();

    QSqlRecord Record=Table.record(0);
    for (int i=2;i<10;i++)
    {
      QString sField=Record.value(i).toString();
      if (sField.isEmpty())
      {
        ui->pTableView->setColumnHidden(i,true);
      }
      else
      {
        m_pModel->setHeaderData(i,Qt::Horizontal,sField);
        ui->pTableView->setColumnHidden(i,false);
      }
    }
  }
}

void CItemView::showEvent( QShowEvent */*pEvent*/ )
{
  //Update Comboxes
  g_refreshCombobox(ui->pCBGroup,QVariant(),false);

  //Filter and update account model & headers
  filterList();
  g_updateFontSize(ui->pTableView);

  //Restore scrollbar value
  ui->pTableView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus to the TableView
  ui->pTableView->setFocus();
}

void CItemView::hideEvent ( QHideEvent */*pEvent*/ )
{
  //Save scrollbar value
  m_iScrollValue=ui->pTableView->verticalScrollBar()->value();
}

