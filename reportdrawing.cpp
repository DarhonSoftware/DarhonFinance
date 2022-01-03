#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "reportdrawing.h"
#include "global.h"

#define NUM_COLORS        24
#define OTHERS						0.03
#define FONT_FAMILY				"Arial"
#define ALPHA							180
#define HEIGHT_CORRECTION 6
#define PI                3.1416

int aiRed[NUM_COLORS]=   {0,0,0,128,128,0,0,255,255,255,255,184,153,51,145,51,204,153,153,255,0,255,87,255};
int aiGreen[NUM_COLORS]= {128,255,128,0,128,0,255,0,0,255,51,71,102,163,80,204,204,204,153,128,102,66,157,181};
int aiBlue[NUM_COLORS]=  {0,255,128,128,0,255,0,0,255,0,102,0,204,204,80,102,0,255,255,128,204,14,28,21};

CReportDrawing::CReportDrawing(QWidget *pWidget) : QLabel(pWidget)
{
  if (g_resolution()==ResSmall)
  {
    m_iFontSize=8;
    m_iFontLabelSize=5;
    m_iTitleMargin=6;
  }
  else
  {
    m_iFontSize=6;
    m_iFontLabelSize=3;
    m_iTitleMargin=4;
  }

  m_pModel=0;
  m_iBarWidth=m_iFontLabelSize*3;
  m_iMarginPie=20;
  m_iRectPie=100-m_iMarginPie*2-m_iFontSize;
  m_iRadiusLabelA=15;
  m_iRadiusLabelB=m_iRectPie/2;
  m_iHMarginHis=5;
  m_iVMarginHis=static_cast<int>(m_iFontLabelSize*2.5);
}

CReportDrawing::~CReportDrawing()
{
}

