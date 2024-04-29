#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include <QtPrintSupport>
#include "mainwindow0.h"
#include "ui_mainwindow0.h"
#include "generallist.h"
#include "currencyedit.h"
#include "scheduledtrans.h"
#include "accountsview.h"
#include "accountedit.h"
#include "accountinfo.h"
#include "transview.h"
#include "transedit.h"
#include "transinfo.h"
#include "splittrans.h"
#include "settings.h"
#include "login.h"
#include "global.h"
#include "dr_about.h"
#include "purgetrans.h"
#include "report.h"
#include "transfer.h"
#include "groupedit.h"
#include "itemview.h"
#include "itemedit.h"
#include "iteminfo.h"
#include "budgetedit.h"
#include "budgetview.h"
#include "budgetrep.h"
#include "search.h"
#include "dr_profile.h"
#include "help0.h"
#include "dr_crypt.h"

#define MAX_FONTWIDTH		14
#define TITLE_FONTSIZE	1.20
#define PAGE_FONTSIZE		0.90
#define FONT_FACTOR			1.40
#define FONT_FAMILY			"sans"
#define TR_ACCOUNT			QT_TRANSLATE_NOOP("Global","Account:")
#define TR_CATEGORY			QT_TRANSLATE_NOOP("Global","Category:")
#define TR_PAYEE				QT_TRANSLATE_NOOP("Global","Payee:")
#define TR_CLASS				QT_TRANSLATE_NOOP("Global","Class:")
#define TR_RANGE				QT_TRANSLATE_NOOP("Global","Range:")
#define TR_PAGE					QT_TRANSLATE_NOOP("Global","Page:")

CMainWindow::CMainWindow(QWidget *pWidget)
    : QMainWindow(pWidget), ui(new Ui::CMainWindow)
{
  ui->setupUi(this);

  //Initiate encryption object
  m_Crypt.setEncryptionMode(CCrypt::SIM256);
  m_Crypt.setMark(QByteArray::fromHex(CRYPT_MARK));
  m_Crypt.setOldKey(QByteArray::fromHex("6e0961d11de537dd034ef1e4fa117c68"));

  //Create the actions
  m_pActionOpen = new QAction(this);
  m_pActionOpen->setText(tr("Open"));

  m_pActionNew = new QAction(this);
  m_pActionNew->setText(tr("New"));

  m_pActionPassword = new QAction(this);
  m_pActionPassword->setText(tr("Password"));

  m_pActionSettings = new QAction(this);
  m_pActionSettings->setText(tr("Settings"));

  m_pActionHelp = new QAction(this);
  m_pActionHelp->setText(tr("Help"));

  m_pActionAbout = new QAction(this);
  m_pActionAbout->setText(tr("About"));

  m_pActionPrint = new QAction(this);
  m_pActionPrint->setText(tr("Print"));

  m_pActionExport = new QAction(this);
  m_pActionExport->setText(tr("Export"));

  m_pActionGeneral = new QAction(this);
  m_pActionGeneral->setText(tr("General"));

  m_pActionCurrencies = new QAction(this);
  m_pActionCurrencies->setText(tr("Currencies"));

  m_pActionSchedule = new QAction(this);
  m_pActionSchedule->setText(tr("Schedule"));

  m_pActionPurge = new QAction(this);
  m_pActionPurge->setText(tr("Purge"));

  m_pActionAdjustment = new QAction(this);
  m_pActionAdjustment->setText(tr("Adjustment"));

  m_pActionSafebox = new QAction(this);
  m_pActionSafebox->setText(tr("Safebox"));

  m_pActionTransfer = new QAction(this);
  m_pActionTransfer->setText(tr("Transfer"));

  m_pActionReport = new QAction(this);
  m_pActionReport->setText(tr("Report"));

  m_pActionBudget = new QAction(this);
  m_pActionBudget->setText(tr("Budget"));

  m_pActionInfo = new QAction(this);
  m_pActionInfo->setText(tr("Info"));

  m_pActionAdd = new QAction(this);
  m_pActionAdd->setText(tr("Add"));

  m_pActionDelete = new QAction(this);
  m_pActionDelete->setText(tr("Delete"));

  m_pActionSearch = new QAction(this);
  m_pActionSearch->setText(tr("Search"));

  m_pActionMore = new QAction(this);
  m_pActionMore->setText(tr("More.."));

  m_pActionQuit = new QAction(this);
  m_pActionQuit->setText(tr("Quit"));

#ifdef MOBILE_PLATFORM
  ui->pMenuBar->addAction(m_pActionAdd);
  ui->pMenuBar->addAction(m_pActionInfo);
  ui->pMenuBar->addAction(m_pActionDelete);
  ui->pMenuBar->addAction(m_pActionTransfer);
  ui->pMenuBar->addAction(m_pActionReport);
  ui->pMenuBar->addAction(m_pActionBudget);
  ui->pMenuBar->addAction(m_pActionSchedule);
  ui->pMenuBar->addAction(m_pActionSafebox);
  ui->pMenuBar->addAction(m_pActionGeneral);
  ui->pMenuBar->addAction(m_pActionCurrencies);
  ui->pMenuBar->addAction(m_pActionSearch);
  ui->pMenuBar->addAction(m_pActionMore);
  ui->pMenuBar->addAction(m_pActionPrint);
  ui->pMenuBar->addAction(m_pActionExport);
  ui->pMenuBar->addAction(m_pActionPurge);
  ui->pMenuBar->addAction(m_pActionAdjustment);

#else

  QIcon Icon1;
  Icon1.addFile(":/menu/icons/menu-open.png");
  m_pActionOpen->setIcon(Icon1);

  QIcon Icon2;
  Icon2.addFile(":/menu/icons/menu-new.png");
  m_pActionNew->setIcon(Icon2);

  QIcon Icon3;
  Icon3.addFile(":/menu/icons/menu-password.png");
  m_pActionPassword->setIcon(Icon3);

  QIcon Icon4;
  Icon4.addFile(":/menu/icons/menu-settings.png");
  m_pActionSettings->setIcon(Icon4);

  QIcon Icon5;
  Icon5.addFile(":/menu/icons/menu-help.png");
  m_pActionHelp->setIcon(Icon5);

  QIcon Icon6;
  Icon6.addFile(":/menu/icons/menu-about.png");
  m_pActionAbout->setIcon(Icon6);

  QIcon Icon7;
  Icon7.addFile(":/menu/icons/menu-print.png");
  m_pActionPrint->setIcon(Icon7);

  QIcon Icon8;
  Icon8.addFile(":/menu/icons/menu-export.png");
  m_pActionExport->setIcon(Icon8);

  QIcon Icon9;
  Icon9.addFile(":/menu/icons/menu-general.png");
  m_pActionGeneral->setIcon(Icon9);

  QIcon Icon10;
  Icon10.addFile(":/menu/icons/menu-currencies.png");
  m_pActionCurrencies->setIcon(Icon10);

  QIcon Icon11;
  Icon11.addFile(":/menu/icons/menu-schedule.png");
  m_pActionSchedule->setIcon(Icon11);

  QIcon Icon12;
  Icon12.addFile(":/menu/icons/menu-purge.png");
  m_pActionPurge->setIcon(Icon12);

  QIcon Icon13;
  Icon13.addFile(":/menu/icons/menu-adjustment.png");
  m_pActionAdjustment->setIcon(Icon13);

  QIcon Icon14;
  Icon14.addFile(":/menu/icons/menu-safebox.png");
  m_pActionSafebox->setIcon(Icon14);

  QIcon Icon15;
  Icon15.addFile(":/menu/icons/menu-transfer.png");
  m_pActionTransfer->setIcon(Icon15);

  QIcon Icon16;
  Icon16.addFile(":/menu/icons/menu-report.png");
  m_pActionReport->setIcon(Icon16);

  QIcon Icon17;
  Icon17.addFile(":/menu/icons/menu-budget.png");
  m_pActionBudget->setIcon(Icon17);

  QIcon Icon18;
  Icon18.addFile(":/menu/icons/menu-info.png");
  m_pActionInfo->setIcon(Icon18);

  QIcon Icon19;
  Icon19.addFile(":/menu/icons/menu-add.png");
  m_pActionAdd->setIcon(Icon19);

  QIcon Icon20;
  Icon20.addFile(":/menu/icons/menu-delete.png");
  m_pActionDelete->setIcon(Icon20);

  QIcon Icon21;
  Icon21.addFile(":/menu/icons/menu-search.png");
  m_pActionSearch->setIcon(Icon21);

  QIcon Icon22;
  Icon22.addFile(":/menu/icons/menu-more.png");
  m_pActionMore->setIcon(Icon22);

  QIcon Icon23;
  Icon23.addFile(":/menu/icons/menu-quit.png");
  m_pActionQuit->setIcon(Icon23);

  //Create main menus
  QMenu* pMenuFile = new QMenu(tr("File"),ui->pMenuBar);
  QMenu* pMenuData = new QMenu(tr("Data"),ui->pMenuBar);
  QMenu* pMenuTransactions = new QMenu(tr("Transactions"),ui->pMenuBar);
  QMenu* pMenuActions = new QMenu(tr("Actions"),ui->pMenuBar);
  QMenu* pMenuTools = new QMenu(tr("Tools"),ui->pMenuBar);
  QMenu* pMenuSettings = new QMenu(tr("Settings"),ui->pMenuBar);
  QMenu* pMenuHelp = new QMenu(tr("Help"),ui->pMenuBar);

  //Add actions for menu File
  pMenuFile->addAction(m_pActionOpen);
  pMenuFile->addAction(m_pActionNew);
  pMenuFile->addSeparator();
  pMenuFile->addAction(m_pActionPrint);
  pMenuFile->addAction(m_pActionExport);
  pMenuFile->addSeparator();
  pMenuFile->addAction(m_pActionQuit);

  //Add actions for menu Data
  pMenuData->addAction(m_pActionGeneral);
  pMenuData->addAction(m_pActionCurrencies);
  pMenuData->addSeparator();
  pMenuData->addAction(m_pActionSchedule);

  //Add actions for menu Transactions
  pMenuTransactions->addAction(m_pActionTransfer);
  pMenuTransactions->addSeparator();
  pMenuTransactions->addAction(m_pActionPurge);
  pMenuTransactions->addAction(m_pActionAdjustment);

  //Add actions for menu Actions
  pMenuActions->addAction(m_pActionInfo);
  pMenuActions->addAction(m_pActionDelete);
  pMenuActions->addAction(m_pActionAdd);

  //Add actions for menu Tools
  pMenuTools->addAction(m_pActionReport);
  pMenuTools->addAction(m_pActionBudget);
  pMenuTools->addSeparator();
  pMenuTools->addAction(m_pActionSafebox);
  pMenuTools->addAction(m_pActionSearch);

  //Add actions for menu Settings
  pMenuSettings->addAction(m_pActionSettings);
  pMenuSettings->addAction(m_pActionPassword);

  //Add actions for menu Help
  pMenuHelp->addAction(m_pActionHelp);
  pMenuHelp->addAction(m_pActionAbout);

  //Add Menus to MenuBar
  ui->pMenuBar->addMenu(pMenuFile);
  ui->pMenuBar->addMenu(pMenuData);
  ui->pMenuBar->addMenu(pMenuTransactions);
  ui->pMenuBar->addMenu(pMenuActions);
  ui->pMenuBar->addMenu(pMenuTools);
  ui->pMenuBar->addMenu(pMenuSettings);
  ui->pMenuBar->addMenu(pMenuHelp);

  //Create Toolbar and add actions
  QToolBar* pToolBar=new QToolBar(this);
  pToolBar->setObjectName("QToolBar");
  pToolBar->setFloatable(false);

  pToolBar->addAction(m_pActionInfo);
  pToolBar->addAction(m_pActionDelete);
  pToolBar->addAction(m_pActionAdd);

  addToolBar(pToolBar);
#endif

  //Set Application Icon for different resolutions
  QIcon Icon;
  Icon.addFile(":/others/icons/drfinance16.png", QSize(16,16), QIcon::Normal, QIcon::Off);
  Icon.addFile(":/others/icons/drfinance24.png", QSize(24,24), QIcon::Normal, QIcon::Off);
  Icon.addFile(":/others/icons/drfinance32.png", QSize(32,32), QIcon::Normal, QIcon::Off);
  Icon.addFile(":/others/icons/drfinance48.png", QSize(48,48), QIcon::Normal, QIcon::Off);
  Icon.addFile(":/others/icons/drfinance64.png", QSize(64,64), QIcon::Normal, QIcon::Off);
  setWindowIcon(Icon);

  m_db=QSqlDatabase::database("qt_sql_default_connection",false);

  //Load settings from file and set application
  QSettings Settings;
  resize(Settings.value("size", sizeHint()).toSize());
  QRect Rect=QApplication::screens().at(0)->availableGeometry();
  move(Settings.value("pos", QPoint((Rect.width()-sizeHint().width())/2,
                                    (Rect.height()-sizeHint().height())/2)).toPoint());
  restoreState(Settings.value("mainwindow",QByteArray()).toByteArray());
  QString sDataBase=Settings.value("database","").toString();

  //Open last database
  openDatabase(sDataBase);

  //Connections
  connect(m_pActionOpen,SIGNAL(triggered()),this,SLOT(actionOpen()));
  connect(m_pActionNew,SIGNAL(triggered()),this,SLOT(actionNew()));
  connect(m_pActionPassword,SIGNAL(triggered()),this,SLOT(actionPassword()));
  connect(m_pActionSettings,SIGNAL(triggered()),this,SLOT(actionSettings()));
  connect(m_pActionHelp,SIGNAL(triggered()),this,SLOT(actionHelp()));
  connect(m_pActionAbout,SIGNAL(triggered()),this,SLOT(actionAbout()));

  connect(m_pActionPrint,SIGNAL(triggered()),this,SLOT(actionPrint()));
  connect(m_pActionExport,SIGNAL(triggered()),this,SLOT(actionExport()));
  connect(m_pActionGeneral,SIGNAL(triggered()),this,SLOT(actionGeneral()));
  connect(m_pActionCurrencies,SIGNAL(triggered()),this,SLOT(actionCurrencies()));
  connect(m_pActionSchedule,SIGNAL(triggered()),this,SLOT(actionSchedule()));
  connect(m_pActionPurge,SIGNAL(triggered()),this,SLOT(actionPurge()));
  connect(m_pActionAdjustment,SIGNAL(triggered()),this,SLOT(actionAdjustment()));
  connect(m_pActionSafebox,SIGNAL(triggered()),this,SLOT(actionSafebox()));
  connect(m_pActionTransfer,SIGNAL(triggered()),this,SLOT(actionTransfer()));
  connect(m_pActionReport,SIGNAL(triggered()),this,SIGNAL(clickReport()));
  connect(m_pActionBudget,SIGNAL(triggered()),this,SLOT(actionBudget()));
  connect(m_pActionInfo,SIGNAL(triggered()),this,SIGNAL(clickInfo()));
  connect(m_pActionAdd,SIGNAL(triggered()),this,SIGNAL(clickAdd()));
  connect(m_pActionDelete,SIGNAL(triggered()),this,SIGNAL(clickDelete()));
  connect(m_pActionSearch,SIGNAL(triggered()),this,SLOT(actionSearch()));
  connect(m_pActionMore,SIGNAL(triggered()),this,SLOT(actionMore()));
  connect(m_pActionQuit,SIGNAL(triggered()),this,SLOT(actionQuit()));

  //Initiate Timer for schedule transatcions and budgets
  connect(&m_TimerSch,SIGNAL(timeout()),this,SLOT(timeoutSchedule()));
  connect(&m_TimerSch,SIGNAL(timeout()),this,SLOT(timeoutBudget()));
  m_TimerSch.setInterval(20000);
  m_TimerSch.start();
}

