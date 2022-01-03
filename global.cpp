#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include "global.h"


int g_generateId(const QString &sTable)
{
  QSqlQuery Query;
  Query.setForwardOnly(true);
  Query.exec("SELECT MAX(idx) FROM " + sTable);
  int id = 0;
  if (Query.next())
  id = Query.value(0).toInt() + 1;
  if (id>ID_MAX) id=ID_MAX;
  return id;
}

void g_transfer(const QVariant& Id1)
{
  //Set Record action: 0 - Do nothing, 1 - Update, 2 - Delete
  int iRecord1=0;
  int iRecord2=0;

  //Set Model for Record1
  QSqlTableModel ModelTrans1;
  ModelTrans1.setTable("trans");
  ModelTrans1.setFilter("idx="+Id1.toString());
  ModelTrans1.select();
  QSqlRecord Record1=ModelTrans1.record(0);
  int iTransfer=Record1.value(12).toInt();
  int iCat=Record1.value(7).toInt();
  int iAcc=Record1.value(6).toInt();
  double dRate=Record1.value(13).toDouble();

  //Set Model for Record2
  QSqlTableModel ModelTrans2;
  ModelTrans2.setTable("trans");
  ModelTrans2.select();
  QSqlRecord Record2;

  //If Category chosen is an Account to be transfered
  if ((iCat<0)&&(iCat>=-ID_MAX))
  {
    //If Transfer already exist, get the record for the pair
    if (iTransfer>0)
    {
      ModelTrans2.setFilter("transfer="+QString::number(iTransfer)+" and "+"idx!="+Id1.toString());
      Record2=ModelTrans2.record(0);
    }
    //If Transfer did't exist, create the record for the pair and set "transfer" field
    else
    {
      int iId=g_generateId("trans");
      ModelTrans2.insertRow(0);
      ModelTrans2.setData(ModelTrans2.index(0,0),iId);
      ModelTrans2.submitAll();
      ModelTrans2.setFilter("idx="+QString::number(iId));

      Record2=ModelTrans2.record(0);

      QSqlQuery Query;
      Query.setForwardOnly(true);
      Query.exec("SELECT MAX(transfer) FROM trans");
      if (Query.next())
      iTransfer = Query.value(0).toInt() + 1;
      if (iTransfer>ID_MAX) iTransfer=ID_MAX;

      Record1.setValue(12,iTransfer);
      Record2.setValue(12,iTransfer);
      iRecord1=1;
    }

    //Fill out pair record
    Record2.setValue(1,Record1.value(1));
    Record2.setValue(2,0);
    Record2.setValue(3,Record1.value(3));
    Record2.setValue(4,Record1.value(4));
    Record2.setValue(5,Record1.value(5));
    Record2.setValue(6,iCat*-1);
    Record2.setValue(7,iAcc*-1);
    Record2.setValue(8,Record1.value(8));
    Record2.setValue(9,QString::number(Record1.value(9).toDouble()*-dRate,'f',2).toDouble());
    Record2.setValue(10,Record1.value(10));
    Record2.setValue(13,QString::number(1.0/Record1.value(13).toDouble(),'f',RATE_PREC));
    if (Record1.value(14).toInt()>0)
      Record2.setValue(14,Record1.value(14).toInt()-SPLIT);
    else
      Record2.setValue(14,0);
    iRecord2=1;
  }
  //If Category chosen isn't an Account to be transfered
  else
  {
    //If Record1 was a transfer transaction before, then delete the pair
    if (iTransfer>0)
    {
      ModelTrans2.setFilter("transfer="+QString::number(iTransfer)+" and "+"idx!="+Id1.toString());
      Record2=ModelTrans2.record(0);

      Record1.setValue(12,0);
      iRecord1=1;
      iRecord2=2;
    }
  }

  if (iRecord1==1)
  {
    ModelTrans1.setRecord(0,Record1);
    ModelTrans1.submitAll();
  }
  if (iRecord2==1)
  {
    ModelTrans2.setRecord(0,Record2);
    ModelTrans2.submitAll();
  }
  if (iRecord2==2)
  {
    ModelTrans2.removeRow(0);
    ModelTrans2.submitAll();
  }
}

