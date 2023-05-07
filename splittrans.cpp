#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "splittrans.h"
#include "basedelegate.h"
#include "ui_splittrans.h"
#include "global.h"
#include "mainwindow0.h"
#include "exchange.h"
#include "dr_profile.h"

CSplitTrans::CSplitTrans(QStandardItemModel *pModel, const QVariant& IdAcc, double dAmountTrans, bool bRateEnable, bool bReadOnly, QWidget *pWidget) :
    CFormBase(pWidget),
    ui(new Ui::CSplitTrans)
{
  ui->setupUi(this);

  //Setup children
  ui->pLEAmount->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pLERate->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
  ui->pSignSymbol->setSign(0);
#ifdef MOBILE_PLATFORM
  ui->pRichTextEditor->setVisible(false);
  ui->pWidgetEdit->setVisible(false);
#else
  ui->pPBList->hide();
  ui->pPBDetail->hide();
  ui->pPBCloseL->hide();
  ui->pPBMemo->hide();
#endif

  //Set member variables
  m_iScrollValue=0;
  m_pModel=pModel;
  m_IdAcc=IdAcc;
  m_dAmountTrans=dAmountTrans;
  m_bRateEnable=bRateEnable;

  //Initiate Class ComboBox
  m_pModelClass=new QSqlTableModel(this);
  m_pModelClass->setTable("class");
  m_pModelClass->setSort(1,Qt::AscendingOrder);
  ui->pCBClass->setModel(m_pModelClass);
  ui->pCBClass->setModelColumn(1);

  //Iniciate model
  m_pModel->setHorizontalHeaderItem(0,new QStandardItem(tr("Category")));
  m_pModel->setHorizontalHeaderItem(1,new QStandardItem(tr("Class")));
  m_pModel->setHorizontalHeaderItem(2,new QStandardItem(tr("Amount")));

  //Set de validators
  QDoubleValidator *pValidatorAmount=new QDoubleValidator(0,MAX_AMOUNT,2,this);
  pValidatorAmount->setNotation(QDoubleValidator::StandardNotation);
  ui->pLEAmount->setValidator(pValidatorAmount);
  QDoubleValidator *pValidatorRate=new QDoubleValidator(0,MAX_AMOUNT,RATE_PREC,this);
  pValidatorRate->setNotation(QDoubleValidator::StandardNotation);
  ui->pLERate->setValidator(pValidatorRate);

  //Set Delegate features
  CBaseDelegate* pDelegate=new CBaseDelegate(ui->pTableView);
  pDelegate->setNumberPrecision(2);
  pDelegate->updateCategoryData();
  pDelegate->updateClassData();
  pDelegate->addColumnFormat(0,CBaseDelegate::FormatCategory);
  pDelegate->addColumnFormat(1,CBaseDelegate::FormatClass);
  pDelegate->addColumnFormat(2,CBaseDelegate::FormatNumber);

  //Set TableView features
  ui->pTableView->setModel(m_pModel);
  ui->pTableView->setItemDelegate(pDelegate);
  ui->pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->pTableView->setGridStyle(Qt::NoPen);
  ui->pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->pTableView->setColumnHidden(3,true);
  ui->pTableView->setColumnHidden(4,true);

  //Set HeaderView features
  QHeaderView * pHeader=ui->pTableView->horizontalHeader();
  pHeader->setSectionResizeMode(QHeaderView::Stretch);

  //Inicitate controls
  ui->pLTotalTrans->setText(QString::number(m_dAmountTrans,'f',2));
  updateTotals();

  //Connect signals
  connect(ui->pTableView, SIGNAL(activated(QModelIndex)),this, SLOT(fillForm(QModelIndex)));
  connect(ui->pLEAmount,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pSignSymbol,SIGNAL(signChanged()),this,SLOT(setDirty()));
  connect(ui->pLERate,SIGNAL(textEdited(QString)),this,SLOT(setDirty()));
  connect(ui->pCBCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pCBClass, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
  connect(ui->pRichTextEditor,SIGNAL(textEdited()),this,SLOT(setDirty()));
  connect(m_pModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(updateTotals()));
  connect(m_pModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(updateTotals()));

  //Set ReadOnly mode
  if (bReadOnly)
  {
    ui->pRichTextEditor->setEnabled(false);
    ui->pWidgetControls->setEnabled(false);
    ui->pPBApply->hide();
  }
  else
  {
    connect(g_pMainWindow,SIGNAL(clickAdd()),this,SLOT(addItem()));
    connect(g_pMainWindow,SIGNAL(clickDelete()),this,SLOT(deleteItem()));
  }
}

CSplitTrans::~CSplitTrans()
{
  delete ui;
}

void CSplitTrans::on_pPBApply_clicked()
{
  QModelIndex Index=ui->pTableView->currentIndex();
  if (!Index.isValid()) return;
  if ((ui->pLERate->isEnabled())&&(ui->pLERate->text().toDouble()==0))
  {
    g_messageWarning(this,tr("Apply"),tr("The rate cannot be zero"));
    return;
  }

  //Get Id from category combobox
  m_pModel->item(Index.row(),0)->setText(ui->pCBCategory->itemData(ui->pCBCategory->currentIndex()).toString());
  m_pModel->item(Index.row(),1)->setText(m_pModelClass->record(ui->pCBClass->currentIndex()).value(0).toString());
  m_pModel->item(Index.row(),2)->setText(QString::number(ui->pLEAmount->text().toDouble()*ui->pSignSymbol->sign(),'f',2));
  m_pModel->item(Index.row(),3)->setText(ui->pRichTextEditor->toHtml());
  if (ui->pLERate->text().isEmpty())
  {
    m_pModel->item(Index.row(),4)->setText("");
  }
  else
  {
    m_pModel->item(Index.row(),4)->setText(QString::number(ui->pLERate->text().toDouble(),'f',RATE_PREC));
  }
  cleanForm();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(false);
}

void CSplitTrans::on_pPBMemo_toggled(bool bChecked)
{
  ui->pPBApply->setVisible(!bChecked);
  ui->pPBClose->setVisible(!bChecked);
  ui->pPBList->setVisible(!bChecked);
  ui->pWidgetControls->setVisible(!bChecked);
  ui->pRichTextEditor->setVisible(bChecked);
}

void CSplitTrans::on_pPBList_clicked()
{
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
}

void CSplitTrans::on_pPBDetail_clicked()
{
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
}

void CSplitTrans::on_pPBClose_clicked()
{
  close();
}

void CSplitTrans::on_pPBCloseL_clicked()
{
  close();
}

void CSplitTrans::on_pPBCalc_clicked()
{
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+m_IdAcc.toString());
  ModelAcc.select();

  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+ModelAcc.record(0).value(5).toString());
  ModelCurr.select();

  double dRate=ModelCurr.record(0).value(2).toDouble();
  QString sCurrency=ModelCurr.record(0).value(1).toString();

  CExchange Exchange(dRate,sCurrency,this);
  if (Exchange.exec()==QDialog::Accepted)
  {
    double dAmount=Exchange.calculatedAmount();
    ui->pLEAmount->setText(QString::number(dAmount,'f',2));
  }
}