CMainWindow::~CMainWindow()
{
  delete ui;
}

CCrypt::Error CMainWindow::encryptDatabase(const QString& sFile)
{
  m_Crypt.encryptFile(sFile);
  return m_Crypt.lastError();
}

CCrypt::Error CMainWindow::decryptDatabase(const QString& sFile)
{
  m_Crypt.decryptFile(sFile);
  return m_Crypt.lastError();
}

void CMainWindow::blockTimerSch(bool bBlock)
{
  if (bBlock)
    m_TimerSch.stop();
  else
    m_TimerSch.start();
}

void CMainWindow::closeEvent(QCloseEvent *pEvent)
{
  //Emit signal to trigger actions in children objects
  emit beforeCloseEvent();

  //Save settings to file
  QSettings Settings;
  Settings.setValue("size", size());
  Settings.setValue("pos", pos());
  Settings.setValue("mainwindow", saveState());
  if (g_isActiveDatabase())
  {
    m_db.close();
    Settings.setValue("database",m_db.databaseName());
    encryptDatabase(m_db.databaseName());
  }

  pEvent->accept();
}

void CMainWindow::addFormSplit(QStandardItemModel *pModel, const QVariant& IdAcc, double dAmountTrans, bool bRateEnable, bool bReadOnly)
{
  QWidget *pWidget;

  pWidget=new CSplitTrans(pModel,IdAcc,dAmountTrans,bRateEnable,bReadOnly,this);

  m_lstForms.addWidget(pWidget);
  ui->pLayout->addWidget(pWidget);
  filterMenusByForm(pWidget->objectName());
}