void g_refreshCombobox(QComboBox *pCB, const QVariant& Id1, bool bAny)
{
  //Block possible signals to avoid to be triggered while refreshing
  pCB->blockSignals(true);

  //Cast the model into SqlTable for convenience
  QSqlTableModel *pModel=qobject_cast<QSqlTableModel*>(pCB->model());

  //Save the text content for an editable combobox
  QString sText;
  if (pCB->isEditable()) sText=pCB->lineEdit()->text();

  //Get Id if element is selected and not valid Variant in case nothing is selected
  QVariant Id;
  int iIdx=pCB->currentIndex();
  if (Id1.isValid())
    Id=Id1;
  else if (iIdx!=-1)
    Id=pModel->record(iIdx).value(0);

  //Refresh Model
  pModel->select();
  if (bAny)
  {
    pCB->insertItem(0,QCoreApplication::translate("Global","All"));
    pModel->setData(pModel->index(0,0),ID_ANY);
  }

  //Look for old selection if existed
  QModelIndexList lstIndex;
  g_fetchAllData(pModel); //Populates model before match operation
  if (Id.isValid()) lstIndex=pModel->match(pModel->index(0,0),Qt::DisplayRole,Id,1,Qt::MatchExactly);

  //Combobox shows an item that is still in the database
  if (lstIndex.size()==1)
    pCB->setCurrentIndex(lstIndex.at(0).row());
  //Combobox shows an item that was deleted from the database
  else
    pCB->setCurrentIndex(0);

  //Restore the text content for an editable combobox
  if (pCB->isEditable() && Id1.isNull()) pCB->lineEdit()->setText(sText);

  //Re-connect the signals
  pCB->blockSignals(false);
}

void g_refreshComboCategory(QComboBox *pCB, const QVariant& Id1, const QVariant& IdAcc1, bool bAny)
{
  QSqlTableModel Model;

  //Block possible signals to avoid to be triggered while refreshing
  pCB->blockSignals(true);

  //Save the selection of the Category combobox
  QVariant Id;
  int iIdx=pCB->currentIndex();
  if (Id1.isValid())
    Id=Id1;
  else if (iIdx!=-1)
    Id=pCB->itemData(iIdx);

  //Refresh Combobox
  pCB->clear();
  if (bAny)
    pCB->addItem(QCoreApplication::translate("Global","All"),ID_ANY);

  //Refresh Category ComboBox with "category" table
  Model.setTable("category");
  Model.setSort(1,Qt::AscendingOrder);
  Model.select();

  int iRowCount=g_rowCount(Model);
  for (int i=0;i<iRowCount;i++)
    pCB->addItem(/*QIcon(Model.record(i).value(2).toString()),*/
                 Model.record(i).value(1).toString(),
                 Model.record(i).value(0));

  //Refresh Category ComboBox with "account" table
  Model.setTable("account");
  Model.setSort(1,Qt::AscendingOrder);
  Model.select();

  iRowCount=g_rowCount(Model);
  for (int i=0;i<iRowCount;i++)
  {
    QVariant IdAcc=Model.record(i).value(0);
    if (IdAcc!=IdAcc1)
      pCB->addItem(/*QIcon(Model.record(i).value(1).toString()),*/
                   "["+Model.record(i).value(3).toString()+"]",
                   IdAcc.toInt()*-1);
  }

  //Look for old selection if existed
  int iIdxOld=-1;
  if (Id.isValid()) iIdxOld=pCB->findData(Id);

  //Combobox shows an item that is still in the database
  if (iIdxOld!=-1)
    pCB->setCurrentIndex(iIdxOld);
  else
    pCB->setCurrentIndex(0);

  //Re-connect the signals
  pCB->blockSignals(false);
}

void g_refreshView(QAbstractItemView *pView, bool bFilter, const QString& sFilter)
{
  //Save current index
  QModelIndex Index=pView->selectionModel()->currentIndex();

  //Refresh model
  QSqlTableModel* pModel=qobject_cast<QSqlTableModel*>(pView->model());
  if (bFilter)
    pModel->setFilter(sFilter);
  else
    pModel->select();

  //Restore current index
  if (Index.isValid())
  {
    pView->selectionModel()->setCurrentIndex(Index,QItemSelectionModel::NoUpdate);
    pView->selectionModel()->select(QItemSelection(pModel->index(Index.row(),0),pModel->index(Index.row(),pModel->columnCount()-1)),QItemSelectionModel::Select);
  }
}

void g_filterText(QComboBox *pCB,const QString &sField,QString &sFilter,bool bModel)
{
  QString sId;
  int iIdx=pCB->currentIndex();

  if (iIdx==-1) return;

  //Get the Id from Combobox's model
  if (bModel)
  {
    QSqlTableModel *pModel=qobject_cast<QSqlTableModel*>(pCB->model());
    sId=pModel->record(iIdx).value(0).toString();
  }
  //Get the Id from Combobox's user data
  else
  {
    sId=pCB->itemData(iIdx).toString();
  }

  if (sId.toInt()!=ID_ANY)
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+=sField+"="+sId;
  }
}

