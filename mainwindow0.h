#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTimer>
#include <QVariant>
#include <QString>
#include <QList>
#include <QDate>
#include "global.h"
#include "transview.h"
#include "formlist.h"
#include "dr_crypt.h"

class QWidget;
class QCloseEvent;
class QObject;
class QStandardItemModel;

namespace Ui
{
  class CMainWindow;
}

class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  enum ScreenOrientation {ScreenOrientationLockPortrait,ScreenOrientationLockLandscape,ScreenOrientationAuto};

  CMainWindow(QWidget *pWidget=0);
  virtual ~CMainWindow();
  void blockTimerSch(bool bBlock);
  void filterMenusByForm(const QString& sForm);
  void addFormSplit(QStandardItemModel *pModel, const QVariant& IdAcc, double dAmountTrans, bool bRateEnable, bool bReadOnly);
  void addForm(const QString& sObjectName, const QVariant& Id1=QVariant(), const QVariant& Id2=QVariant(),
               Status iStatus=StatusAll, CTransView::Filters flFilter=CTransView::FilterNone, const QVariant& IdCurr=QVariant());
  void databaseChanged();

private:
  Ui::CMainWindow *ui;
  CFormList m_lstForms;
  enum FileAction {ActionOpen,ActionNew};
  QTimer m_TimerSch;
  QSqlDatabase m_db;
  CCrypt m_Crypt;

  QAction *m_pActionOpen;
  QAction *m_pActionNew;
  QAction *m_pActionPassword;
  QAction *m_pActionSettings;
  QAction *m_pActionHelp;
  QAction *m_pActionAbout;

  QAction *m_pActionPrint;
  QAction *m_pActionExport;
  QAction *m_pActionGeneral;
  QAction *m_pActionCurrencies;
  QAction *m_pActionSchedule;
  QAction *m_pActionPurge;
  QAction *m_pActionAdjustment;
  QAction *m_pActionSafebox;
  QAction *m_pActionTransfer;
  QAction *m_pActionReport;
  QAction *m_pActionBudget;
  QAction *m_pActionInfo;
  QAction *m_pActionAdd;
  QAction *m_pActionDelete;
  QAction *m_pActionSearch;
  QAction *m_pActionMore;
  QAction *m_pActionQuit;

private:
  CCrypt::Error encryptDatabase(const QString& sFile);
  CCrypt::Error decryptDatabase(const QString& sFile);
  void newOpenAction(FileAction iAction);
  bool openDatabase(const QString& sDataBase);
  bool upgradeDatabase();
  void filterMenus(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10,
                   bool b11, bool b12, bool b13, bool b14, bool b15, bool b16, bool b17);

protected:
  void closeEvent(QCloseEvent *event);

public slots:
  void closeForm(QObject* pObject);

private slots:
  void actionOpen();
  void actionNew();
  void actionPassword();
  void actionSettings();
  void actionHelp();
  void actionAbout();

  void actionPrint();
  void actionExport();
  void actionGeneral();
  void actionCurrencies();
  void actionSchedule();
  void actionPurge();
  void actionAdjustment();
  void actionSafebox();
  void actionBudget();
  void actionTransfer();
  void actionSearch();
  void actionMore();
  void actionQuit();

  void timeoutSchedule();
  void timeoutBudget();

signals:
  void beforeChangeDatabase();
  void afterChangeDatabase();
  void newTransaction();
  void updateBudget();
  void beforeCloseEvent();

  void clickReport();
  void clickInfo();
  void clickAdd();
  void clickDelete();
};


#endif // MAINWINDOW_H
