#include <QtGui>
#include <QtWidgets>
#include <QtSql>
#include "basedelegate.h"
#include "global.h"

#define ITEM_MARGIN 3
#define PROGRESS_MARGIN 10
#define ICON_UNCHECKED   ":/status/icons/status-unchecked.png"
#define ICON_PARTIALLY   ":/status/icons/status-partially.png"
#define ICON_CHECKED     ":/status/icons/status-checked.png"

CBaseDelegate::CBaseDelegate(QObject *pObject):QStyledItemDelegate(pObject)
{
}

void CBaseDelegate::paint(QPainter *pPainter, const QStyleOptionViewItem &Option,const QModelIndex &Index) const
{
  int iActualColumn=Index.column();
  if (m_hashColumnFormat.contains(iActualColumn))
  {
    //Get the format to be analyzed
    Format iFormat=m_hashColumnFormat.value(iActualColumn);

    //Draw an Icon from the file name in the cell
    if (iFormat==FormatIcon)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      QPixmap Pixmap(Index.data().toString());
      QApplication::style()->drawItemPixmap(pPainter,Option.rect,Qt::AlignCenter,Pixmap);
    }

    //Draw a Status pixmap in the cell
    else if (iFormat==FormatCheck)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      int iState=Index.data().toInt();
      if (iState==0) QApplication::style()->drawItemPixmap(pPainter,Option.rect,Qt::AlignCenter,QPixmap(ICON_UNCHECKED));
      if (iState==1) QApplication::style()->drawItemPixmap(pPainter,Option.rect,Qt::AlignCenter,QPixmap(ICON_PARTIALLY));
      if (iState==2) QApplication::style()->drawItemPixmap(pPainter,Option.rect,Qt::AlignCenter,QPixmap(ICON_CHECKED));
    }

    //Draw a formatted Number in the cell
    else if (iFormat==FormatNumber)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      //Save original Painter
      pPainter->save();

      //Set Font to Painter
      if (!Index.data(Qt::FontRole).isNull())
      {
        pPainter->setFont(Index.data(Qt::FontRole).value<QFont>());
      }
      else
      {
        pPainter->setFont(Option.font);
      }

      //Format the amount
      QString sTotal;
      double dTotal=Index.data().toDouble();

      if (dTotal<0)
      {
        pPainter->setPen(QColor(255,0,0));
        sTotal="("+ QString::number(dTotal*-1.0,'f',m_iNumberPrecision) + ")";
      }
      else
      {
        sTotal=QString::number(dTotal,'f',m_iNumberPrecision);
      }

      //Select area in if it is the case
      if (Option.state & QStyle::State_Selected)
      {
        pPainter->setPen(Option.palette.highlightedText().color());
      }

      QApplication::style()->drawItemText(pPainter,Option.rect.adjusted(0,0,-ITEM_MARGIN,0),Qt::AlignRight|Qt::AlignVCenter,Option.palette,true,sTotal);
      pPainter->restore();
    }

    //Draw a formatted Date in the cell
    else if (iFormat==FormatDate)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=QDate::fromJulianDay(Index.data().toInt()).toString(m_sDateFormat);
      OptionV4.displayAlignment=Qt::AlignHCenter|Qt::AlignVCenter;
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem,&OptionV4,pPainter);
    }

    //Draw a Category in the cell
    else if (iFormat==FormatCategory)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=m_hashCategoryData.value(Index.data().toInt());;
      OptionV4.displayAlignment=Qt::AlignLeft|Qt::AlignVCenter;
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem,&OptionV4,pPainter);
    }

    //Draw a Unit in the cell
    else if (iFormat==FormatUnit)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);

      if (Index.data().toInt()==0)
        OptionV4.text=tr("Day");
      else if (Index.data().toInt()==1)
        OptionV4.text=tr("Week");
      else if (Index.data().toInt()==2)
        OptionV4.text=tr("Month");

      OptionV4.displayAlignment=Qt::AlignHCenter|Qt::AlignVCenter;
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem,&OptionV4,pPainter);
    }

    //Draw a Class in the cell
    else if (iFormat==FormatClass)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=m_hashClassData.value(Index.data().toInt());
      OptionV4.displayAlignment=Qt::AlignLeft|Qt::AlignVCenter;
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem,&OptionV4,pPainter);
    }

    //Draw a Frequency in the cell
    else if (iFormat==FormatFrequency)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.displayAlignment=Qt::AlignHCenter|Qt::AlignVCenter;
      QApplication::style()->drawControl(QStyle::CE_ItemViewItem,&OptionV4,pPainter);
    }

    //Draw a Progress pixmap in the cell
    else if (iFormat==FormatProgress)
    {
      //Draw the ItemView according to the Style
      QStyledItemDelegate::paint(pPainter,Option,QModelIndex());

      //Get parameters from Option
      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      int iHeight=OptionV4.rect.height();
      int iWidth=OptionV4.rect.width();
      QStringList lstParam=OptionV4.text.split(',');

      //Create and draw pixmap
      QPixmap Pixmap(iWidth,iHeight/3);
      Pixmap.fill(QApplication::palette().base().color());
      QPainter Painter(&Pixmap);

      //Draw color progress bar
      Painter.setPen(Qt::NoPen);
      Painter.setBrush(QBrush(QColor(lstParam.at(1).toInt(),lstParam.at(2).toInt(),lstParam.at(3).toInt(),lstParam.at(4).toInt())));
      Painter.drawRect(PROGRESS_MARGIN,0,(iWidth-PROGRESS_MARGIN*2)*lstParam.at(0).toInt()/100,iHeight/3);

      //Draw border progress bar
      Painter.setPen(QPen(QApplication::palette().text().color()));
      Painter.setBrush(QBrush(Qt::NoBrush));
      Painter.drawRect(PROGRESS_MARGIN,0,(iWidth-PROGRESS_MARGIN*2-1),iHeight/3-1);

      QApplication::style()->drawItemPixmap(pPainter,OptionV4.rect,Qt::AlignCenter,Pixmap);
    }
  }
  else
    QStyledItemDelegate::paint(pPainter,Option,Index);
}

