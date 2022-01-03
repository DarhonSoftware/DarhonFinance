#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "accountedit.h"
#include "ui_accountedit.h"
#include "dialogicons.h"
#include "global.h"
#include "mainwindow0.h"
#include "signsymbol.h"
#include "dr_profile.h"

CAccountEdit::CAccountEdit(const QVariant& Id, QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CAccountEdit)
{
  ui->setupUi(this);

  //Setup children
  ui->pLEIcon->setVisible(false);
  ui->pLEIniBalance->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pSignSymbol->setNegativeZero(false);
  ui->pSignSymbol->setSign(0);

#ifdef MOBILE_PLATFORM
  ui->pWidgetEdit->setVisible(false);
  ui->pRichTextEditor->setVisible(false);
#else
  ui->pPBList->hide();
  ui->pPBDetail->hide();
  ui->pPBCloseL->hide();
  ui->pPBMemo->hide();
#endif

  //Set members
  m_iScrollValue=0;

  //Set de validators
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  ui->pLEIniBalance->setValidator(pValidatorAmount);

  m_pModel=new QSqlTableModel(this);
  m_pModel->setTable("account");
  m_pModel->setSort(3,Qt::AscendingOrder);
  m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_pModel->select();

  ui->pListView->setModel(m_pModel);
  ui->pListView->setModelColumn(3);
  ui->pListView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

  //Initiate Type ComboBox
  m_pModelType=new QSqlTableModel(this);
  m_pModelType->setTable("type");
  m_pModelType->setSort(1,Qt::AscendingOrder);
  ui->pCBType->setModel(m_pModelType);
  ui->pCBType->setModelColumn(1);

  //Initiate Bank ComboBox
  m_pModelBank=new QSqlTableModel(this);
  m_pModelBank->setTable("bank");
  m_pModelBank->setSort(1,Qt::AscendingOrder);
  ui->pCBBank->setModel(m_pModelBank);
  ui->pCBBank->setModelColumn(1);

  //Initiate Currency ComboBox
  m_pModelCurrency=new QSqlTableModel(this);
  m_pModelCurrency->setTable("currency");
  ui->pCBCurrency->setModel(m_pModelCurrency);
  ui->pCBCurrency->setModelColumn(1);

  //Connect selection model signals
  connect(ui->pListView, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));

  //Connect controls to detect editing
  connect(ui->pLEName,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEHolder,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLENumber,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pLEIniBalance,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pSignSymbol,SIGNAL(signChanged()),this,SLOT(setDirty()));
  connect(ui->pLEIcon,SIGNAL(textChanged(QString)),this,SLOT(setDirty()));
  connect(ui->pRichTextEditor,SIGNAL(textEdited()),this,SLOT(setDirty()));
  connect(ui->pCBBank,SIGNAL(currentIndexChanged(int)),this,SLOT(setDirty()));
  connect(ui->pCBCurrency,SIGNAL(currentIndexChanged(int)),this,SLOT(setDirty()));
  connect(ui->pCBType,SIGNAL(currentIndexChanged(int)),this,SLOT(setDirty()));
  connect(ui->pCBAccStatus,SIGNAL(currentIndexChanged(int)),this,SLOT(setDirty()));
  connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
  connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));

  //Select the initial account in case Id is valid
  if (Id.isValid())
  {
    QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,0),Qt::DisplayRole,Id,1,Qt::MatchExactly);
    if (lstIndex.size()==0) return;

    QModelIndex Index=m_pModel->index(lstIndex.at(0).row(),3);

    ui->pListView->setCurrentIndex(Index);
    ui->pListView->scrollTo(Index);
    fillForm(Index);
  }
}

CAccountEdit::~CAccountEdit()
{
  delete ui;
}

