#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "generallist.h"
#include "ui_generallist.h"
#include "formbase.h"
#include "dialogicons.h"
#include "global.h"
#include "mainwindow0.h"
#include "dr_profile.h"

CGeneralList::CGeneralList(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CGeneralList)
{
  ui->setupUi(this);

  //Setup children
  ui->pLEIconCategory->setVisible(false);
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    ui->pPBMemo->setEnabled(true);
  }
  else
  {
    ui->pPBMemo->setEnabled(false);
  }
#ifdef MOBILE_PLATFORM
  ui->pRTEBank->setVisible(false);
#else
  ui->pPBMemo->hide();
  ui->pVLBank->addWidget(ui->pRTEBank);
#endif

  //Set members
  m_iScrollValue=0;

  //Initiate Bank Tab
  m_pModelBank=new QSqlTableModel(this);
  m_pModelBank->setTable("bank");
  m_pModelBank->setSort(1,Qt::AscendingOrder);
  m_pModelBank->setFilter("idx>0");
  m_pModelBank->setEditStrategy(QSqlTableModel::OnManualSubmit);

  ui->pListViewBank->setModel(m_pModelBank);
  ui->pListViewBank->setModelColumn(1);
  ui->pListViewBank->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pListViewBank->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Initiate Payee Tab
  m_pModelPayee=new QSqlTableModel(this);
  m_pModelPayee->setTable("payee");
  m_pModelPayee->setSort(1,Qt::AscendingOrder);
  m_pModelPayee->setFilter("idx>0");
  m_pModelPayee->setEditStrategy(QSqlTableModel::OnManualSubmit);

  ui->pListViewPayee->setModel(m_pModelPayee);
  ui->pListViewPayee->setModelColumn(1);
  ui->pListViewPayee->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pListViewPayee->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Initiate Category Tab
  m_pModelCategory=new QSqlTableModel(this);
  m_pModelCategory->setTable("category");
  m_pModelCategory->setSort(1,Qt::AscendingOrder);
  m_pModelCategory->setFilter("idx>0");
  m_pModelCategory->setEditStrategy(QSqlTableModel::OnManualSubmit);

  ui->pListViewCategory->setModel(m_pModelCategory);
  ui->pListViewCategory->setModelColumn(1);
  ui->pListViewCategory->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pListViewCategory->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Initiate Class Tab
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  m_pModelClass->setFilter("idx>0");
  m_pModelClass->setEditStrategy(QSqlTableModel::OnManualSubmit);

  ui->pListViewClass->setModel(m_pModelClass);
  ui->pListViewClass->setModelColumn(1);
  ui->pListViewClass->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pListViewClass->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Initiate Type Tab
  m_pModelType=new QSqlTableModel(this);
  m_pModelType->setTable("type");
  m_pModelType->setSort(1,Qt::AscendingOrder);
  m_pModelType->setFilter("idx>0");
  m_pModelType->setEditStrategy(QSqlTableModel::OnManualSubmit);

  ui->pListViewType->setModel(m_pModelType);
  ui->pListViewType->setModelColumn(1);
  ui->pListViewType->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->pListViewType->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Connect selection model signals
  connect(ui->pListViewBank, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));
  connect(ui->pListViewPayee, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));
  connect(ui->pListViewCategory, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));
  connect(ui->pListViewClass, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));
  connect(ui->pListViewType, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));

  //Connect controls to detect editing
  connect(ui->pLENameBank,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEAddressBank,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEOtherBank,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pRTEBank,SIGNAL(textEdited()),this,SLOT(setDirty()));
  connect(ui->pLENamePayee,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLENameCategory,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEIconCategory,SIGNAL(textChanged(QString)),this,SLOT(setDirty()));
  connect(ui->pLENameClass,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLENameType,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
}

CGeneralList::~CGeneralList()
{
  delete ui;
}