void CReportDrawing::drawPie()
{
  //Calculate the number of Pies and exit in case threre are none
  int iNumRows=m_pModel->rowCount()-1;
  if (iNumRows==0) return;

  //Get the number of columns
  int iNumCols=m_pModel->columnCount()-2;

  //Read measurements of drawing area
  const int iHeightPix = this->height()-HEIGHT_CORRECTION;

  //Initializes Painter
  QPixmap Pixmap(iHeightPix*iNumCols,iHeightPix);
  Pixmap.fill(palette().color(QPalette::Base));
  QPainter Painter(&Pixmap);

  //Initialize Title font
  QFont FontTitle(FONT_FAMILY);
  FontTitle.setBold(false);
  FontTitle.setWeight(QFont::Light);
  FontTitle.setPixelSize(m_iFontSize);

  //Initialize Label font
  QFont FontLabel(FONT_FAMILY);
  FontLabel.setBold(false);
  FontLabel.setWeight(QFont::Light);
  FontLabel.setPixelSize(m_iFontLabelSize);

  //Set Painter
  Painter.setWindow(0,0,100,100);
  Painter.setViewport(0,0,iHeightPix,iHeightPix);
  QColor ColorText=palette().color(QPalette::Text);

  //Draw one Pie per column in the Model
  for (int iCol=0;iCol<iNumCols;iCol++)
  {
    //Set font & draw Title
    Painter.setFont(FontTitle);
    Painter.setPen(ColorText);
    Painter.drawText(0,m_iTitleMargin,100,m_iFontSize+1,Qt::AlignHCenter|Qt::AlignTop,m_pModel->horizontalHeaderItem(iCol+2)->text());

    //Calculate the absolute Total for the column
    double dTotalVal=0.0;
    for (int iRow=0; iRow<iNumRows; iRow++)
      dTotalVal+=qAbs(m_pModel->item(iRow,iCol+2)->text().toDouble());

    //Set Pen for Pie
    Painter.setPen(Qt::NoPen);

    int iAng1=0;
    int iAng2=0;
    double dSumVal=0.0;
    for (int iRow=0;iRow<iNumRows;++iRow)
    {
      double dVal=qAbs(m_pModel->item(iRow,iCol+2)->text().toDouble());

      //Min size of each slice = OTHERS
      if ((dTotalVal!=0.0)&&(dVal/dTotalVal>OTHERS))
      {
        //Evaluate Span Angle and Middle Angle
        iAng2=static_cast<int>(dVal/dTotalVal*5760.0);
        dSumVal+=dVal;
        if (dSumVal==dTotalVal) iAng2=5760-iAng1;

        //Draw color slice
        Painter.setBrush(QBrush(QColor(aiRed[iRow % (NUM_COLORS-1)],aiGreen[iRow % (NUM_COLORS-1)],aiBlue[iRow % (NUM_COLORS-1)],ALPHA)));
        Painter.drawPie(m_iMarginPie+m_iFontSize/2,m_iMarginPie+m_iFontSize,m_iRectPie,m_iRectPie,iAng1,iAng2);
        iAng1+=iAng2;
      }
    }

    //Draw slice OTHER
    if (iAng1<5760)
    {
      iAng2=5760-iAng1;
      Painter.setBrush(QBrush(QColor(aiRed[NUM_COLORS-1],aiGreen[NUM_COLORS-1],aiBlue[NUM_COLORS-1],ALPHA)));
      Painter.drawPie(m_iMarginPie+m_iFontSize/2,m_iMarginPie+m_iFontSize,m_iRectPie,m_iRectPie,iAng1,iAng2);
    }

    //Draw labels
    Painter.setFont(FontLabel);
    Painter.setPen(ColorText);

    iAng1=0.0;
    dSumVal=0.0;
    int iSumPercentage=0;
    for (int iRow=0;iRow<iNumRows;++iRow)
    {
      double dVal=qAbs(m_pModel->item(iRow,iCol+2)->text().toDouble());

      //Min size of each slice = OTHERS
      if ((dTotalVal!=0)&&(dVal/dTotalVal>OTHERS))
      {
        //Evaluate Span Angle and Middle Angle
        iAng2=static_cast<int>(dVal/dTotalVal*5760.0);
        dSumVal+=dVal;
        if (dSumVal==dTotalVal) iAng2=5760-iAng1;
        int iAngMid=iAng1+iAng2/2;

        //Format the text and get the metrics
        iSumPercentage+=dVal/dTotalVal*100.0;
        QString s=m_pModel->item(iRow,1)->text()+" "+QString::number(static_cast<int>(dVal/dTotalVal*100.0))+"%";
        QRect RectLabel=Painter.boundingRect(0,0,100,100,Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,s);

        //Evaluate angule and radius for labels
        double dAngMid=static_cast<double>(iAngMid/16.0*PI/180.0);
        int iRadius=static_cast<int>((m_iRadiusLabelA*m_iRadiusLabelB)/sqrt(pow(m_iRadiusLabelB*cos(dAngMid),2)+pow(m_iRadiusLabelA*sin(dAngMid),2)));

        //Calculate the x,y coordinate and draw the text
        int iX1=0;
        if ((iAngMid>90*16)&&(iAngMid<270*16)) iX1=RectLabel.width();
        int iX=static_cast<int>(iRadius*cos(dAngMid)+50-iX1);
        if (iX<0) iX=0;
        int iY=static_cast<int>(50-iRadius*sin(dAngMid));
        Painter.drawText(iX,iY,50-m_iRadiusLabelA,m_iFontLabelSize+1,Qt::AlignLeft|Qt::AlignTop,s);
        iAng1+=iAng2;
      }
    }

    //Draw the label OTHER
    if (iAng1<5760)
    {
      iAng2=5760-iAng1;
      int iAngMid=iAng1+iAng2/2;

      //Format the text and get the metrics
      QString s=tr("Others ")+QString::number(100-iSumPercentage)+"%";
      QRect RectLabel=Painter.boundingRect(0,0,100,100,Qt::AlignLeft|Qt::AlignTop|Qt::TextSingleLine,s);

      //Evaluate angule and radius for labels
      double dAngMid=static_cast<double>(iAngMid/16.0*PI/180.0);
      int iRadius=static_cast<int>((m_iRadiusLabelA*m_iRadiusLabelB)/sqrt(pow(m_iRadiusLabelB*cos(dAngMid),2)+pow(m_iRadiusLabelA*sin(dAngMid),2)));

      //Calculate the x,y coordinate and draw the text
      int iX1=0;
      if ((iAngMid>90*16)&&(iAngMid<270*16)) iX1=RectLabel.width();
      int iX=static_cast<int>(iRadius*cos(dAngMid)+50-iX1);
      if (iX<0) iX=0;
      int iY=static_cast<int>(50-iRadius*sin(dAngMid));
      Painter.drawText(iX,iY,50-m_iRadiusLabelA,m_iFontLabelSize+1,Qt::AlignLeft|Qt::AlignTop,s);
    }

    //Move the axe to the next Pie
    Painter.translate(100,0);
  }
  setPixmap(Pixmap);
}