void CAccountEdit::on_pPBApply_clicked()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (!Index.isValid()) return;

  //Validate data
  QString sName=ui->pLEName->text().simplified();
  if (sName.isEmpty()) return;
  QModelIndexList lstIndex=m_pModel->match(m_pModel->index(0,3),Qt::DisplayRole,sName,1,Qt::MatchExactly);
  if ((lstIndex.size()==1)&&(lstIndex.at(0).row()!=Index.row()))
  {
    g_messageWarning(this,tr("Apply"),tr("The account's name is already in use"));
    return;
  }

  //Write data into database
  QSqlRecord Record=m_pModel->record(Index.row());
  Record.setValue(1,ui->pLEIcon->text());
  Record.setValue(2,m_pModelType->record(ui->pCBType->currentIndex()).value(0));
  Record.setValue(3,sName);
  Record.setValue(4,m_pModelBank->record(ui->pCBBank->currentIndex()).value(0));
  Record.setValue(5,m_pModelCurrency->record(ui->pCBCurrency->currentIndex()).value(0));
  Record.setValue(6,ui->pLENumber->text().simplified());
  Record.setValue(7,ui->pLEHolder->text().simplified());
  Record.setValue(8,ui->pLEIniBalance->text().toDouble()*ui->pSignSymbol->sign());
  Record.setValue(9,ui->pRichTextEditor->toHtml());
  Record.setValue(11,ui->pCBAccStatus->currentIndex());
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

void CAccountEdit::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBClose->setVisible(!bChecked);
  ui->pPBList->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pRichTextEditor->setVisible(bChecked);
}

void CAccountEdit::on_pPBList_clicked()
{
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
}

void CAccountEdit::on_pPBDetail_clicked()
{
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
}

void CAccountEdit::on_pPBClose_clicked()
{
  close();
}

void CAccountEdit::on_pPBCloseL_clicked()
{
  close();
}

void CAccountEdit::on_pTBIcon_clicked()
{
  CDialogIcons Dialog(this);
  int iResult=Dialog.exec();
  if (iResult==QDialog::Rejected) return;

  QString s=Dialog.icon();
  ui->pLEIcon->setText(s);
  updateIcon();
}

void CAccountEdit::updateIcon()
{
  QString sIcon=ui->pLEIcon->text();
  if (sIcon.isEmpty())
  {
    QIcon Icon(":/others/icons/drfinance32.png");
    ui->pTBIcon->setIcon(Icon);
  }
  else
  {
    QIcon Icon(sIcon);
    ui->pTBIcon->setIcon(Icon);
  }
}