void CSplitTrans::addItem()
{
  if (!isVisible()) return;

  int iRow=m_pModel->rowCount();

  QList<QStandardItem*> lstItems;
  lstItems<<new QStandardItem("0");
  lstItems<<new QStandardItem("0");
  lstItems<<new QStandardItem("0");
  lstItems<<new QStandardItem("");
  lstItems<<new QStandardItem("");

  m_pModel->appendRow(lstItems);
  ui->pTableView->setColumnHidden(3,true);
  ui->pTableView->setColumnHidden(4,true);

  ui->pTableView->setCurrentIndex(m_pModel->index(iRow,0));
  fillForm(m_pModel->index(iRow,1));
  ui->pLEAmount->setFocus();
  ui->pLEAmount->selectAll();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);
}

void CSplitTrans::deleteItem()
{
  if (!isVisible()) return;

  QModelIndexList lstIndex=ui->pTableView->selectionModel()->selectedRows();
  if (lstIndex.count()!=1) return;
  if (g_messageYesNo(this,tr("Confirmation"),tr("Delete selected transaction?"))==QMessageBox::No) return;

  ui->pTableView->setFocus();

  QModelIndex Index = lstIndex.at(0);
  m_pModel->removeRow(Index.row());
  cleanForm();

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(true);
  ui->pWidgetEdit->setVisible(false);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(false);
}

void CSplitTrans::on_pCBCategory_activated(int iIndex)
{
  if (!m_bRateEnable) return;

  int iCat=ui->pCBCategory->itemData(iIndex).toInt();
  if ((iCat<0)&&(iCat>=-ID_MAX))
  {
    bool bDiff;
    double dRate=g_transferRate(m_IdAcc.toInt(),iCat*-1,&bDiff);
    ui->pLERate->setText(QString::number(dRate,'f',RATE_PREC));
    if (bDiff)
      ui->pLERate->setEnabled(true);
    else
      ui->pLERate->setEnabled(false);
  }
  else
  {
    ui->pLERate->clear();
    ui->pLERate->setEnabled(false);
  }
}