void CGeneralList::on_pPBApply_clicked()
{
  QModelIndex Index;

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    Index=ui->pListViewBank->currentIndex();
    if (!Index.isValid()) return;

    //Validate data
    QString sName=ui->pLENameBank->text().simplified();
    if (sName.isEmpty()) return;
    QModelIndexList lstIndex=m_pModelBank->match(m_pModelBank->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
    if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
    {
      g_messageWarning(this,tr("Apply"),tr("The bank's name is already in use"));
      return;
    }

    //Write data into database
    QSqlRecord Record=m_pModelBank->record(Index.row());
    Record.setValue(1,sName);
    Record.setValue(2,ui->pLEAddressBank->text().simplified());
    Record.setValue(3,ui->pLEOtherBank->text().simplified());
    Record.setValue(4,ui->pRTEBank->toHtml());
    m_pModelBank->setRecord(Index.row(),Record);
    m_pModelBank->submitAll();
    cleanForm();

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(false);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    Index=ui->pListViewPayee->currentIndex();
    if (!Index.isValid()) return;

    //Validate empty name
    QString sName=ui->pLENamePayee->text().simplified();
    if (sName.isEmpty()) return;

    //Validate name
    if (sName==TRANSFER_STR)
    {
      g_messageWarning(this,tr("Apply"),tr("The payee cannot use the reserved name ") + TRANSFER_QTD);
      return;
    }

    //Validate name duplication
    g_fetchAllData(m_pModelPayee); //Populates model before match operation
    QModelIndexList lstIndex=m_pModelPayee->match(m_pModelPayee->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
    if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
    {
      g_messageWarning(this,tr("Apply"),tr("The payee's name is already in use"));
      return;
    }

    //Write data into database
    QSqlRecord Record=m_pModelPayee->record(Index.row());
    Record.setValue(1,sName);
    m_pModelPayee->setRecord(Index.row(),Record);
    m_pModelPayee->submitAll();
    cleanForm();

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(false);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    Index=ui->pListViewCategory->currentIndex();
    if (!Index.isValid()) return;

    //Validate empty name
    QString sName=ui->pLENameCategory->text().simplified();
    if (sName.isEmpty()) return;

    //Validate name
    if (sName==SPLIT_STR)
    {
      g_messageWarning(this,tr("Apply"),tr("The category cannot use the reserved name ") + SPLIT_QTD);
      return;
    }

    //Validate name duplication
    g_fetchAllData(m_pModelCategory); //Populates model before match operation
    QModelIndexList lstIndex=m_pModelCategory->match(m_pModelCategory->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
    if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
    {
      g_messageWarning(this,tr("Apply"),tr("The category's name is already in use"));
      return;
    }

    //Write data into database
    QSqlRecord Record=m_pModelCategory->record(Index.row());
    Record.setValue(1,sName);
    Record.setValue(2,ui->pLEIconCategory->text());
    m_pModelCategory->setRecord(Index.row(),Record);
    m_pModelCategory->submitAll();
    cleanForm();

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(false);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    Index=ui->pListViewClass->currentIndex();
    if (!Index.isValid()) return;

    //Validate data
    QString sName=ui->pLENameClass->text().simplified();
    if (sName.isEmpty()) return;
    QModelIndexList lstIndex=m_pModelClass->match(m_pModelClass->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
    if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
    {
      g_messageWarning(this,tr("Apply"),tr("The class' name is already in use"));
      return;
    }

    //Write data into database
    QSqlRecord Record=m_pModelClass->record(Index.row());
    Record.setValue(1,sName);
    m_pModelClass->setRecord(Index.row(),Record);
    m_pModelClass->submitAll();
    cleanForm();

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(false);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    Index=ui->pListViewType->currentIndex();
    if (!Index.isValid()) return;

    //Validate data
    QString sName=ui->pLENameType->text().simplified();
    if (sName.isEmpty()) return;
    QModelIndexList lstIndex=m_pModelType->match(m_pModelType->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
    if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
    {
      g_messageWarning(this,tr("Apply"),tr("The type's name is already in use"));
      return;
    }

    //Write data into database
    QSqlRecord Record=m_pModelType->record(Index.row());
    Record.setValue(1,sName);
    m_pModelType->setRecord(Index.row(),Record);
    m_pModelType->submitAll();
    cleanForm();

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(false);
  }
}

void CGeneralList::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBClose->setVisible(!bChecked);
  ui->pTabWidget->setVisible(!bChecked);
  ui->pRTEBank->setVisible(bChecked);
}

void CGeneralList::on_pPBClose_clicked()
{
  close();
}

void CGeneralList::on_pTBIconCategory_clicked()
{
  CDialogIcons Dialog(this);
  int iResult=Dialog.exec();
  if (iResult==QDialog::Rejected) return;

  QString s=Dialog.icon();
  ui->pLEIconCategory->setText(s);
  updateIcon();
}

void CGeneralList::updateIcon()
{
  QString sIcon=ui->pLEIconCategory->text();
  if (sIcon.isEmpty())
  {
    QIcon Icon(":/others/icons/drfinance32.png");
    ui->pTBIconCategory->setIcon(Icon);
  }
  else
  {
    QIcon Icon(sIcon);
    ui->pTBIconCategory->setIcon(Icon);
  }
}

void CGeneralList::addItem()
{
  if (!isVisible()) return;

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    addItem1("bank", ui->pListViewBank, m_pModelBank, ui->pLENameBank);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    addItem1("payee", ui->pListViewPayee, m_pModelPayee, ui->pLENamePayee);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    addItem1("category", ui->pListViewCategory, m_pModelCategory, ui->pLENameCategory);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    addItem1("class", ui->pListViewClass, m_pModelClass, ui->pLENameClass);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    addItem1("type", ui->pListViewType, m_pModelType, ui->pLENameType);
  }
}

void CGeneralList::deleteItem()
{
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    deleteItem1("SELECT idx FROM account WHERE bankid=","","",
                "DELETE FROM bank WHERE aux='x'",ui->pListViewBank,m_pModelBank);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    deleteItem1("SELECT idx FROM account WHERE typeid=","","",
                "DELETE FROM type WHERE aux='x'",ui->pListViewType,m_pModelType);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    deleteItem1("SELECT idx FROM trans WHERE payeeid=","SELECT idx FROM schedule WHERE payeeid=","",
               "DELETE FROM payee WHERE aux='x'",ui->pListViewPayee,m_pModelPayee);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    deleteItem1("SELECT idx FROM trans WHERE categoryid=","SELECT idx FROM schedule WHERE categoryid=","SELECT idx FROM budgetcat WHERE categoryid=",
               "DELETE FROM category WHERE aux='x'",ui->pListViewCategory,m_pModelCategory);
  }
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    deleteItem1("SELECT idx FROM trans WHERE classid=","SELECT idx FROM schedule WHERE classid=","SELECT idx FROM budget WHERE classid=",
               "DELETE FROM class WHERE aux='x'",ui->pListViewClass,m_pModelClass);
  }
}