void CMainWindow::addForm(const QString& sObjectName, const QVariant& Id1, const QVariant& Id2,
                         Status iStatus, CTransView::Filters flFilter, const QVariant& IdCurr)
{
  if (m_lstForms.containsWidget(sObjectName)) return;

  g_waitingStatus(true);

  QWidget *pWidget=0;

  if (sObjectName=="CGeneralList")
    pWidget=new CGeneralList(this);

  if (sObjectName=="CCurrencyEdit")
    pWidget=new CCurrencyEdit(this);

  if (sObjectName=="CScheduledTrans")
    pWidget=new CScheduledTrans(this);

  if (sObjectName=="CSettings")
    pWidget=new CSettings(this);

  if (sObjectName=="CItemView")
    pWidget=new CItemView(this);

  if (sObjectName=="CItemEdit")
    pWidget=new CItemEdit(static_cast<Action>(Id2.toInt()),Id1,this);

  if (sObjectName=="CItemInfo")
    pWidget=new CItemInfo(Id1,this);

  if (sObjectName=="CGroupEdit")
    pWidget=new CGroupEdit(Id1,this);

  if (sObjectName=="CTransView")
    pWidget=new CTransView(iStatus,flFilter,Id1,Id2,IdCurr,this);

  if (sObjectName=="CTransEdit")
    pWidget=new CTransEdit(static_cast<Action>(Id2.toInt()),Id1,this);

  if (sObjectName=="CTransInfo")
    pWidget=new CTransInfo(Id1,this);

  if (sObjectName=="CAccountsView")
    pWidget=new CAccountsView(this);

  if (sObjectName=="CAccountEdit")
    pWidget=new CAccountEdit(Id1,this);

  if (sObjectName=="CAccountInfo")
    pWidget=new CAccountInfo(Id1,this);

  if (sObjectName=="CReport")
    pWidget=new CReport(iStatus,this);

  if (sObjectName=="CTransfer")
    pWidget=new CTransfer(this);

  if (sObjectName=="CBudgetEdit")
    pWidget=new CBudgetEdit(Id1,this);

  if (sObjectName=="CBudgetView")
    pWidget=new CBudgetView(this);

  if (sObjectName=="CBudgetRep")
    pWidget=new CBudgetRep(Id1,this);

  if (sObjectName=="CSearch")
    pWidget=new CSearch(this);

  if (sObjectName=="CHelp")
    pWidget=new CHelp(this);

  m_lstForms.addWidget(pWidget);
  ui->pLayout->addWidget(pWidget);
  filterMenusByForm(sObjectName);

  g_waitingStatus(false);
}

void CMainWindow::closeForm(QObject* pObject)
{
  QWidget *pWidget=qobject_cast<QWidget*>(pObject);
  m_lstForms.removeWidget(pWidget);
  if (!m_lstForms.isEmpty()) filterMenusByForm(m_lstForms.last()->objectName());
}

bool CMainWindow::openDatabase(const QString& sDataBase)
{
  //Objective Positive: databaseName="..." - Open (database) - setWindow(...) - setMenus - return (true)
  //Objective Negative: databaseName="" - Close (database) - setWindow(..none..) - setMenus - return (false)

  //Validate variable: Empty only if create wasn't successful
  if (sDataBase.isEmpty())
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    return false;
  }

  //Verify if file exist - User could have deleted or renamed the file
  if (!QFile::exists(sDataBase))
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    return false;
  }

  //Get password
  CLogin Login(QByteArray(),CLogin::TypeLogin,this);
  Login.exec();
  m_Crypt.setPass(Login.pass());

  //Validate password isn't NULL
  if (m_Crypt.pass().isEmpty())
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    return false;
  }

  //Dencrypt file
  CCrypt::Error iError=decryptDatabase(sDataBase);
  if (iError==CCrypt::ErrorUnknownVersion)
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    g_messageWarning(
        this,
        tr("Open database"),
        tr("The database's version isn't supported.\nYou will need to upgrade your application."));
    return false;
  }
  if (iError==CCrypt::ErrorIntegrityFailed)
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    g_messageWarning(this,tr("Open database"),tr("Your database seems to be corrupted."));
    return false;
  }
  if (iError==CCrypt::ErrorWrongPass)
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    g_messageWarning(this,tr("Open database"),tr("Wrong password. Please try again."));
    return false;
  }

  //Open database
  m_db.setDatabaseName(QDir::toNativeSeparators(sDataBase));
  if (!m_db.open())
  {
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    g_messageWarning(this,tr("Open database"),tr("Error reported by the driver while opening the database:\n")+m_db.lastError().text());
    return false;
  }

  //Validate database
  QStringList lsts=m_db.tables();
  if ((!lsts.contains("settings"))||(!lsts.contains("account"))||(!lsts.contains("trans")))
  {
    m_db.close();
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    g_messageWarning(this,tr("Validate database"),tr("The database is not a valid DRF file."));
    return false;
  }

  //Upgrade database
  if (!upgradeDatabase())
  {
    m_db.close();
    m_db.setDatabaseName("");
    filterMenusByForm("CAccountsView");
    encryptDatabase(sDataBase);
    return false;
  }

  filterMenusByForm("CAccountsView");
  return true;
}

void CMainWindow::actionOpen()
{
  newOpenAction(ActionOpen);
}

void CMainWindow::actionNew()
{
  newOpenAction(ActionNew);
}

void CMainWindow::actionPassword()
{
  CLogin Login(m_Crypt.pass(),CLogin::TypePass,this);
  Login.exec();

  if (!Login.pass().isEmpty()) m_Crypt.setPass(Login.pass());
}

void CMainWindow::actionSettings()
{
  addForm("CSettings");
}

void CMainWindow::actionHelp()
{
  addForm("CHelp");
}

void CMainWindow::actionAbout()
{
  const QString sIconAbout=":/others/icons/about.png";
  const QString sIconDR=":/others/icons/darhon.png";
  const QString sAppName="Darhon Finance";
  const QString sCaption=tr("Manage your Personal Accounts");
  const QString sDescription=tr("Darhon Finance is a simple to use and yet powerful software packed with all the necessary tools to control your accounts.");
  const QString sCopyright = tr("Copyright 2010-2024 - Darhon Software");
  const QString sPrivacy="<a href='https://darhon.com/darhon-finance-privacy-policy'>"+tr("Privacy Policy")+"</a>";

  CAbout About(sIconAbout,sIconDR,sAppName,sCaption,sDescription,sCopyright,sPrivacy,this);
  About.exec();
}

