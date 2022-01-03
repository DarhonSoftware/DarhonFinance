#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "groupedit.h"
#include "ui_groupedit.h"
#include "global.h"
#include "mainwindow0.h"
#include "dr_profile.h"

CGroupEdit::CGroupEdit(const QVariant& Id, QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CGroupEdit)
{
  ui->setupUi(this);

  //Setup children
#ifdef MOBILE_PLATFORM
  ui->pWidgetEdit->setVisible(false);
#else
  ui->pPBList->hide();
  ui->pPBDetail->hide();
  ui->pPBCloseL->hide();
#endif

  //Set members
  m_iScrollValue=0;

  //Initiate Model
  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("sbgroup");
  m_pModel->setSort(1,Qt::AscendingOrder);
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_pModel->select();

  ui->pListView->setModel(m_pModel);
  ui->pListView->setModelColumn(1);
  ui->pListView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Connect selection model signals
  connect(ui->pListView, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));

  //Connect controls to detect editing
  connect(ui->pLEName,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField1,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField2,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField3,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField4,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField5,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField6,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField7,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEField8,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));

  //Select the initial group in case Id is valid
  if (Id.isValid())
  {
    QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,0),Qt::DisplayRole,Id,1,Qt::MatchExactly);
    if (lstIndex.size()==0) return;

    QModelIndex Index=m_pModel->index(lstIndex.at(0).row(),1);

    ui->pListView->setCurrentIndex(Index);
    ui->pListView->scrollTo(Index);
    fillForm(Index);
  }
}

CGroupEdit::~CGroupEdit()
{
  delete ui;
}

void CGroupEdit::on_pPBApply_clicked()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (!Index.isValid()) return;

  //Validate data name
  QString sName=ui->pLEName->text().simplified();
  if (sName.isEmpty()) return;
  QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
  if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
  {
    g_messageWarning(this,tr("Apply"),tr("The group's name is already in use."));
    return;
  }

  //Validate data field1
  QString sField1=ui->pLEField1->text().simplified();
  if (sField1.isEmpty())
  {
    g_messageWarning(this,tr("Apply"),tr("At least Field1 needs to be filled."));
    return;
  }

  //Write data into database
  QSqlRecord Record=m_pModel->record(Index.row());
  Record.setValue(1,sName);
  Record.setValue(2,ui->pLEField1->text().simplified());
  Record.setValue(3,ui->pLEField2->text().simplified());
  Record.setValue(4,ui->pLEField3->text().simplified());
  Record.setValue(5,ui->pLEField4->text().simplified());
  Record.setValue(6,ui->pLEField5->text().simplified());
  Record.setValue(7,ui->pLEField6->text().simplified());
  Record.setValue(8,ui->pLEField7->text().simplified());
  Record.setValue(9,ui->pLEField8->text().simplified());
  m_pModel->setRecord(Index.row(),Record);
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

void CGroupEdit::on_pPBList_clicked()
{
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
}

void CGroupEdit::on_pPBDetail_clicked()
{
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
}

void CGroupEdit::on_pPBClose_clicked()
{
  close();
}

void CGroupEdit::on_pPBCloseL_clicked()
{
  close();
}

void CGroupEdit::addItem()
{
  if (!isVisible()) return;

  m_pModel->insertRow(0);
  m_pModel->setData(m_pModel->index(0,0),g_generateId("sbgroup"));
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

void CGroupEdit::deleteItem()
{
  if (!isVisible()) return;

  ui->pListView->setFocus();

  //Obtain the selected group
  QModelIndexList lstIndex=ui->pListView->selectionModel()->selectedIndexes();
  if (lstIndex.count()!=1) return;
  QModelIndex Index = lstIndex.at(0);

  //Confirm deletion
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete selected group?"))==QMessageBox::No) return;

  //Check if group is being used in dependent tables
  bool bAssigned=false;
  QSqlQuery Query;
  QSqlRecord Record = m_pModel->record(Index.row());
  Query.setForwardOnly(true);

  //Search if the item is in use by sbitem table
  Query.exec("SELECT idx FROM sbitem WHERE groupid=" + 	Record.value(0).toString());
  if (Query.next()) bAssigned=true;

  //Leave in case the item is used in sbitem table
  if (bAssigned)
  {
    g_messageWarning(this,tr("Delete"),tr("The group couldn't be deleted, because it has existing items"));
    return;
  }

  //Delete group from table and reset form
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

void CGroupEdit::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  //Select Comboboxes
  ui->pLEName->setText(m_pModel->record(Index.row()).value(1).toString());
  ui->pLEField1->setText(m_pModel->record(Index.row()).value(2).toString());
  ui->pLEField2->setText(m_pModel->record(Index.row()).value(3).toString());
  ui->pLEField3->setText(m_pModel->record(Index.row()).value(4).toString());
  ui->pLEField4->setText(m_pModel->record(Index.row()).value(5).toString());
  ui->pLEField5->setText(m_pModel->record(Index.row()).value(6).toString());
  ui->pLEField6->setText(m_pModel->record(Index.row()).value(7).toString());
  ui->pLEField7->setText(m_pModel->record(Index.row()).value(8).toString());
  ui->pLEField8->setText(m_pModel->record(Index.row()).value(9).toString());

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CGroupEdit::cleanForm()
{
  ui->pLEName->clear();
  ui->pLEField1->clear();
  ui->pLEField2->clear();
  ui->pLEField3->clear();
  ui->pLEField4->clear();
  ui->pLEField5->clear();
  ui->pLEField6->clear();
  ui->pLEField7->clear();
  ui->pLEField8->clear();
  ui->pListView->setCurrentIndex(QModelIndex());
}

void CGroupEdit::enableForm(bool bEnable)
{
  ui->pLEName->setEnabled(bEnable);
  ui->pLEField1->setEnabled(bEnable);
  ui->pLEField2->setEnabled(bEnable);
  ui->pLEField3->setEnabled(bEnable);
  ui->pLEField4->setEnabled(bEnable);
  ui->pLEField5->setEnabled(bEnable);
  ui->pLEField6->setEnabled(bEnable);
  ui->pLEField7->setEnabled(bEnable);
  ui->pLEField8->setEnabled(bEnable);
}

void CGroupEdit::showEvent( QShowEvent */*pEvent*/ )
{
  //Refresh ListView
  g_refreshView(ui->pListView,false);

  //Restore scrollbar value
  ui->pListView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the ListView
  ui->pListView->setFocus();
}

void CGroupEdit::hideEvent ( QHideEvent */*pEvent*/ )
{
  //Save scrollbar value
  m_iScrollValue=ui->pListView->verticalScrollBar()->value();
}

void CGroupEdit::setDirty()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (Index.isValid())
    ui->pPBApply->setEnabled(true);
}