void CReportDrawing::drawHistogram()
{
  //Calculate the number of Bars and exit in case threre are none
  int iNumBars=m_pModel->rowCount()-1;
  if (iNumBars==0) return;

  //Get the number of columns
  int iNumCols;
  if (m_pModel->columnCount()==3)
    iNumCols=1;
  else
    iNumCols=m_pModel->columnCount()-3;

  //Read measurements of drawing area
  const int iHeightPix = this->height()-HEIGHT_CORRECTION;

  //Calculate the parameters to be used during drawing
  double dMaxVal=0.0;
  double dMinVal=0.0;
  for (int iCol=0;iCol<iNumCols;iCol++)
    for (int iRow=0; iRow<iNumBars; iRow++)
    {
      double dVal=m_pModel->item(iRow,iCol+2)->text().toDouble();
      if (dVal>dMaxVal) dMaxVal=dVal;
      if (dVal<dMinVal) dMinVal=dVal;
    }
  double dUnit=0;
  int iCenter=50;
  if ((dMaxVal+qAbs(dMinVal))!=0.0)
  {
    dUnit=(100-m_iVMarginHis*2-m_iFontSize-m_iTitleMargin)/(dMaxVal+qAbs(dMinVal));
    iCenter=static_cast<int>(m_iFontSize+m_iVMarginHis+dMaxVal*dUnit);
  }
  int iWidth=qMax(100,m_iBarWidth*iNumBars+m_iHMarginHis*2);

  //Initializes Painter
  QPixmap Pixmap(qRound(static_cast<double>(iHeightPix)*(static_cast<double>(iWidth)/100.0)*static_cast<double>(iNumCols)),iHeightPix);
  Pixmap.fill(palette().color(QPalette::Base));
  QPainter Painter(&Pixmap);

  //Initialize Title font
  QFont FontTitle(FONT_FAMILY);
  FontTitle.setBold(false);
  FontTitle.setWeight(QFont::Light);
  FontTitle.setPixelSize(m_iFontSize);

  //Initialize Label font
  QFont FontLabel(FONT_FAMILY);
  FontLabel.setBold(false);
  FontLabel.setWeight(QFont::Light);
  FontLabel.setPixelSize(m_iFontLabelSize);

  //Set Painter
  Painter.setWindow(0,0,100,100);
  Painter.setViewport(0,0,iHeightPix,iHeightPix);
  QColor ColorText=palette().color(QPalette::Text);

  //Draw one Histogram per column in the Model
  for (int iCol=0;iCol<iNumCols;iCol++)
  {
    //Draw Title
    QPen Pen;
    Pen.setColor(ColorText);
    Pen.setCosmetic(true);
    Painter.setPen(Pen);
    Painter.setFont(FontTitle);
    Painter.drawText(0,m_iTitleMargin,iWidth,m_iFontSize+1,Qt::AlignHCenter|Qt::AlignTop,m_pModel->horizontalHeaderItem(iCol+2)->text());

    //Draw the x-axe
    Painter.drawLine(m_iHMarginHis,iCenter,iWidth-m_iHMarginHis,iCenter);

    //Set Pen for bar
    Painter.setPen(Qt::NoPen);

    //Draw colored bars
    for (int iRow=0;iRow<iNumBars;++iRow)
    {
      Painter.setBrush(QBrush(QColor(aiRed[iRow % NUM_COLORS],aiGreen[iRow % NUM_COLORS],aiBlue[iRow % NUM_COLORS],ALPHA)));
      double dVal=m_pModel->item(iRow,iCol+2)->text().toDouble();
      Painter.drawRect(m_iHMarginHis+iRow*m_iBarWidth,iCenter-qRound(dVal*dUnit),m_iBarWidth/2,qRound(dVal*dUnit));
    }

    //Draw labels
    Painter.setFont(FontLabel);
    Painter.setPen(ColorText);

    int iDirection;
    if (iCenter<=50)
      iDirection=1; //From up to down
    else
      iDirection=-1; //From down to up

    Painter.rotate(90*iDirection);
    for (int iRow=0;iRow<iNumBars;++iRow)
    {
      if (iDirection==1)
        Painter.drawText(iCenter+m_iFontLabelSize,(m_iHMarginHis+iRow*m_iBarWidth+m_iBarWidth)*-1,
                           100,m_iBarWidth/2+1,Qt::AlignBottom|Qt::AlignLeft,m_pModel->item(iRow,1)->text());
      else
        Painter.drawText(-iCenter+m_iFontLabelSize,(m_iHMarginHis+iRow*m_iBarWidth+m_iBarWidth/2),
                           100,m_iBarWidth/2,Qt::AlignTop|Qt::AlignLeft,m_pModel->item(iRow,1)->text());
    }
    Painter.rotate(-90*iDirection);

    //Move the axe to the next Histogram
    Painter.translate(iWidth,0);
  }
  setPixmap(Pixmap);
}


void CReportDrawing::updateModel(QStandardItemModel *pModel, Draw iDraw)
{
  m_pModel=pModel;
  updatePixmap(iDraw);
}

void CReportDrawing::updatePixmap(Draw iDraw)
{
  //Reset pixmap & size to zero
  setPixmap(QPixmap());
  resize(0,0);

  //Validate Model
  if (m_pModel==0) return;								//No assigned Model
  if (m_pModel->columnCount()<3) return;	//Empty Model
  if (size().isNull()) return;						//Widget is minimized

  if (iDraw==DrawPie)
    drawPie();
  if (iDraw==DrawHistogram)
    drawHistogram();
}

void	CReportDrawing::mousePressEvent ( QMouseEvent * pEvent )
{
  m_Point=pEvent->pos();
  QLabel::mousePressEvent(pEvent);
}

void	CReportDrawing::mouseReleaseEvent ( QMouseEvent * pEvent )
{
  if ((m_Point - pEvent->pos()).manhattanLength() < QApplication::startDragDistance()) emit hideDrawing();
  QLabel::mouseReleaseEvent(pEvent);
}