void CAccountEdit::addItem()
{
  if (!isVisible()) return;

  int iId=g_generateId("account");
  m_pModel->insertRow(0);
  m_pModel->setData(m_pModel->index(0,0),iId);
  m_pModel->setData(m_pModel->index(0,2),0);
  m_pModel->setData(m_pModel->index(0,4),0);
  m_pModel->setData(m_pModel->index(0,5),0);
  m_pModel->setData(m_pModel->index(0,11),1);
  m_pModel->submitAll();

  ui->pListView->setCurrentIndex(m_pModel->index(0,3));
  fillForm(m_pModel->index(0,3));
  ui->pLEName->setFocus();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CAccountEdit::deleteItem()
{
  if (!isVisible()) return;

  ui->pListView->setFocus();

  //Obtain the selected account
  QModelIndexList lstIndex=ui->pListView->selectionModel()->selectedIndexes();
  if (lstIndex.count()!=1) return;
  QModelIndex Index = lstIndex.at(0);

  //Confirm deletion
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete selected account?"))==QMessageBox::No) return;

  //Check if account is being used in dependent tables
  bool bAssigned=false;
  QSqlQuery Query;
  QSqlRecord Record = m_pModel->record(Index.row());
  Query.setForwardOnly(true);

  //Search if the item is in use by trans table
  Query.exec("SELECT idx FROM trans WHERE accountid=" + 	Record.value(0).toString());
  if (Query.next()) bAssigned=true;

  //Leave in case the item is used in trans table
  if (bAssigned)
  {
    g_messageWarning(this,tr("Delete"),tr("The account couldn't be deleted, because it has existing transactions"));
    return;
  }

  //Search if the item is in use by schedule table
  Query.exec("SELECT idx FROM schedule WHERE accountid=" + 	Record.value(0).toString());
  if (Query.next()) bAssigned=true;

  //Leave in case the item is used in schedule table
  if (bAssigned)
  {
    g_messageWarning(this,tr("Delete"),tr("The account couldn't be deleted, because it has existing schedules"));
    return;
  }

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

void CAccountEdit::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  //Select Comboboxes
  g_refreshCombobox(ui->pCBType,m_pModel->record(Index.row()).value(2),false);
  g_refreshCombobox(ui->pCBBank,m_pModel->record(Index.row()).value(4),false);
  g_refreshCombobox(ui->pCBCurrency,m_pModel->record(Index.row()).value(5),false);
  g_initComboboxAccStatus(ui->pCBAccStatus,m_pModel->record(Index.row()).value(11).toInt());

  QString s=m_pModel->record(Index.row()).value(1).toString();
  ui->pLEIcon->setText(s);
  ui->pLEName->setText(m_pModel->record(Index.row()).value(3).toString());
  ui->pLENumber->setText(m_pModel->record(Index.row()).value(6).toString());
  ui->pLEHolder->setText(m_pModel->record(Index.row()).value(7).toString());
  ui->pLEIniBalance->setText(QString::number(qAbs(m_pModel->record(Index.row()).value(8).toDouble()),'f',2));
  ui->pSignSymbol->setSign(m_pModel->record(Index.row()).value(8).toDouble());
  ui->pRichTextEditor->setHtml(m_pModel->record(Index.row()).value(9).toString());
  updateIcon();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CAccountEdit::setDirty()
{
  QModelIndex Index=ui->pListView->currentIndex();
  if (Index.isValid())
    ui->pPBApply->setEnabled(true);
}

void CAccountEdit::cleanForm()
{
  ui->pLEName->clear();
  ui->pCBBank->setCurrentIndex(0);
  ui->pCBType->setCurrentIndex(0);
  ui->pCBCurrency->setCurrentIndex(0);
  ui->pCBAccStatus->setCurrentIndex(1);
  ui->pLEHolder->clear();
  ui->pLENumber->clear();
  ui->pLEIniBalance->clear();
  ui->pSignSymbol->setSign(0);
  ui->pLEIcon->clear();
  ui->pRichTextEditor->clear();
  ui->pListView->setCurrentIndex(QModelIndex());
  updateIcon();
}

void CAccountEdit::enableForm(bool bEnable)
{
  ui->pLEName->setEnabled(bEnable);
  ui->pCBBank->setEnabled(bEnable);
  ui->pCBType->setEnabled(bEnable);
  ui->pCBCurrency->setEnabled(bEnable);
  ui->pCBAccStatus->setEnabled(bEnable);
  ui->pLEHolder->setEnabled(bEnable);
  ui->pLENumber->setEnabled(bEnable);
  ui->pLEIniBalance->setEnabled(bEnable);
  ui->pSignSymbol->setEnabled(bEnable);
  ui->pRichTextEditor->setEnabled(bEnable);
  ui->pTBIcon->setEnabled(bEnable);
}

void CAccountEdit::showEvent( QShowEvent */*pEvent*/ )
{
  //Refresh ListView
  g_refreshView(ui->pListView,false);

  //Refresh Comboboxes
  g_refreshCombobox(ui->pCBType,QVariant(),false);
  g_refreshCombobox(ui->pCBBank,QVariant(),false);
  g_refreshCombobox(ui->pCBCurrency,QVariant(),false);

  //Restore scrollbar value
  ui->pListView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the ListView
  ui->pListView->setFocus();
}

void CAccountEdit::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  m_iScrollValue=ui->pListView->verticalScrollBar()->value();
}