void CSplitTrans::fillForm(QModelIndex Index)
{
  if (!Index.isValid()) return;

  ui->pLEAmount->setText(QString::number(qAbs(m_pModel->item(Index.row(),2)->text().toDouble()),'f',2));
  ui->pSignSymbol->setSign(m_pModel->item(Index.row(),2)->text().toDouble());
  ui->pRichTextEditor->setHtml(m_pModel->item(Index.row(),3)->text());
  ui->pLERate->setText(m_pModel->item(Index.row(),4)->text());

  int iCat=m_pModel->item(Index.row(),0)->text().toInt();
  g_refreshComboCategory(ui->pCBCategory,iCat,m_IdAcc,false);
  int iId=ui->pCBCategory->findData(ID_SPLIT);
  ui->pCBCategory->removeItem(iId);

  int iClass=m_pModel->item(Index.row(),1)->text().toInt();
  g_refreshCombobox(ui->pCBClass,iClass,false);

  //Set visible and enable controls
#ifdef MOBILE_PLATFORM
  ui->pWidgetList->setVisible(false);
  ui->pWidgetEdit->setVisible(true);
#endif
  ui->pPBApply->setEnabled(false);
  enableForm(true);

  if (!m_bRateEnable) return;

  //Set Enable state for Rate
  if ((iCat<0)&&(iCat>=-ID_MAX))
  {
    bool bDiff;
    g_transferRate(m_IdAcc.toInt(),iCat*-1,&bDiff);
    if (bDiff)
      ui->pLERate->setEnabled(true);
    else
      ui->pLERate->setEnabled(false);
  }
  else
  {
    ui->pLERate->setEnabled(false);
  }
}

void CSplitTrans::setDirty()
{
  QModelIndex Index=ui->pTableView->currentIndex();
  if (Index.isValid())
    ui->pPBApply->setEnabled(true);
}

void CSplitTrans::updateTotals()
{
  double dTotal=0.0;
  for (int iRow=0;iRow<m_pModel->rowCount();iRow++)
    dTotal+=m_pModel->item(iRow,2)->text().toDouble();

  //Write Total Split
  ui->pLTotalSplit->setText(QString::number(dTotal,'f',2));

  //Write Amount Trans
  QString s=QString::number(m_dAmountTrans-dTotal,'f',2);
  if (s=="-0.00")
    ui->pLTotalOff->setText("0.00");
  else
    ui->pLTotalOff->setText(QString::number(m_dAmountTrans-dTotal,'f',2));
}

void CSplitTrans::cleanForm()
{
  ui->pLEAmount->clear();
  ui->pSignSymbol->setSign(0);
  ui->pLERate->clear();
  ui->pRichTextEditor->clear();
  ui->pCBCategory->setCurrentIndex(0);
  ui->pCBClass->setCurrentIndex(0);
  ui->pTableView->setCurrentIndex(QModelIndex());
}

void CSplitTrans::enableForm(bool bEnable)
{
  ui->pLEAmount->setEnabled(bEnable);
  ui->pSignSymbol->setEnabled(bEnable);
  ui->pPBCalc->setEnabled(bEnable);
  ui->pRichTextEditor->setEnabled(bEnable);
  ui->pCBCategory->setEnabled(bEnable);
  ui->pCBClass->setEnabled(bEnable);
  ui->pLERate->setEnabled(false);
}

void CSplitTrans::showEvent( QShowEvent */*pEvent*/ )
{
  //Update Symbol
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+m_IdAcc.toString());
  ModelAcc.select();

  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+ModelAcc.record(0).value(5).toString());
  ModelCurr.select();
  ui->pLSymbol->setText(ModelCurr.record(0).value(3).toString());

  //Update combobox Category
  g_refreshComboCategory(ui->pCBCategory,QVariant(),m_IdAcc,false);
  int iId=ui->pCBCategory->findData(ID_SPLIT);
  ui->pCBCategory->removeItem(iId);

  //Update combobox Class
  g_refreshCombobox(ui->pCBClass,QVariant(),false);

  //Review content of TableView in case there is an unexistent category
  for (int iRow=0;iRow<m_pModel->rowCount();iRow++)
  {
    bool bResult;

    int iCat=m_pModel->item(iRow,0)->text().toInt();
    if ((iCat<0)&&(iCat>=-ID_MAX))
      bResult=g_findRecordFromTable("account","idx",QString::number(iCat*-1));
    else
      bResult=g_findRecordFromTable("category","idx",QString::number(iCat));
    if (!bResult) m_pModel->item(iRow,0)->setText("0");
  }

  //Review content of TableView in case there is an unexistent class
  for (int iRow=0;iRow<m_pModel->rowCount();iRow++)
  {
    if (!g_findRecordFromTable("class","idx",m_pModel->item(iRow,1)->text())) m_pModel->item(iRow,1)->setText("0");
  }

  //Update Delegate for Category
  CBaseDelegate* pDelegate=dynamic_cast<CBaseDelegate*>(ui->pTableView->itemDelegate());
  pDelegate->updateCategoryData();
  pDelegate->updateClassData();

  //Restore scrollbar value
  ui->pTableView->verticalScrollBar()->setValue(m_iScrollValue);

  //Set the focus o the TableView and resize columns
  ui->pTableView->setFocus();
}

void CSplitTrans::hideEvent (QHideEvent */*pEvent*/)
{
  //Save scrollbar value
  m_iScrollValue=ui->pTableView->verticalScrollBar()->value();
}
