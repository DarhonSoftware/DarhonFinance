#include <QtGui>
#include <QtWidgets>
#include "ui_dr_about.h"
#include "dr_about.h"
#include "dr_profile.h"
#ifdef SILVER_VERSION
#include "dr_version.h"
#endif

#define SILVER_TEXT QT_TRANSLATE_NOOP("Global","Silver version")
#define GOLD_TEXT   QT_TRANSLATE_NOOP("Global","Gold version")

CAbout::CAbout(const QString& sIconAbout, const QString& sIconDR, const QString& sAppName,
               const QString& sCaption, const QString& sDescription, const QString& sCopyright,
               const QString& sPrivacy, QWidget *pWidget) :
    QDialog(pWidget),
    ui(new Ui::CAbout)
{
  ui->setupUi(this);

  //Setup children
  ui->pLEActCode->setInputMethodHints(Qt::ImhNoAutoUppercase);
  ui->pLPrivacy->setOpenExternalLinks(true);

  //Set visibility
  ui->pLEActCode->hide();
  ui->pPBClose->hide();
  if (sPrivacy.isEmpty()) ui->pLPrivacy->hide();
  ui->pPBEdit->hide();

  //Set icons
  ui->pLAboutIcon->setPixmap(QPixmap(sIconAbout));
  ui->pLDRIcon->setPixmap(QPixmap(sIconDR));

  //Writes information
  ui->pLTitle->setText(QString("<b>%1 %2</b>").arg(sAppName,qApp->applicationVersion()));
  ui->pLQtVersion->setText(QString("(Qt - %1)").arg(QT_VERSION_STR));
  ui->pLCaption->setText(sCaption);
  ui->pLDescription->setText(sDescription);
  ui->pLCopyright->setText(sCopyright);
  ui->pLWebsite->setText(tr("Help & Support") + " - " + QCoreApplication::organizationDomain());
  ui->pLPrivacy->setText(sPrivacy);

  //Load activation code from file
  ui->pLVersion->setText(QCoreApplication::translate("Global",GOLD_TEXT));

  //Get application font for reference
  QFont Font=qApp->font();
  int iSize=QFontInfo(Font).pixelSize();
  int iSizeSmall, iSizeNormal, iSizeLarge;

#ifdef MOBILE_PLATFORM
  iSizeSmall=iSize-3*iSize/10;
  iSizeNormal=iSize-2*iSize/10;
  iSizeLarge=iSize;
#else
  iSizeSmall=iSize-2*iSize/10;
  iSizeNormal=iSize;
  iSizeLarge=iSize+2*iSize/10;
#endif

  //Set font for first line
  Font.setPixelSize(iSizeLarge);
  ui->pLTitle->setFont(Font);

  //Set font for middle line
  Font.setPixelSize(iSizeNormal);
  ui->pLQtVersion->setFont(Font);
  ui->pLCaption->setFont(Font);
  ui->pLDescription->setFont(Font);
  ui->pLEActCode->setFont(Font);
  ui->pLVersion->setFont(Font);

  //Set font for last line
  Font.setPixelSize(iSizeSmall);
  ui->pLAppCode->setFont(Font);
  ui->pLCopyright->setFont(Font);
  ui->pLWebsite->setFont(Font);
  ui->pLPrivacy->setFont(Font);

  //Resize the window to optimaize the platform style
  adjustSize();
}

CAbout::~CAbout()
{
  delete ui;
}