QSize CBaseDelegate::sizeHint(const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
  int iActualColumn=Index.column();
  if (m_hashColumnFormat.contains(iActualColumn))
  {
    //Get the format to be analyzed
    Format iFormat=m_hashColumnFormat.value(iActualColumn);

    //Draw an Icon from the file name in the cell
    if (iFormat==FormatIcon)
    {
      return QSize(ICON_WIDTH,0);
    }

    //Draw a Checkbox in the cell
    else if (iFormat==FormatCheck)
    {
      return QSize(ICON_WIDTH,0);
    }

    //Draw a formatted Number in the cell
    else if (iFormat==FormatNumber)
    {
      QString sTotal;
      double dTotal=Index.data().toDouble();

      if (dTotal<0)
        sTotal="("+ QString::number(dTotal*-1.0,'f',m_iNumberPrecision) + ")";
      else
        sTotal=QString::number(dTotal,'f',m_iNumberPrecision);

      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=sTotal;

      //Obtain size and make correction
      QSize Size=QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem,&OptionV4,QSize());
      Size.setWidth(Size.width()+ITEM_MARGIN*4);

      return Size;
    }

    //Draw a formatted Date in the cell
    else if (iFormat==FormatDate)
    {
      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=QDate::fromJulianDay(Index.data().toInt()).toString(m_sDateFormat);

      return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem,&OptionV4,QSize());
    }

    //Draw a Category in the cell
    else if (iFormat==FormatCategory)
    {
      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=m_hashCategoryData.value(Index.data().toInt());

      return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem,&OptionV4,QSize());
    }

    //Draw a Unit in the cell
    else if (iFormat==FormatUnit)
    {
      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);

      if (Index.data().toInt()==0)
        OptionV4.text=tr("Day");
      else if (Index.data().toInt()==1)
        OptionV4.text=tr("Week");
      else if (Index.data().toInt()==2)
        OptionV4.text=tr("Month");

      return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem,&OptionV4,QSize());
    }

    //Draw a Class in the cell
    else if (iFormat==FormatClass)
    {
      QStyleOptionViewItem OptionV4=Option;
      initStyleOption(&OptionV4,Index);
      OptionV4.text=m_hashClassData.value(Index.data().toInt());

      return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem,&OptionV4,QSize());
    }

    //Draw a Frequency in the cell
    else if (iFormat==FormatFrequency)
    {
      return QStyledItemDelegate::sizeHint(Option,Index);
    }

    //Draw a Progress in the cell
    else if (iFormat==FormatProgress)
    {
      return QStyledItemDelegate::sizeHint(Option,Index);
    }
  }

  return QStyledItemDelegate::sizeHint(Option,Index);
}

void CBaseDelegate::updateCategoryData()
{
  QSqlTableModel Model;

  m_hashCategoryData.clear();

  //Fill out Hash with "category" table
  Model.setTable("category");
  Model.setSort(1,Qt::AscendingOrder);
  Model.select();

  int iRowCount=g_rowCount(Model);
  for (int i=0;i<iRowCount;i++)
    m_hashCategoryData.insert(Model.record(i).value(0).toInt(),
                              Model.record(i).value(1).toString());

  //Fill out Hash with "account" table
  Model.setTable("account");
  Model.setSort(1,Qt::AscendingOrder);
  Model.select();

  iRowCount=g_rowCount(Model);
  for (int i=0;i<iRowCount;i++)
    m_hashCategoryData.insert(Model.record(i).value(0).toInt()*-1,
                              "["+Model.record(i).value(3).toString()+"]");

}

void CBaseDelegate::updateClassData()
{
  QSqlTableModel Model;

  m_hashClassData.clear();

  //Fill out Hash with "class" table
  Model.setTable("class");
  Model.setSort(1,Qt::AscendingOrder);
  Model.select();

  int iRowCount=g_rowCount(Model);
  for (int i=0;i<iRowCount;i++)
    m_hashClassData.insert(Model.record(i).value(0).toInt(),
                              Model.record(i).value(1).toString());
}

void CBaseDelegate::clear()
{
  m_hashColumnFormat.clear();
}