void CMainWindow::actionPrint()
{
  //Initialize Printer
  QPrinter Printer;
  Printer.setCreator(qApp->applicationName());

  QPrintDialog PrintDialog(&Printer, this);
  if (PrintDialog.exec() == QDialog::Accepted)
  {
    //Initiate Models
    QSqlTableModel ModelAcc;
    ModelAcc.setTable("account");
    ModelAcc.select();

    QSqlTableModel ModelCat;
    ModelCat.setTable("category");
    ModelCat.select();

    QSqlTableModel ModelPayee;
    ModelPayee.setTable("payee");
    ModelPayee.select();

    QSqlTableModel ModelClass;
    ModelClass.setTable("class");
    ModelClass.select();

    //Initiate pointers
    CTransView *pTransView=qobject_cast<CTransView *>(m_lstForms.last());
    QTableView *pTableView=pTransView->tableView();
    QSqlTableModel *pModel=qobject_cast<QSqlTableModel *>(pTableView->model());

    //Initiate variables
    QString s;
    QList<int> lstMaxCol;
    QString sDateFormat=g_dateFormat();
    int iNumCols=pModel->columnCount();
    int iNumRows=g_rowCount(*pModel);
    int iWidth=Printer.width();
    int iHeight=Printer.height();

    //Get the maximum size of each column and fill List
    for (int iCol=0;iCol<iNumCols;iCol++)
      if (!pTableView->isColumnHidden(iCol))
      {
        //Get the lenght of the Title
        int iLen=pModel->headerData(iCol,Qt::Horizontal).toString().length();

        //Icon
        if (iCol==1)
          if (2>iLen) iLen=2;

        //Status
        if (iCol==2)
          if (3>iLen) iLen=3;

        //date
        if (iCol==3)
          if (sDateFormat.length()>iLen) iLen=sDateFormat.length();

        //class || payee || account || cheque
        if ((iCol==4)||(iCol==5)||(iCol==6)||(iCol==8))
          for (int iRow=0;iRow<iNumRows;iRow++)
            if (pModel->record(iRow).value(iCol).toString().length()>iLen)
              iLen=pModel->record(iRow).value(iCol).toString().length();

        //category
        if (iCol==7)
          for (int iRow=0;iRow<iNumRows;iRow++)
          {
            int iCat=pModel->record(iRow).value(iCol).toInt();
            if ((iCat>=0)||(iCat==ID_SPLIT))
            {
              ModelCat.setFilter("idx="+QString::number(iCat));
              s=ModelCat.record(0).value(1).toString();
            }
            else
            {
              ModelAcc.setFilter("idx="+QString::number(iCat*-1));
              s="["+ModelAcc.record(0).value(3).toString()+"]";
            }

            if (s.length()>iLen)
              iLen=s.length();
          }

        //amount || balance
        if ((iCol==9)||(iCol==11))
          for (int iRow=0;iRow<iNumRows;iRow++)
          {
            double d=pModel->record(iRow).value(iCol).toDouble();
            if (d<0)
              s="("+ QString::number(d*-1.0,'f',2) + ")";
            else
              s=QString::number(d,'f',2);
            if (s.length()>iLen) iLen=s.length();
          }

        //Add to the List
        lstMaxCol.append(iLen);
      }
      else
        lstMaxCol.append(0);

    //Evaluate the maximum number of characters for an horizontal line on the Data section
    int iTotColSize=0;
    for (int i=0; i<lstMaxCol.length(); i++)
      iTotColSize+=lstMaxCol.at(i);

    //Evaluate the maximum number of characters for an horizontal line on the Title section
    CTransView::Filters flFilter=pTransView->filter();

    int iTitle1=0;
    if (flFilter & CTransView::FilterPayee) //Payee:_XXXX
    {
      for (int iRow=0;iRow<iNumRows;iRow++)
      {
        int iLen;
        iLen=pModel->record(iRow).value(5).toString().length();
        if (iLen>iTitle1) iTitle1=iLen;
      }
      iTitle1+=QString(QCoreApplication::translate("Global",TR_PAYEE)).length()+1;
    }
    if (flFilter & CTransView::FilterAccount) //Account:_XXXX
    {
      for (int iRow=0;iRow<iNumRows;iRow++)
      {
        int iLen;
        iLen=pModel->record(iRow).value(6).toString().length();
        if (iLen>iTitle1) iTitle1=iLen;
      }
      iTitle1+=QString(QCoreApplication::translate("Global",TR_ACCOUNT)).length()+1;
    }
    if (flFilter & CTransView::FilterCategory) //Category:_XXXX
    {
      for (int iRow=0;iRow<iNumRows;iRow++)
      {
        int iCat=pModel->record(iRow).value(7).toInt();
        if ((iCat>=0)||(iCat==ID_SPLIT))
        {
          ModelCat.setFilter("idx="+QString::number(iCat));
          s=ModelCat.record(0).value(1).toString();
        }
        else
        {
          ModelAcc.setFilter("idx="+QString::number(iCat*-1));
          s="["+ModelAcc.record(0).value(3).toString()+"]";
        }
        if (s.length()>iTitle1)
          iTitle1=s.length();
      }
      iTitle1+=QString(QCoreApplication::translate("Global",TR_CATEGORY)).length()+1;
    }

    int iTitle2=0;
    if (flFilter & CTransView::FilterClass) //Class:_XXXX
    {
      for (int iRow=0;iRow<iNumRows;iRow++)
      {
        int iLen;
        iLen=pModel->record(iRow).value(4).toString().length();
        if (iLen>iTitle2) iTitle2=iLen;
      }
      iTitle2+=QString(QCoreApplication::translate("Global",TR_CLASS)).length()+1;
    }

    int iTitle3=0;
    if (!pTransView->rangeDates().isEmpty()) //Range:_XXXX_XXXX
    {
      iTitle3=QString(QCoreApplication::translate("Global",TR_RANGE)).length()+1;
      iTitle3+=pTransView->rangeDates().length()+1;
      iTitle3+=pTransView->rangeAmounts().length();
    }

    //Account/Category/Payee:_XXXX__Class:_XXXX__Range:_XXXX_XXXX__Page:_000/000
    int iTotTitleSize=iTitle1+2+iTitle2+2+iTitle3+2+QString(QCoreApplication::translate("Global",TR_PAGE)).length()+8;

    //Evaluate the Font width to be used in all Horizontal measurements
    int iTotSize=qMax(iTotColSize,iTotTitleSize);
    int iFontWidth=qMin(iWidth/iTotSize,MAX_FONTWIDTH);

    //Initializes Title font
    QFont FontTitle(FONT_FAMILY);
    FontTitle.setWeight(QFont::Bold);
    FontTitle.setPixelSize(qRound((double)iFontWidth*FONT_FACTOR*TITLE_FONTSIZE));

    //Initializes SubTitle font
    QFont FontSubTitle(FONT_FAMILY);
    FontSubTitle.setWeight(QFont::Bold);
    FontSubTitle.setPixelSize((double)iFontWidth*FONT_FACTOR);

    //Initializes Text font
    QFont FontText(FONT_FAMILY);
    FontText.setWeight(QFont::Normal);
    FontText.setPixelSize((double)iFontWidth*FONT_FACTOR);

    //Initializes Page number font
    QFont FontNumber(FONT_FAMILY);
    FontNumber.setWeight(QFont::Normal);
    FontNumber.setPixelSize(qRound((double)iFontWidth*FONT_FACTOR*PAGE_FONTSIZE));

    //Evaluate the number of rows per page and the Font height to be used in all Vertical measurement
    int iFontHeight=QFontMetrics(FontText).height();
    int iRowsPerPage=(iHeight-QFontMetrics(FontTitle).height()-iFontHeight*3)/(iFontHeight);

    //Evaluate the number of pages
    int iPages=qRound(iNumRows/iRowsPerPage+0.5);

    //Initiate Painter
    QPainter Painter(&Printer);

    //Initializes common variables
    QRect Rect;
    int iY;

    for (int iPage=1;iPage<=iPages;iPage++)
    {
      if (iPage>1) Printer.newPage();

      //Writes Title (Line1)
      iY=0;
      s=tr("Transactions")+" "+pTransView->currencyName();
      Painter.setFont(FontTitle);
      Rect=Painter.boundingRect(0,iY,iWidth,iHeight,Qt::AlignHCenter|Qt::AlignTop,s);
      Painter.drawText(Rect,0,s);

      //Writes Title (Line2 - Left)
      iY=QFontMetrics(FontTitle).height()+iFontHeight;

      if (flFilter & CTransView::FilterAccount)
      {
        s=QCoreApplication::translate("Global",TR_ACCOUNT)+" ";
        Painter.setFont(FontSubTitle);
        Rect=Painter.boundingRect(0,iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);

        ModelAcc.setFilter("idx="+pTransView->id1().toString());
        s=ModelAcc.record(0).value(3).toString();
        Painter.setFont(FontText);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);
      }

      if (flFilter & CTransView::FilterPayee)
      {
        s=QCoreApplication::translate("Global",TR_PAYEE)+" ";
        Painter.setFont(FontSubTitle);
        Rect=Painter.boundingRect(0,iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);

        ModelPayee.setFilter("idx="+pTransView->id1().toString());
        s=ModelPayee.record(0).value(1).toString();
        Painter.setFont(FontText);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);
      }

      if (flFilter & CTransView::FilterCategory)
      {
        s=QCoreApplication::translate("Global",TR_CATEGORY)+" ";
        Painter.setFont(FontSubTitle);
        Rect=Painter.boundingRect(0,iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);

        ModelCat.setFilter("idx="+pTransView->id1().toString());
        s=ModelCat.record(0).value(1).toString();
        Painter.setFont(FontText);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);
      }

      if (flFilter & CTransView::FilterClass)
      {
        s="  "+QCoreApplication::translate("Global",TR_CLASS)+" ";
        Painter.setFont(FontSubTitle);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);

        ModelClass.setFilter("idx="+pTransView->id2().toString());
        s=ModelClass.record(0).value(1).toString();
        Painter.setFont(FontText);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);
      }

      if (!pTransView->rangeDates().isEmpty())
      {
        s="  "+QCoreApplication::translate("Global",TR_RANGE)+" ";
        Painter.setFont(FontSubTitle);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);

        s=pTransView->rangeDates();
        Painter.setFont(FontText);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);
      }

      if (!pTransView->rangeAmounts().isEmpty())
      {
        s=" ("+pTransView->rangeAmounts()+")";
        Painter.setFont(FontText);
        Rect=Painter.boundingRect(Rect.right(),iY,iWidth,iHeight,Qt::AlignLeft|Qt::AlignTop,s);
        Painter.drawText(Rect,0,s);
      }

      //Writes Title (Line2 - Right)
      s=QString(QCoreApplication::translate("Global",TR_PAGE)+" %1/%2").arg(iPage,3,10,QChar('0')).arg(iPages,3,10,QChar('0'));
      Painter.setFont(FontNumber);
      Rect=Painter.boundingRect(0,iY,iWidth,iHeight,Qt::AlignRight|Qt::AlignTop,s);
      Painter.drawText(Rect,0,s);

      //Create mapping between visual idx and logic idx
      QHeaderView *pHeader=pTableView->horizontalHeader();
      QMap<int,int> mapColumns;
      for (int iCol=0;iCol<iNumCols;iCol++)
        if (!pTableView->isColumnHidden(iCol))
          mapColumns.insert(pHeader->visualIndex(iCol),iCol);

      //Writes first row
      Painter.setFont(FontText);
      iY+=iFontHeight*2;
      Rect.setRect(0,iY,0,iFontHeight);

      foreach (int iCol, mapColumns)
      {
        Rect.moveLeft(Rect.width()+Rect.left());
        Rect.setWidth(lstMaxCol.at(iCol)*iFontWidth);
        s=pModel->headerData(iCol,Qt::Horizontal).toString();
        Painter.drawText(Rect,Qt::AlignHCenter|Qt::AlignTop,s);
      }

      //Writes data rows
      Painter.setFont(FontText);
      int iPageRows=iRowsPerPage;
      if (iPage==iPages) iPageRows=iNumRows-(iPages-1)*iRowsPerPage;

      for (int iRow=0;iRow<iPageRows;iRow++)
      {
        iY+=iFontHeight;
        Rect.setRect(0,iY,0,iFontHeight);
        foreach (int iCol, mapColumns)
        {
          Rect.moveLeft(Rect.width()+Rect.left());
          Rect.setWidth(lstMaxCol.at(iCol)*iFontWidth);
          s=pModel->record(iRow+iRowsPerPage*(iPage-1)).value(iCol).toString();

          //Icon
          if (iCol==1)
            Painter.drawPixmap(Rect.x()+(Rect.width()-Rect.height())/2,Rect.y(),Rect.height(),Rect.height(),QPixmap(s));

          //Status
          if (iCol==2)
            Painter.drawText(Rect,Qt::AlignHCenter|Qt::AlignTop,s);

          //date
          if (iCol==3)
          {
            QDate Date=QDate::fromJulianDay(s.toInt());
            Painter.drawText(Rect,Qt::AlignHCenter|Qt::AlignTop,Date.toString(sDateFormat));
          }

          //class || payee || account || cheque
          if ((iCol==4)||(iCol==5)||(iCol==6)||(iCol==8))
            Painter.drawText(Rect,Qt::AlignHCenter|Qt::AlignTop,s);

          //category
          if (iCol==7)
          {
            int iCat=s.toInt();
            if ((iCat>=0)||(iCat==ID_SPLIT))
            {
              ModelCat.setFilter("idx="+QString::number(iCat));
              s=ModelCat.record(0).value(1).toString();
            }
            else
            {
              ModelAcc.setFilter("idx="+QString::number(iCat*-1));
              s="["+ModelAcc.record(0).value(3).toString()+"]";
            }
            Painter.drawText(Rect,Qt::AlignHCenter|Qt::AlignTop,s);
          }

          //amount || balance
          if ((iCol==9)||(iCol==11))
          {
            QPen Pen;
            double d=s.toDouble();
            if (d<0)
            {
              s="("+ QString::number(d*-1.0,'f',2) + ")";
              Pen.setColor(Qt::red);
            }
            else
              s=QString::number(d,'f',2);
            Painter.save();
            Painter.setPen(Pen);
            Painter.drawText(Rect,Qt::AlignRight|Qt::AlignTop,s);
            Painter.restore();
          }
        }
      }

      //Writes Lines (Vertical)
      int iX=0;
      int iY1=QFontMetrics(FontTitle).height()+iFontHeight*3;
      int iY2=iY1+(iPageRows+1)*iFontHeight;
      foreach (int iCol, mapColumns)
      {
        Painter.drawLine(iX,iY1,iX,iY2);
        iX+=lstMaxCol.at(iCol)*iFontWidth;
      }
      Painter.drawLine(iX,iY1,iX,iY2);

      //Writes Lines (Horizontal)
      iY1=QFontMetrics(FontTitle).height()+iFontHeight*3;
      Painter.drawLine(0,iY1,iX,iY1);
      iY1+=iFontHeight;
      Painter.drawLine(0,iY1,iX,iY1);
      iY1+=iFontHeight*iPageRows;
      Painter.drawLine(0,iY1,iX,iY1);
    }
  }
}