void CGeneralList::on_pTabWidget_currentChanged(int /*iIndex*/)
{
  ui->pPBApply->setEnabled(false);
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    ui->pPBMemo->setEnabled(true);
  }
  else
  {
    ui->pPBMemo->setEnabled(false);
  }
}

void CGeneralList::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    ui->pLENameBank->setText(m_pModelBank->record(Index.row()).value(1).toString());
    ui->pLEAddressBank->setText(m_pModelBank->record(Index.row()).value(2).toString());
    ui->pLEOtherBank->setText(m_pModelBank->record(Index.row()).value(3).toString());
    ui->pRTEBank->setHtml(m_pModelBank->record(Index.row()).value(4).toString());

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    ui->pLENamePayee->setText(m_pModelPayee->record(Index.row()).value(1).toString());

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    QString s=m_pModelCategory->record(Index.row()).value(2).toString();
    ui->pLEIconCategory->setText(s);
    ui->pLENameCategory->setText(m_pModelCategory->record(Index.row()).value(1).toString());
    updateIcon();

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    ui->pLENameClass->setText(m_pModelClass->record(Index.row()).value(1).toString());

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    ui->pLENameType->setText(m_pModelType->record(Index.row()).value(1).toString());

    //Set visible and enable controls
    ui->pPBApply->setEnabled(false);
    enableForm(true);
  }
}