bool g_isActiveDatabase()
{
  return QSqlDatabase::database("qt_sql_default_connection",false).isOpen();
}

QString g_dateFormat()
{
  QSqlTableModel ModelSet;
  ModelSet.setTable("settings");
  ModelSet.select();

  return ModelSet.record(0).value(21).toString();
}

void g_createSplitTrans(const QVariant& Id1,QStandardItemModel *pModelSplit)
{
  //Set Model for Record1
  QSqlTableModel ModelTrans1;
  ModelTrans1.setTable("trans");
  ModelTrans1.setFilter("idx="+Id1.toString());
  ModelTrans1.select();
  QSqlRecord Record1=ModelTrans1.record(0);
  int iSplit=Record1.value(14).toInt();

  //Delete any previous childdren records if exist
  if (iSplit>0)
  {
    QSqlQuery Query;
    Query.setForwardOnly(true);
    Query.exec("DELETE FROM trans WHERE split="+QString::number(SPLIT+iSplit));
    Query.exec("DELETE FROM trans WHERE split="+QString::number(iSplit)+" and idx!="+Id1.toString());
  }

  //If Category chosen is SPLIT
  if (Record1.value(7).toInt()==ID_SPLIT)
  {
    //Set split number in case is a new transaction
    if (iSplit==0)
    {
      QSqlQuery Query;
      Query.setForwardOnly(true);
      Query.exec("SELECT MAX(split) FROM trans WHERE split<"+QString::number(SPLIT));
      if (Query.next())
      iSplit=Query.value(0).toInt()+1;
      if (iSplit==SPLIT) iSplit=SPLIT-1;

      Record1.setValue(14,iSplit);
      ModelTrans1.setRecord(0,Record1);
      ModelTrans1.submitAll();
    }

    //Create one transaction per row of the ModelSplit
    QSqlTableModel ModelTrans2;
    ModelTrans2.setTable("trans");
    ModelTrans2.select();
    for (int iRow=0;iRow<pModelSplit->rowCount();iRow++)
    {
      int iId=g_generateId("trans");
      ModelTrans2.insertRow(0);
      ModelTrans2.setData(ModelTrans2.index(0,0),iId);
      ModelTrans2.submitAll();
      ModelTrans2.setFilter("idx="+QString::number(iId));

      QSqlRecord Record2=ModelTrans2.record(0);

      int iCat=pModelSplit->item(iRow,0)->text().toInt(); //Cat
      if ((iCat<0)&&(iCat>=-ID_MAX))
      {
        Record2.setValue(1,0);
        Record2.setValue(5,ID_TRANSFER);
      }
      else
      {
        Record2.setValue(1,iCat);
        Record2.setValue(5,Record1.value(5));
      }

      Record2.setValue(2,Record1.value(2));
      Record2.setValue(3,Record1.value(3));
      Record2.setValue(4,pModelSplit->item(iRow,1)->text()); //Class
      Record2.setValue(6,Record1.value(6));
      Record2.setValue(7,iCat);
      Record2.setValue(8,Record1.value(8));
      Record2.setValue(9,pModelSplit->item(iRow,2)->text().toDouble()); //Amount
      Record2.setValue(10,pModelSplit->item(iRow,3)->text()); //Note
      Record2.setValue(12,0);
      Record2.setValue(13,pModelSplit->item(iRow,4)->text()); //Aux
      Record2.setValue(14,iSplit+SPLIT);

      ModelTrans2.setRecord(0,Record2);
      ModelTrans2.submitAll();

      //Generate Transfer transaction
      g_transfer(Record2.value(0));
    }
  }
  else
  {
    Record1.setValue(14,0);
    ModelTrans1.setRecord(0,Record1);
    ModelTrans1.submitAll();
  }
}

void g_initComboboxStatus(QComboBox *pCB,Status iStatus)
{
  pCB->blockSignals(true);
  pCB->clear();
  pCB->addItem(QCoreApplication::translate("Global","All status"));
  pCB->addItem(QCoreApplication::translate("Global","Reconciled"));
  pCB->addItem(QCoreApplication::translate("Global","Unreconciled"));
  pCB->addItem(QCoreApplication::translate("Global","Cleared"));
  pCB->addItem(QCoreApplication::translate("Global","Uncleared"));
  pCB->setCurrentIndex(iStatus);
  pCB->blockSignals(false);
}