void CMainWindow::actionExport()
{
  //Get file name
  QString sFile;
  sFile=QFileDialog::getSaveFileName(this,tr("Create File"),
                                     QStandardPaths::writableLocation (QStandardPaths::DocumentsLocation),
                                     "Tab Text (*.txt)");
  if (sFile.isEmpty()) return;

  //Create file
  QFile File(sFile);
  File.open(QFile::WriteOnly);
  QTextStream Stream(&File);

  //Initiate Models
  QSqlTableModel ModelAcc;
  ModelAcc.setTable("account");
  ModelAcc.select();

  QSqlTableModel ModelCat;
  ModelCat.setTable("category");
  ModelCat.select();

  //Initiate pointers
  CTransView *pTransView=qobject_cast<CTransView *>(m_lstForms.last());
  QTableView *pTableView=pTransView->tableView();
  QSqlTableModel *pModel=qobject_cast<QSqlTableModel *>(pTableView->model());

  //Initiate variables
  QString s;
  int iNumCols=pModel->columnCount();
  int iNumRows=g_rowCount(*pModel);

  //Write Title
  for (int iCol=0;iCol<iNumCols;iCol++)
    if ((iCol!=0)&&(iCol!=1)&&(iCol!=10)&&(iCol!=12)&&(iCol!=13)&&(iCol!=14))
    {
      Stream<<pModel->headerData(iCol,Qt::Horizontal).toString();
      if (iCol!=11) Stream<<QChar(0x09);
    }
  Stream<<Qt::endl;

  //Writes data rows
  for (int iRow=0;iRow<iNumRows;iRow++)
  {
    for (int iCol=0;iCol<iNumCols;iCol++)
    {
      s=pModel->record(iRow).value(iCol).toString();

      //status || class || payee || account || cheque
      if ((iCol==2)||(iCol==4)||(iCol==5)||(iCol==6)||(iCol==8))
      {
        Stream<<s<<QChar(0x09);
      }

      //date
      if (iCol==3)
      {
        QString sDateFormat;
        if (g_dateFormat().left(2)=="dd")
          sDateFormat="dd.MM.yy";
        else
          sDateFormat="MM.dd.yy";

        QDate Date=QDate::fromJulianDay(s.toInt());
        Stream<<Date.toString(sDateFormat)<<QChar(0x09);
      }

      //category
      if (iCol==7)
      {
        int iCat=s.toInt();
        if ((iCat>=0)||(iCat==ID_SPLIT))
        {
          ModelCat.setFilter("idx="+QString::number(iCat));
          s=ModelCat.record(0).value(1).toString();
        }
        else
        {
          ModelAcc.setFilter("idx="+QString::number(iCat*-1));
          s="["+ModelAcc.record(0).value(3).toString()+"]";
        }
        Stream<<s<<QChar(0x09);
      }

      //amount || balance
      if ((iCol==9)||(iCol==11))
      {
        double d=s.toDouble();
        s=QString::number(d,'f',2);
        Stream<<s;
        if (iCol!=11) Stream<<QChar(0x09);
      }
    }

    //Finish the line
    Stream<<Qt::endl;
  }

  File.close();
}

void CMainWindow::actionGeneral()
{
  addForm("CGeneralList");
}

void CMainWindow::actionCurrencies()
{
  addForm("CCurrencyEdit");
}

void CMainWindow::actionSchedule()
{
  QSqlTableModel Table;
  Table.setTable("account");
  Table.select();

  if (Table.rowCount()>0)
  {
    addForm("CScheduledTrans");
  }
}

void CMainWindow::actionPurge()
{
  CPurgeTrans PurgeTrans(this);
  if (PurgeTrans.exec()==QDialog::Accepted)
  {
    CTransView *pTransView=qobject_cast<CTransView *>(m_lstForms.last());
    if (PurgeTrans.choice()==0)  pTransView->purgeTransactions(CTransView::PurgeDate,PurgeTrans.date());
    if (PurgeTrans.choice()==1)  pTransView->purgeTransactions(CTransView::PurgeClear);
    if (PurgeTrans.choice()==2)  pTransView->purgeTransactions(CTransView::PurgeRecon);
  }
}

void CMainWindow::actionAdjustment()
{
  bool bOk;
  double dAmount=QInputDialog::getDouble(this,tr("Adjutment of final balance"),
                          tr("Adjust initial balance to get the following final balance:"),
                          0.0,-MAX_AMOUNT,MAX_AMOUNT,2,&bOk);
  if (bOk)
  {
    CTransView *pTransView=qobject_cast<CTransView *>(m_lstForms.last());
    pTransView->adjustTotal(dAmount);
  }
}

void CMainWindow::actionSafebox()
{
  addForm("CItemView");
}

void CMainWindow::actionBudget()
{
  addForm("CBudgetView");
}

void CMainWindow::actionTransfer()
{
  QSqlTableModel Table;
  Table.setTable("account");
  Table.select();

  if (Table.rowCount()>1)
  {
    addForm("CTransfer");
  }
}