void CGeneralList::setDirty()
{
  QModelIndex Index;

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    Index=ui->pListViewBank->currentIndex();
    if (Index.isValid())
      ui->pPBApply->setEnabled(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    Index=ui->pListViewPayee->currentIndex();
    if (Index.isValid())
      ui->pPBApply->setEnabled(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    Index=ui->pListViewCategory->currentIndex();
    if (Index.isValid())
      ui->pPBApply->setEnabled(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    Index=ui->pListViewClass->currentIndex();
    if (Index.isValid())
      ui->pPBApply->setEnabled(true);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    Index=ui->pListViewType->currentIndex();
    if (Index.isValid())
      ui->pPBApply->setEnabled(true);
  }
}

void CGeneralList::deleteItem1(const QString &sQuerySearch1, const QString &sQuerySearch2, const QString &sQuerySearch3,
                               const QString &sQueryDel,QListView *pListView,	QSqlTableModel *pModel)
{
  if (!isVisible()) return;

  pListView->setFocus();

  //Obtain the selected items
  QModelIndexList lstIndex=pListView->selectionModel()->selectedIndexes();
  if (lstIndex.count()==0) return;

  //Confirm deletion
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete %n selected item(s)?",0,lstIndex.count()))==QMessageBox::No) return;

  //Check if itmes are being used in dependent tables and mark the unused ones
  bool bSumAssigned=false;
  QSqlQuery Query;
  Query.setForwardOnly(true);
  for (int i=0; i<lstIndex.count(); i++)
  {
    bool bAssigned;

    QModelIndex Index = lstIndex.at(i);
    QSqlRecord Record = pModel->record(Index.row());
    bAssigned=false;

    //Search if the item is in use by another dependent table
    if (!sQuerySearch1.isEmpty())
    {
      Query.exec(sQuerySearch1 + 	Record.value(0).toString());
      if (Query.next()) bAssigned=true;
    }

    //Search if the item is in use by another dependent table
    if (!sQuerySearch2.isEmpty())
    {
      Query.exec(sQuerySearch2 + 	Record.value(0).toString());
      if (Query.next()) bAssigned=true;
    }

    //Search if the item is in use by another dependent table
    if (!sQuerySearch3.isEmpty())
    {
      Query.exec(sQuerySearch3 + 	Record.value(0).toString());
      if (Query.next()) bAssigned=true;
    }

    //Mark for deletion only if this item isn't assigned to dependent tables
    if (!bAssigned)
    {
      Record.setValue("aux","x");
      pModel->setRecord(Index.row(),Record);
    }

    bSumAssigned|=bAssigned;
  }

  //Delete itmes from table and reset form
  pModel->submitAll();
  Query.exec(sQueryDel);
  pModel->select();
  cleanForm();

  //Set visible and enable controls
  ui->pPBApply->setEnabled(false);
  enableForm(false);

  //Inform the user about the actual result of the delete operation
  if (bSumAssigned && sQueryDel.contains("bank"))
    g_messageWarning(this,tr("Delete"),tr("Some items couldn't be deleted, because they are assigned in one or more accounts"));
  if (bSumAssigned && sQueryDel.contains("type"))
    g_messageWarning(this,tr("Delete"),tr("Some items couldn't be deleted, because they are assigned in one or more accounts"));
  if (bSumAssigned && sQueryDel.contains("payee"))
    g_messageWarning(this,tr("Delete"),tr("Some items couldn't be deleted, because they are assigned in one or more transactions or schedules"));
  if (bSumAssigned && sQueryDel.contains("category"))
    g_messageWarning(this,tr("Delete"),tr("Some items couldn't be deleted, because they are assigned in one or more transactions, schedules or budgets"));
  if (bSumAssigned && sQueryDel.contains("class"))
    g_messageWarning(this,tr("Delete"),tr("Some items couldn't be deleted, because they are assigned in one or more transactions, schedules or budgets"));
}

void CGeneralList::addItem1(const QString &sTable, QListView *pListView, QSqlTableModel *pModel, QLineEdit *pLineEdit)
{
  pModel->insertRow(0);
  pModel->setData(pModel->index(0,0),g_generateId(sTable));
  pModel->submitAll();

  pListView->setCurrentIndex(pModel->index(0,1));
  fillForm(pModel->index(0,1));
  pLineEdit->setFocus();

  //Set visible and enable controls
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CGeneralList::cleanForm()
{
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    ui->pLENameBank->clear();
    ui->pLEAddressBank->clear();
    ui->pLEOtherBank->clear();
    ui->pRTEBank->clear();
    ui->pListViewBank->setCurrentIndex(QModelIndex());
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    ui->pLENamePayee->clear();
    ui->pListViewPayee->setCurrentIndex(QModelIndex());
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    ui->pLENameCategory->clear();
    ui->pLEIconCategory->clear();
    ui->pListViewCategory->setCurrentIndex(QModelIndex());
    updateIcon();
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    ui->pLENameClass->clear();
    ui->pListViewClass->setCurrentIndex(QModelIndex());
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    ui->pLENameType->clear();
    ui->pListViewType->setCurrentIndex(QModelIndex());
  }
}

void CGeneralList::enableForm(bool bEnable)
{
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    ui->pLENameBank->setEnabled(bEnable);
    ui->pLEAddressBank->setEnabled(bEnable);
    ui->pLEOtherBank->setEnabled(bEnable);
    ui->pRTEBank->setEnabled(bEnable);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    ui->pLENamePayee->setEnabled(bEnable);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    ui->pLENameCategory->setEnabled(bEnable);
    ui->pLEIconCategory->setEnabled(bEnable);
    ui->pTBIconCategory->setEnabled(bEnable);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    ui->pLENameClass->setEnabled(bEnable);
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    ui->pLENameType->setEnabled(bEnable);
  }
}

void CGeneralList::showEvent( QShowEvent */*pEvent*/ )
{
  //Refresh ListViews
  g_refreshView(ui->pListViewBank,false);
  g_refreshView(ui->pListViewCategory,false);
  g_refreshView(ui->pListViewClass,false);
  g_refreshView(ui->pListViewPayee,false);
  g_refreshView(ui->pListViewType,false);

  //Set the focus and scroll position of the ListView
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
  {
    //Restore scrollbar value
    ui->pListViewBank->verticalScrollBar()->setValue(m_iScrollValue);
    ui->pListViewBank->setFocus();
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
  {
    //Restore scrollbar value
    ui->pListViewPayee->verticalScrollBar()->setValue(m_iScrollValue);
    ui->pListViewPayee->setFocus();
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
  {
    //Restore scrollbar value
    ui->pListViewCategory->verticalScrollBar()->setValue(m_iScrollValue);
    ui->pListViewCategory->setFocus();
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
  {
    //Restore scrollbar value
    ui->pListViewClass->verticalScrollBar()->setValue(m_iScrollValue);
    ui->pListViewClass->setFocus();
  }

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
  {
    //Restore scrollbar value
    ui->pListViewType->verticalScrollBar()->setValue(m_iScrollValue);
    ui->pListViewType->setFocus();
  }
}

void CGeneralList::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetBank")
    m_iScrollValue=ui->pListViewBank->verticalScrollBar()->value();

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetPayee")
    m_iScrollValue=ui->pListViewPayee->verticalScrollBar()->value();

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetCategory")
    m_iScrollValue=ui->pListViewCategory->verticalScrollBar()->value();

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetClass")
    m_iScrollValue=ui->pListViewClass->verticalScrollBar()->value();

  if(ui->pTabWidget->currentWidget()->objectName()=="pWidgetType")
    m_iScrollValue=ui->pListViewType->verticalScrollBar()->value();
}

