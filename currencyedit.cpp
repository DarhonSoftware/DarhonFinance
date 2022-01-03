#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "currencyedit.h"
#include "ui_currencyedit.h"
#include "global.h"
#include "mainwindow0.h"
#include "dr_profile.h"

CCurrencyEdit::CCurrencyEdit(QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CCurrencyEdit)
{
  ui->setupUi(this);

  //Setup children
  ui->pLERate->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
#ifdef MOBILE_PLATFORM
  ui->pWidgetEdit->setVisible(false);
#else
  ui->pPBList->hide();
  ui->pPBDetail->hide();
  ui->pPBCloseL->hide();
#endif

  //Set members
  m_iScrollValue=0;

  //Set de validators
  QDoubleValidator *pValidatorRate=new QDoubleValidator(0,MAX_AMOUNT,RATE_PREC,this);
  pValidatorRate->setNotation(QDoubleValidator::StandardNotation);
  ui->pLERate->setValidator(pValidatorRate);

  //Set Model
  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("currency");
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

  //Set ListView
  ui->pListView->setModel(m_pModel);
  ui->pListView->setModelColumn(1);
  ui->pListView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Connect signals
  connect(ui->pListView, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));
  connect(ui->pLEName,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLERate,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLESymbol,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
}

CCurrencyEdit::~CCurrencyEdit()
{
  delete ui;
}

void CCurrencyEdit::on_pPBApply_clicked()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (!Index.isValid()) return;

  //Validate data
  if (ui->pLERate->text().toDouble()==0)
  {
    g_messageWarning(this,tr("Apply"),tr("The rate cannot be zero"));
    return;
  }

  QString sName=ui->pLEName->text().simplified();
  if (sName.isEmpty()) return;
  QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,1),Qt::DisplayRole,sName,1,Qt::MatchExactly);
  if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
  {
    g_messageWarning(this,tr("Apply"),tr("The currency's name is already in use"));
    return;
  }

  //Write data into database
  QSqlRecord Record=m_pModel->record(Index.row());
  Record.setValue(1,sName);
  Record.setValue(2,ui->pLERate->text().toDouble());
  Record.setValue(3,ui->pLESymbol->text().simplified());
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

void CCurrencyEdit::on_pPBList_clicked()
{
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
}

void CCurrencyEdit::on_pPBDetail_clicked()
{
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
}

void CCurrencyEdit::on_pPBClose_clicked()
{
  close();
}

void CCurrencyEdit::on_pPBCloseL_clicked()
{
  close();
}

void CCurrencyEdit::addItem()
{
  if (!isVisible()) return;

  int iRow=g_rowCount(*m_pModel);
  m_pModel->insertRow(iRow);
  m_pModel->setData(m_pModel->index(iRow,0),g_generateId("currency"));
  m_pModel->setData(m_pModel->index(iRow,2),1);
  m_pModel->submitAll();

  ui->pListView->setCurrentIndex(m_pModel->index(iRow,1));
  fillForm(m_pModel->index(iRow,1));
  ui->pLEName->setFocus();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CCurrencyEdit::deleteItem()
{
  if (!isVisible()) return;

  ui->pListView->setFocus();

  //Obtain the selected currency
  QModelIndexList lstIndex=ui->pListView->selectionModel()->selectedIndexes();
  if (lstIndex.count()!=1) return;
  QModelIndex Index = lstIndex.at(0);

  //Check if currency is base
  if (Index.row()==0) return;

  //Confirm deletion
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete selected currency?"))==QMessageBox::No) return;

  //Check if currency is being used in dependent tables
  bool bAssigned=false;
  QSqlQuery Query;
  QSqlRecord Record = m_pModel->record(Index.row());
  Query.setForwardOnly(true);

  //Search if the item is in use by account table
  Query.exec("SELECT idx FROM account WHERE currencyid=" + 	Record.value(0).toString());
  if (Query.next()) bAssigned=true;

  //Leave in case the item is used in account table
  if (bAssigned)
  {
    g_messageWarning(this,tr("Delete"),tr("The currency couldn't be deleted, because it is assigned in one or more accounts"));
    return;
  }

  //Search if the item is in use by budget table
  Query.exec("SELECT idx FROM budget WHERE currencyid=" + 	Record.value(0).toString());
  if (Query.next()) bAssigned=true;

  //Leave in case the item is used in budget table
  if (bAssigned)
  {
    g_messageWarning(this,tr("Delete"),tr("The currency couldn't be deleted, because it is assigned in one or more budgets"));
    return;
  }

  //Delete currency from table and reset form
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

void CCurrencyEdit::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  ui->pLEName->setText(m_pModel->record(Index.row()).value(1).toString());
  ui->pLERate->setText(QString::number(m_pModel->record(Index.row()).value(2).toDouble(),'f',RATE_PREC));
  ui->pLESymbol->setText(m_pModel->record(Index.row()).value(3).toString());

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);

  if (Index.row()==0)
    ui->pLERate->setEnabled(false);
  else
    ui->pLERate->setEnabled(true);
}

void CCurrencyEdit::setDirty()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (Index.isValid())
    ui->pPBApply->setEnabled(true);
}

void CCurrencyEdit::cleanForm()
{
  ui->pLEName->clear();
  ui->pLESymbol->clear();
  ui->pLERate->clear();
  ui->pListView->setCurrentIndex(QModelIndex());
}

void CCurrencyEdit::enableForm(bool bEnable)
{
  ui->pLEName->setEnabled(bEnable);
  ui->pLESymbol->setEnabled(bEnable);
  ui->pLERate->setEnabled(bEnable);
}

void CCurrencyEdit::showEvent( QShowEvent */*pEvent*/ )
{
  //Refresh ListView
  g_refreshView(ui->pListView,false);

  //Restore scrollbar value
  ui->pListView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the TableView
  ui->pListView->setFocus();
}

void CCurrencyEdit::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  m_iScrollValue=ui->pListView->verticalScrollBar()->value();
}