void CMainWindow::actionMore()
{
  QMenu Menu(this);
  Menu.addAction(m_pActionOpen);
  Menu.addAction(m_pActionNew);
  Menu.addAction(m_pActionPassword);
  Menu.addAction(m_pActionSettings);
  Menu.addAction(m_pActionHelp);
  Menu.addAction(m_pActionAbout);

  Menu.exec(mapToGlobal(QPoint((width()-Menu.sizeHint().width())/2,(height()-Menu.sizeHint().height())/2)));
}

void CMainWindow::actionSearch()
{
  g_pMainWindow->addForm("CSearch");
}

void CMainWindow::actionQuit()
{
  close();
}

void CMainWindow::filterMenus(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10,
                              bool b11, bool b12, bool b13, bool b14, bool b15, bool b16, bool b17)
{
#ifdef MOBILE_PLATFORM
  m_pActionPrint->setVisible(b1);
  m_pActionExport->setVisible(b2);
  m_pActionGeneral->setVisible(b3);
  m_pActionCurrencies->setVisible(b4);
  m_pActionSchedule->setVisible(b5);
  m_pActionPurge->setVisible(b6);
  m_pActionAdjustment->setVisible(b7);
  m_pActionSafebox->setVisible(b8);
  m_pActionTransfer->setVisible(b9);
  m_pActionReport->setVisible(b10);
  m_pActionInfo->setVisible(b11);
  m_pActionAdd->setVisible(b12);
  m_pActionDelete->setVisible(b13);
  m_pActionMore->setVisible(b14);
  m_pActionPassword->setVisible(b15);
  m_pActionSettings->setVisible(b15);
  m_pActionBudget->setVisible(b16);
  m_pActionSearch->setVisible(b17);
#else
  m_pActionPrint->setEnabled(b1);
  m_pActionExport->setEnabled(b2);
  m_pActionGeneral->setEnabled(b3);
  m_pActionCurrencies->setEnabled(b4);
  m_pActionSchedule->setEnabled(b5);
  m_pActionPurge->setEnabled(b6);
  m_pActionAdjustment->setEnabled(b7);
  m_pActionSafebox->setEnabled(b8);
  m_pActionTransfer->setEnabled(b9);
  m_pActionReport->setEnabled(b10);
  m_pActionInfo->setEnabled(b11);
  m_pActionAdd->setEnabled(b12);
  m_pActionDelete->setEnabled(b13);
  m_pActionMore->setEnabled(b14);
  m_pActionPassword->setEnabled(b15);
  m_pActionSettings->setEnabled(b15);
  m_pActionOpen->setEnabled(b14);
  m_pActionNew->setEnabled(b14);
  m_pActionBudget->setEnabled(b16);
  m_pActionSearch->setEnabled(b17);
#endif
}

void CMainWindow::filterMenusByForm(const QString& sForm)
{
  QString sTitle="DRF ["+QFileInfo(m_db.databaseName()).baseName()+"] - %1";

  if ((sForm=="CAccountsView")&&(m_lstForms.count()>0))
  {
    if (g_isActiveDatabase())
    {
      filterMenus(false,false,true,true,true,false,false,true,true,true,true,false,false,true,true,true,true);
      setWindowTitle(sTitle.arg(tr("Accounts")));
    }
    else
    {
      filterMenus(false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false);
      setWindowTitle("DRF");
    }
  }

  if (sForm=="CTransViewAd1PA1Pr1")
  {
    filterMenus(true,true,true,true,false,true,true,false,false,false,true,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Transactions")));
  }

  if (sForm=="CTransViewAd1PA0Pr1")
  {
    filterMenus(true,true,true,true,false,false,false,false,false,false,true,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Transactions")));
  }

  if (sForm=="CTransViewAd0PA1Pr1")
  {
    filterMenus(true,true,true,true,false,true,true,false,false,false,true,false,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Transactions")));
  }

  if (sForm=="CTransViewAd0PA0Pr1")
  {
    filterMenus(true,true,true,true,false,false,false,false,false,false,true,false,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Transactions")));
  }

  if (sForm=="CTransViewAd0PA0Pr0")
  {
    filterMenus(false,true,true,true,false,false,false,false,false,false,true,false,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Transactions")));
  }

  if (sForm=="CGeneralList")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("General")));
  }

  if (sForm=="CCurrencyEdit")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Currencies")));
  }

  if (sForm=="CScheduledTrans")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Schedule")));
  }

  if (sForm=="CSettings")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Settings")));
  }

  if (sForm=="CItemView")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,true,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Items")));
  }

  if (sForm=="CItemEdit")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Edit Item")));
  }

  if (sForm=="CItemInfo")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Item Info")));
  }

  if (sForm=="CGroupEdit")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Edit Group")));
  }

  if (sForm=="CTransEdit")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Edit Trans")));
  }

  if (sForm=="CTransInfo")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Trans Info")));
  }

  if (sForm=="CAccountEdit")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Edit Account")));
  }

  if (sForm=="CAccountInfo")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Account Info")));
  }

  if (sForm=="CReport")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Report")));
  }

  if (sForm=="CTransfer")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Transfer")));
  }

  if (sForm=="CSplitTrans")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Split")));
  }

  if (sForm=="CBudgetEdit")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,true,true,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Edit Budget")));
  }

  if (sForm=="CBudgetView")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Budgets")));
  }

  if (sForm=="CBudgetRep")
  {
    filterMenus(false,false,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Budget Rep")));
  }

  if (sForm=="CSearch")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Search")));
  }

  if (sForm=="CHelp")
  {
    filterMenus(false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false);
    setWindowTitle(sTitle.arg(tr("Help")));
  }
}

void CMainWindow::timeoutSchedule()
{
  if (!g_isActiveDatabase()) return;

  bool bChange=false;

  QSqlTableModel ModelSch;
  ModelSch.setTable("schedule");
  ModelSch.setFilter("split<"+QString::number(SPLIT));
  ModelSch.select();

  QSqlTableModel ModelTrans;
  ModelTrans.setTable("trans");
  ModelTrans.select();

  QDate DateToday=QDate::currentDate();

  int iRowCount=g_rowCount(ModelSch);
  for (int i=0;i<iRowCount; i++)
  {
    //Read parameters
    QSqlRecord RecordSch=ModelSch.record(i);
    int iAcc=RecordSch.value(1).toInt();
    int iCat=RecordSch.value(3).toInt();
    QDate Date=QDate::fromJulianDay(RecordSch.value(6).toInt());
    int iDayMonth=RecordSch.value(7).toInt();
    int iFrequency=RecordSch.value(8).toInt();
    int iUnit=RecordSch.value(10).toInt();

    //Read Limits
    QStringList lsts=RecordSch.value(13).toString().split('#');
    int iLimits=lsts.at(0).toInt();
    QDate DateLimit=QDate::fromJulianDay(lsts.at(1).toInt());
    int iTimesLimit=lsts.at(1).toInt();

    while ((Date<=DateToday) && ((iLimits==0) || ((iLimits==1)&&(Date<=DateLimit)) || ((iLimits==2)&&(iTimesLimit>0))))
    {
      //Create New Transaction
      int iId=g_generateId("trans");
      ModelTrans.insertRow(0);
      ModelTrans.setData(ModelTrans.index(0,0),iId);
      ModelTrans.submitAll();
      ModelTrans.setFilter("idx="+QString::number(iId));

      QSqlRecord RecordTrans=ModelTrans.record(0);

      //Set Icon based on category without considering [account]
      if ((iCat>0)||(iCat==ID_SPLIT))
      {
        RecordTrans.setValue(1,iCat);
        RecordTrans.setValue(13,"");
      }
      else
      {
        RecordTrans.setValue(1,0);
        RecordTrans.setValue(13,QString::number(g_transferRate(iAcc,iCat*-1),'f',RATE_PREC));
      }

      //Set rest of fields for transaction
      RecordTrans.setValue(2,0);
      RecordTrans.setValue(3,Date.toJulianDay());
      RecordTrans.setValue(4,RecordSch.value(4));
      RecordTrans.setValue(5,RecordSch.value(2));
      RecordTrans.setValue(6,iAcc);
      RecordTrans.setValue(7,iCat);
      RecordTrans.setValue(9,RecordSch.value(5));
      RecordTrans.setValue(10,RecordSch.value(9));
      RecordTrans.setValue(12,0);
      RecordTrans.setValue(14,0);
      ModelTrans.setRecord(0,RecordTrans);
      ModelTrans.submitAll();

      //Create Split Transactions in case it is ID_SPLIT
      if (iCat==ID_SPLIT)
      {
        int iSplit=RecordSch.value(12).toInt();

        QSqlTableModel ModelSch1;
        ModelSch1.setTable("schedule");
        ModelSch1.setFilter("split="+QString::number(SPLIT+iSplit));
        ModelSch1.select();

        QStandardItemModel ModelSplit;
        int iRowCount=g_rowCount(ModelSch1);
        for (int iRow=0;iRow<iRowCount;iRow++)
        {
          int iCat1=ModelSch1.record(iRow).value(3).toInt();
          ModelSplit.setItem(iRow,0,new QStandardItem(ModelSch1.record(iRow).value(3).toString())); //Category
          ModelSplit.setItem(iRow,1,new QStandardItem(ModelSch1.record(iRow).value(4).toString())); //Class
          ModelSplit.setItem(iRow,2,new QStandardItem(ModelSch1.record(iRow).value(5).toString())); //Amount
          ModelSplit.setItem(iRow,3,new QStandardItem(ModelSch1.record(iRow).value(9).toString())); //Note
          if ((iCat1<0)&&(iCat1>=-ID_MAX))
            ModelSplit.setItem(iRow,4,new QStandardItem(QString::number(g_transferRate(iAcc,iCat1*-1),'f',RATE_PREC)));
          else
            ModelSplit.setItem(iRow,4,new QStandardItem(""));
        }
        g_createSplitTrans(RecordTrans.value(0),&ModelSplit);
      }

      //Evaluate the rate in case it is a transfer transaction
      if ((iCat<0)&&(iCat>=-ID_MAX))
      {
        g_transfer(RecordTrans.value(0));
      }

      //Change the next Scheduled Date
      if (iUnit==0)
      {
        Date=Date.addDays(iFrequency);
      }
      else if (iUnit==1)
      {
        Date=Date.addDays(iFrequency*7);
      }
      else if (iUnit==2)
      {
        Date=Date.addMonths(iFrequency);
        while ((Date.day()!=Date.daysInMonth()) &&
              (Date.day()<iDayMonth))
                Date=Date.addDays(1);
      }

      //Update TimesLimit in case this has been set as limit
      if (iLimits==2) iTimesLimit--;

      bChange=true;
    }

    if (bChange)
    {
      RecordSch.setValue(6,Date.toJulianDay());
      if (iLimits==2) RecordSch.setValue(13,"2#"+QString::number(iTimesLimit));
      ModelSch.setRecord(i,RecordSch);
      ModelSch.submitAll();
    }
  }
  if (bChange) emit newTransaction();
}