void g_initComboboxAccStatus(QComboBox *pCB,int iAccStatus)
{
  pCB->blockSignals(true);
  pCB->clear();
  pCB->addItem(QCoreApplication::translate("Global","Inactive"));
  pCB->addItem(QCoreApplication::translate("Global","Active"));
  pCB->setCurrentIndex(iAccStatus);
  pCB->blockSignals(false);
}

void g_initComboboxAmounts(QComboBox *pCB,int iAmountType)
{
  pCB->blockSignals(true);
  pCB->clear();
  pCB->addItem(QCoreApplication::translate("Global","All amounts"));
  pCB->addItem(QCoreApplication::translate("Global","Incomes"));
  pCB->addItem(QCoreApplication::translate("Global","Expenses"));
  pCB->setCurrentIndex(iAmountType);
  pCB->blockSignals(false);
}

void g_filterStatus(QComboBox *pCB,QString &sFilter)
{
  int iStatus=pCB->currentIndex();
  if (iStatus==0) //All
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.status>=0";
  }
  if (iStatus==1) //Reconciled
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.status==2";
  }
  if (iStatus==2) //Unreconciled
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.status<=1";
  }
  if (iStatus==3) //Cleared
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.status>=1";
  }
  if (iStatus==4) //Uncleared
  {
    if (!sFilter.isEmpty()) sFilter+=" and ";
    sFilter+="trans.status==0";
  }
}

double g_transferRate(int iAcc1, int iAcc2, bool *pbDiff)
{
  //Get the rate for the currency of Acc1
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.setFilter("idx="+QString::number(iAcc1));
  ModelAcc.select();
  QString sCurr1=ModelAcc.record(0).value(5).toString();

  QSqlTableModel ModelCurr;
  ModelCurr.setTable("currency");
  ModelCurr.setFilter("idx="+sCurr1);
  ModelCurr.select();
  double dRate1=ModelCurr.record(0).value(2).toDouble();

  //Get the rate for the currency of Acc2
  ModelAcc.setFilter("idx="+QString::number(iAcc2));
  ModelAcc.select();
  QString sCurr2=ModelAcc.record(0).value(5).toString();

  ModelCurr.setFilter("idx="+sCurr2);
  ModelCurr.select();
  double dRate2=ModelCurr.record(0).value(2).toDouble();

  if (pbDiff)
  {
    if (sCurr1==sCurr2)
      *pbDiff=false;
    else
      *pbDiff=true;
  }

  QString s=QString::number(dRate2/dRate1,'f',RATE_PREC);
  return s.toDouble();
}

void g_messageWarning( QWidget * pWidget, const QString& sText1, const QString& sText2)
{
  QMessageBox::warning (pWidget,sText1,sText2,QMessageBox::Cancel,QMessageBox::Cancel);
}

int g_messageYesNo( QWidget * pWidget, const QString& sText1, const QString& sText2)
{
  return QMessageBox::question (pWidget,sText1,sText2,QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
}

void g_updateFontSize(QWidget * pWidget)
{
  //Get the font size from settings
  QSqlTableModel Model;
  Model.setTable("settings");
  Model.select();
  int iSizeSet=Model.record(0).value(19).toInt();

  //Obtain the size of the application global font
  QFont Font=qApp->font();
  int iSize=QFontInfo(Font).pixelSize();

  //Set the new font size
  Font.setPixelSize((double)iSize-(double)iSize/10.0*(double)iSizeSet);
  pWidget->setFont(Font);
}

void g_waitingStatus(bool bWait)
{
  if (bWait)
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  else
    QApplication::restoreOverrideCursor();
}

Resolution g_resolution()
{
  //SCREEN SIZES
  //  360x640  (small)
  //  800x480  (normal)
  //  854x480  (normal)
  //  others   (X-large)

  int iHeight=QApplication::screens().at(0)->availableGeometry().height();
  int iWidth=QApplication::screens().at(0)->availableGeometry().width();

  if (qMin(iHeight,iWidth)<=360) return ResSmall;
  if (qMin(iHeight,iWidth)<=480) return ResNormal;
  return ResXLarge;
}

QDate g_endRecurrentDate(const QDate& Date1,Recurrent iRecurrent)
{
  if (iRecurrent==RecurrentWeekly)
    return Date1.addDays(6);
  if (iRecurrent==RecurrentFortnightly)
    return Date1.addDays(13);
  if (iRecurrent==RecurrentMonthly)
    return Date1.addDays(Date1.daysInMonth()-1);

  return QDate();
}