void CMainWindow::timeoutBudget()
{
  if (!g_isActiveDatabase()) return;

  bool bChange=false;

  QSqlTableModel ModelBudget;
  ModelBudget.setTable("budget");
  ModelBudget.select();

  QDate DateToday=QDate::currentDate();

  int iRowCount=g_rowCount(ModelBudget);
  for (int i=0;i<iRowCount; i++)
  {
    QSqlRecord Record=ModelBudget.record(i);
    Recurrent iRecurrent=static_cast<Recurrent>(Record.value(4).toInt());

    //Review and update records when End Date for recurrent budgets is over
    while ((iRecurrent>0) && (Record.value(3).toInt()<DateToday.toJulianDay()))
    {
      QDate Date1=QDate::fromJulianDay(Record.value(2).toInt());
      QDate Date2=QDate::fromJulianDay(Record.value(3).toInt());
      Date1=Date2.addDays(1);
      Date2=g_endRecurrentDate(Date1,iRecurrent);

      Record.setValue(2,Date1.toJulianDay());
      Record.setValue(3,Date2.toJulianDay());
      ModelBudget.setRecord(i,Record);
      ModelBudget.submitAll();
      bChange=true;
    }
  }

  if (bChange) emit updateBudget();
}

void CMainWindow::newOpenAction(FileAction iAction)
{
  QString sFile;

  //Open the File Dialog New
  if (iAction==ActionNew)
  {
    sFile=QFileDialog::getSaveFileName(this,tr("Create New Database"),
                                       QStandardPaths::writableLocation (QStandardPaths::DocumentsLocation),
                                       qApp->applicationName()+" (*.drf)",0,QFileDialog::DontConfirmOverwrite);
    if (sFile.isEmpty()) return;
    if (!sFile.endsWith(".drf",Qt::CaseInsensitive)) sFile.append(".drf");
    if (QFile::exists(sFile))
    {
      g_messageWarning(this,tr("Database"),tr("The file already exist"));
      return;
    }
  }

  //Open the File Dialog Open
  if (iAction==ActionOpen)
  {
    sFile=QFileDialog::getOpenFileName(this,tr("Open Existing Database"),
                                       QStandardPaths::writableLocation (QStandardPaths::DocumentsLocation),
                                       qApp->applicationName()+" (*.drf)");
    if (sFile.isEmpty()) return;
    if (!QFile::exists(sFile))
    {
      g_messageWarning(this,tr("Database"),tr("The file doesn't exist"));
      return;
    }
  }

  //Inform that database is closing for AccountView to save state
  emit beforeChangeDatabase();
  m_db.close();
  encryptDatabase(m_db.databaseName());

  //Create tables for New Database
  if (iAction==ActionNew)
  {
    //Setup progress bar and cursor
    QProgressBar ProgressBar(this);
    ProgressBar.setAlignment(Qt::AlignCenter);
    ProgressBar.setFormat("%p%");
    ProgressBar.setMinimum(0);
    ProgressBar.setMaximum(11);
    ProgressBar.setValue(0);
    ProgressBar.resize(width()/2,ProgressBar.height());
    ProgressBar.move((width()-ProgressBar.width())/2,(height()-ProgressBar.height())/2);
    ProgressBar.show();
    g_waitingStatus(true);

    //Create database file
    bool bError=false;
    m_db.setDatabaseName(QDir::toNativeSeparators(sFile));
    bError|=!m_db.open();
    ProgressBar.setValue(1);

    //Create database tables
    QSqlQuery Query;
    Query.setForwardOnly(true);
    bError|=!Query.exec("CREATE TABLE bank (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, address TEXT, other TEXT, note TEXT, aux TEXT);");
    bError|=!Query.exec("INSERT INTO bank VALUES (0,'',NULL,NULL,NULL,NULL);");
    ProgressBar.setValue(2);

    bError|=!Query.exec("CREATE TABLE category (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, icon TEXT, budget NUMERIC, aux TEXT);");
    bError|=!Query.exec("INSERT INTO category VALUES (0,'',NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (1,"+tr("'Groceries'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (2,"+tr("'Education'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (3,"+tr("'Entertaiment'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (4,"+tr("'Health'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (5,"+tr("'Salary'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (6,"+tr("'Home'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (7,"+tr("'Sport'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES (8,"+tr("'Travel'")+",NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO category VALUES ("+QString::number(ID_SPLIT)+","+SPLIT_QTD+",NULL,NULL,NULL);");
    ProgressBar.setValue(3);

    bError|=!Query.exec("CREATE TABLE class (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, aux TEXT);");
    bError|=!Query.exec("INSERT INTO class VALUES (0,'',NULL);");
    bError|=!Query.exec("INSERT INTO class VALUES (1,"+tr("'Personal'")+",NULL);");
    bError|=!Query.exec("INSERT INTO class VALUES (2,"+tr("'Business'")+",NULL);");
    ProgressBar.setValue(4);

    bError|=!Query.exec("CREATE TABLE payee (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, aux TEXT);");
    bError|=!Query.exec("INSERT INTO payee VALUES (0,'',NULL);");
    bError|=!Query.exec("INSERT INTO payee VALUES ("+QString::number(ID_TRANSFER)+","+TRANSFER_QTD+",NULL);");
    ProgressBar.setValue(5);

    bError|=!Query.exec("CREATE TABLE type (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, aux TEXT);");
    bError|=!Query.exec("INSERT INTO type VALUES (0,'',NULL);");
    bError|=!Query.exec("INSERT INTO type VALUES (1,"+tr("'Checking'")+",NULL);");
    bError|=!Query.exec("INSERT INTO type VALUES (2,"+tr("'Credit card'")+",NULL);");
    bError|=!Query.exec("INSERT INTO type VALUES (3,"+tr("'Savings'")+",NULL);");
    bError|=!Query.exec("INSERT INTO type VALUES (4,"+tr("'Cash'")+",NULL);");
    ProgressBar.setValue(6);

    bError|=!Query.exec("CREATE TABLE currency (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, rate NUMERIC, symbol TEXT);");
    bError|=!Query.exec("INSERT INTO currency VALUES (0,'US Dollar',1.0,'$');");
    ProgressBar.setValue(7);

    bError|=!Query.exec("CREATE TABLE schedule (idx INTEGER PRIMARY KEY, accountid NUMERIC, payeeid NUMERIC, categoryid NUMERIC, classid NUMERIC, amount NUMERIC, date NUMERIC, daymonth NUMERIC, frequency NUMERIC, note TEXT, unit TEXT, aux TEXT, split NUMERIC, limits TEXT);");

    bError|=!Query.exec("CREATE TABLE settings (idx INTEGER PRIMARY KEY, hash BLOB, code BLOB, acc_list BLOB, acc_status NUMERIC, acc_type NUMERIC, acc_bank NUMERIC, acc_curr NUMERIC, trans_lista BLOB, trans_listp BLOB, trans_listc BLOB, sch_list BLOB, rep_type NUMERIC, rep_class NUMERIC, rep_columns NUMERIC, rep_inex NUMERIC, rep_period NUMERIC, rep_start NUMERIC, rep_end NUMERIC, set_font NUMERIC, set_aux NUMERIC, set_datefmt TEXT, item_group NUMERIC, rep_curr NUMERIC, bud_filter NUMERIC, acc_sort TEXT, trans_sort TEXT, sch_sort TEXT);");
    bError|=!Query.exec("INSERT INTO settings VALUES (0,NULL,NULL,NULL,0,"+QString::number(ID_ANY)+","+QString::number(ID_ANY)+","+QString::number(ID_ANY)+",NULL,NULL,NULL,NULL,0,"+QString::number(ID_ANY)+",0,0,0,0,0,0,"+QString::number(DB_VER)+",'MM.dd.yy',0,0,"+QString::number(ID_ANY)+",'3,0','3,0','1,0');");
    ProgressBar.setValue(8);

    bError|=!Query.exec("CREATE TABLE sbgroup (idx INTEGER PRIMARY KEY, name TEXT UNIQUE, field1 TEXT, field2 TEXT, field3 TEXT, field4 TEXT, field5 TEXT, field6 TEXT, field7 TEXT, field8 TEXT);");
    bError|=!Query.exec("INSERT INTO sbgroup VALUES (0,"+tr("'Emails'")+","+tr("'email'")+","+tr("'username'")+","+tr("'pass'")+","+tr("'server'")+",NULL,NULL,NULL,NULL);");
    bError|=!Query.exec("INSERT INTO sbgroup VALUES (1,"+tr("'Websites'")+","+tr("'url'")+","+tr("'username'")+","+tr("'pass'")+",NULL,NULL,NULL,NULL,NULL);");
    ProgressBar.setValue(9);

    bError|=!Query.exec("CREATE TABLE account (idx INTEGER PRIMARY KEY, icon TEXT, typeid NUMERIC, name TEXT UNIQUE, bankid NUMERIC, currencyid NUMERIC, number TEXT, holder TEXT, inbalance NUMERIC, note TEXT, total NUMERIC, status NUMERIC);");
    bError|=!Query.exec("CREATE TABLE trans (idx INTEGER PRIMARY KEY, iconid NUMERIC, status NUMERIC, date NUMERIC, classid NUMERIC, payeeid NUMERIC, accountid NUMERIC, categoryid NUMERIC, cheque NUMERIC, amount NUMERIC, note TEXT, balance NUMERIC, transfer NUMERIC, aux TEXT,split NUMERIC);");
    bError|=!Query.exec("CREATE TABLE sbitem (idx INTEGER PRIMARY KEY, groupid NUMERIC, field1 TEXT, field2 TEXT, field3 TEXT, field4 TEXT, field5 TEXT, field6 TEXT, field7 TEXT, field8 TEXT, aux TEXT);");
    ProgressBar.setValue(10);

    bError|=!Query.exec("CREATE TABLE budget (idx INTEGER PRIMARY KEY, name TEXT, start NUMERIC, end NUMERIC, recurrent NUMERIC, currencyid NUMERIC, classid NUMERIC);");
    bError|=!Query.exec("CREATE TABLE budgetcat (idx INTEGER PRIMARY KEY, budgetid NUMERIC, categoryid NUMERIC, amount NUMERIC);");
    ProgressBar.setValue(11);

    g_waitingStatus(false);

    //Close database and finish progress bar and cursor
    if (bError)
    {
      QFile::remove(sFile);
      sFile.clear();
      g_messageWarning(this,tr("Create database"),tr("Error reported by the driver while creating the database."));
    }
    m_db.close();
  }

  //Reset MainWindow for new database and inform the change to update AccountView
  openDatabase(sFile);
  emit afterChangeDatabase();
}

void CMainWindow::databaseChanged()
{
  emit afterChangeDatabase();
}

bool CMainWindow::upgradeDatabase()
{
  //Initiate query variable for all steps
  QString sMessage;
  QSqlQuery Query;
  Query.setForwardOnly(true);

  //Retrieves the database and application versions
  Query.exec("SELECT set_aux FROM settings WHERE idx=0;");
  if (!Query.next()) return false;
  int iDBVersion=Query.value(0).toInt();
  Query.finish();
  int iAppVersion=DB_VER;

  //Database is same version as application
  if (iDBVersion==iAppVersion) return true;

  //Database is newer version than application
  if (iDBVersion>iAppVersion)
  {
    sMessage=tr("The database's version isn't supported by your current package.\nYou will need to upgrade your application.");
    g_messageWarning(this,tr("Validate database"),sMessage);
    return false;
  }

  //Request confirmation to upgrade the database
  sMessage=tr("Your database needs to be upgraded to version %1. Once the upgrade is committed, it won't be readable by older applications.\nDo you want to continue?");
  if (g_messageYesNo(this,tr("Confirmation"),sMessage.arg(iAppVersion))==QMessageBox::No) return false;

  //BEGIN TRANSACTION
  Query.exec("BEGIN TRANSACTION;");
  bool bError=false;

  /***************************************************************************************************************************************/
  //AVAILABLE ACTIONS:
  //ALTER TABLE table-name RENAME TO new-table-name
  //ALTER TABLE table-name ADD COLUMN column-name TEXT/NUMERIC

  //Upgrade database: 0 to 104
  if (iDBVersion<104)
  {
    bError|=!Query.exec("ALTER TABLE settings ADD COLUMN rep_curr NUMERIC;");
    bError|=!Query.exec("UPDATE settings SET rep_curr=0 WHERE idx=0;");
  }

  //Upgrade database: 104 to 110
  if (iDBVersion<110)
  {
    bError|=!Query.exec("CREATE TABLE budget (idx INTEGER PRIMARY KEY, name TEXT, start NUMERIC, end NUMERIC, recurrent NUMERIC, currencyid NUMERIC, classid NUMERIC);");
    bError|=!Query.exec("CREATE TABLE budgetcat (idx INTEGER PRIMARY KEY, budgetid NUMERIC, categoryid NUMERIC, amount NUMERIC);");

    bError|=!Query.exec("ALTER TABLE settings ADD COLUMN bud_filter NUMERIC;");
    bError|=!Query.exec("UPDATE settings SET bud_filter="+QString::number(ID_ANY)+" WHERE idx=0;");
  }

  //Upgrade database: 111 to 112
  if (iDBVersion<112)
  {
    bError|=!Query.exec("ALTER TABLE settings ADD COLUMN acc_sort TEXT;");
    bError|=!Query.exec("ALTER TABLE settings ADD COLUMN trans_sort TEXT;");
    bError|=!Query.exec("ALTER TABLE settings ADD COLUMN sch_sort TEXT;");
    bError|=!Query.exec("UPDATE settings SET acc_sort='3,0' WHERE idx=0;");
    bError|=!Query.exec("UPDATE settings SET trans_sort='3,0' WHERE idx=0;");
    bError|=!Query.exec("UPDATE settings SET sch_sort='1,0' WHERE idx=0;");
    bError|=!Query.exec("UPDATE settings SET rep_period=0 WHERE idx=0;");
  }

  //Upgrade database: 112 to 113
  if (iDBVersion<113)
  {
    bError|=!Query.exec("UPDATE payee SET name="+QString(TRANSFER_QTD)+" WHERE idx="+QString::number(ID_TRANSFER)+";");
    bError|=!Query.exec("UPDATE category SET name="+QString(SPLIT_QTD)+" WHERE idx="+QString::number(ID_SPLIT)+";");
  }

  //Upgrade database: 113 to 200
  if (iDBVersion<200)
  {
    bError|=!Query.exec("ALTER TABLE schedule ADD COLUMN limits TEXT;");
    bError|=!Query.exec("ALTER TABLE account ADD COLUMN status NUMERIC;");
    bError|=!Query.exec("UPDATE schedule SET limits='0#';");
    bError|=!Query.exec("UPDATE account SET status=1;");
  }

  /***************************************************************************************************************************************/

  //Rollback to the original state in case errors were detected
  if (bError)
  {
    Query.exec("ROLLBACK TRANSACTION;");
    g_messageWarning(this,tr("Upgrade database"),tr("There was a problem while upgrading your database.\nNo changes were committed."));
    return false;
  }

  //Update the database version
  Query.exec("UPDATE settings SET set_aux="+QString::number(DB_VER)+" WHERE idx=0;");

  //COMMIT TRANSACTION: After all steps are succesfully completed
  Query.exec("COMMIT TRANSACTION;");

  return true;
}
