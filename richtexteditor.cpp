#include <QtGui>
#include <QtWidgets>
#include "richtexteditor.h"
#include "ui_richtexteditor.h"

CRichTextEditor::CRichTextEditor(QWidget *pWidget) :
    QFrame(pWidget),
    ui(new Ui::CRichTextEditor)
{
  ui->setupUi(this);

#ifdef Q_WS_MAEMO_5
  ui->pHLayout->setSpacing(0);
#endif

  QIcon Icon1;
  Icon1.addFile(":/buttons/icons/button-editor-bold.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBBold->setIcon(Icon1);

  QIcon Icon2;
  Icon2.addFile(":/buttons/icons/button-editor-italic.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBItalic->setIcon(Icon2);

  QIcon Icon3;
  Icon3.addFile(":/buttons/icons/button-editor-underline.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBUnderline->setIcon(Icon3);

  QIcon Icon4;
  Icon4.addFile(":/buttons/icons/button-editor-left.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBLeft->setIcon(Icon4);

  QIcon Icon5;
  Icon5.addFile(":/buttons/icons/button-editor-center.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBCenter->setIcon(Icon5);

  QIcon Icon6;
  Icon6.addFile(":/buttons/icons/button-editor-right.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBRight->setIcon(Icon6);

  QIcon Icon7;
  Icon7.addFile(":/buttons/icons/button-editor-justify.png", QSize(), QIcon::Normal, QIcon::Off);
  ui->pTBJustify->setIcon(Icon7);

  //Set Alignment buttons to be mutual exclusive
  QButtonGroup *pButtonGroup=new QButtonGroup(this);
  pButtonGroup->addButton(ui->pTBCenter);
  pButtonGroup->addButton(ui->pTBLeft);
  pButtonGroup->addButton(ui->pTBRight);
  pButtonGroup->addButton(ui->pTBJustify);

  //Set Pixmap for Color control
  QSize Size=ui->pTBColor->iconSize();
  m_pPixmap=new QPixmap(Size.width(),Size.height());
  updateTBIcon();

  //Get font size based on application font
  QFont Font=qApp->font();
  int iSize=QFontInfo(Font).pointSize();

  QFont FontAux;

  //Fill font size combobox
  ui->pCBSize->blockSignals(true);

  FontAux.setPointSize(iSize);
  ui->pCBSize->addItem(tr("normal"),QFontInfo(FontAux).pointSize());

  FontAux.setPointSize(qRound(iSize-iSize/8.0));
  ui->pCBSize->addItem(tr("medium"),QFontInfo(FontAux).pointSize());

  FontAux.setPointSize(qRound(iSize-2.0*iSize/8.0));
  ui->pCBSize->addItem(tr("small"),QFontInfo(FontAux).pointSize());

  FontAux.setPointSize(qRound(iSize-3.0*iSize/8.0));
  ui->pCBSize->addItem(tr("xsmall"),QFontInfo(FontAux).pointSize());

  ui->pCBSize->setCurrentIndex(0);
  ui->pTextEditor->setFontPointSize(ui->pCBSize->itemData(0).toDouble());
  ui->pCBSize->blockSignals(false);

  //Create connections
  connect(ui->pTextEditor,SIGNAL(textChanged()),this,SIGNAL(textEdited()));
}

CRichTextEditor::~CRichTextEditor()
{
  delete ui;
  delete m_pPixmap;
}

void CRichTextEditor::showEvent( QShowEvent */*pEvent*/ )
{
  //Focus on Editor
  ui->pTextEditor->setFocus();
}

QString CRichTextEditor::toHtml()
{
  return ui->pTextEditor->toHtml();
}

void CRichTextEditor::setHtml(const QString& sText)
{
  ui->pTextEditor->blockSignals(true);
  ui->pTextEditor->setHtml(sText);
  ui->pTextEditor->blockSignals(false);
}

void CRichTextEditor::clear()
{
  ui->pTextEditor->blockSignals(true);
  ui->pTextEditor->clear();
  ui->pTextEditor->blockSignals(false);
}

void CRichTextEditor::on_pCBSize_currentIndexChanged(int iIndex)
{
  ui->pTextEditor->setFontPointSize(ui->pCBSize->itemData(iIndex).toDouble());
  ui->pTextEditor->setFocus();
  emit textEdited();
}

void CRichTextEditor::on_pTBColor_clicked()
{
  QColor Color1=ui->pTextEditor->textColor();
  QColor Color2=QColorDialog::getColor(Color1,this,tr("Color chooser"));
  ui->pTextEditor->setTextColor(Color2);
  updateTBIcon();
}

void CRichTextEditor::on_pTBBold_clicked(bool bChecked)
{
  if (bChecked)
    ui->pTextEditor->setFontWeight(QFont::Bold);
  else
    ui->pTextEditor->setFontWeight(QFont::Normal);
  emit textEdited();
}

void CRichTextEditor::on_pTBItalic_clicked(bool bChecked)
{
  ui->pTextEditor->setFontItalic(bChecked);
  emit textEdited();
}

void CRichTextEditor::on_pTBUnderline_clicked(bool bChecked)
{
  ui->pTextEditor->setFontUnderline(bChecked);
  emit textEdited();
}

void CRichTextEditor::on_pTBCenter_clicked(bool bChecked)
{
  if (bChecked) ui->pTextEditor->setAlignment(Qt::AlignHCenter);
  emit textEdited();
}

void CRichTextEditor::on_pTBLeft_clicked(bool bChecked)
{
  if (bChecked) ui->pTextEditor->setAlignment(Qt::AlignLeft);
  emit textEdited();
}

void CRichTextEditor::on_pTBRight_clicked(bool bChecked)
{
  if (bChecked) ui->pTextEditor->setAlignment(Qt::AlignRight);
  emit textEdited();
}

void CRichTextEditor::on_pTBJustify_clicked(bool bChecked)
{
  if (bChecked) ui->pTextEditor->setAlignment(Qt::AlignJustify);
  emit textEdited();
}

void CRichTextEditor::on_pTextEditor_cursorPositionChanged()
{
  //Update button Color
  updateTBIcon();

  //Update button Bold
  if (ui->pTextEditor->fontWeight()==QFont::Bold)
    ui->pTBBold->setChecked(true);
  else
    ui->pTBBold->setChecked(false);

  //Update button Italic
  ui->pTBItalic->setChecked(ui->pTextEditor->fontItalic());

  //Update button Underline
  ui->pTBUnderline->setChecked(ui->pTextEditor->fontUnderline());

  //Update combobox Size
  ui->pCBSize->blockSignals(true);
  int iSize=ui->pTextEditor->fontPointSize();
  for (int i=0;i<4;i++)
  {
    if (ui->pCBSize->itemData(i).toInt()==iSize)
      ui->pCBSize->setCurrentIndex(i);
  }
  ui->pCBSize->blockSignals(false);

  //Update buttons for Aligment
  switch (ui->pTextEditor->alignment())
  {
    case Qt::AlignHCenter:
      ui->pTBCenter->setChecked(true);
      break;
    case Qt::AlignRight:
      ui->pTBRight->setChecked(true);
      break;
    case Qt::AlignLeft:
      ui->pTBLeft->setChecked(true);
      break;
    case Qt::AlignJustify:
      ui->pTBJustify->setChecked(true);
      break;
  }
}

void CRichTextEditor::updateTBIcon()
{
  m_pPixmap->fill(ui->pTextEditor->textColor());
  QPainter Painter(m_pPixmap);
  Painter.setPen(Qt::white);
  Painter.drawRect(m_pPixmap->rect().adjusted(0,0,-1,-1));
  ui->pTBColor->setIcon(QIcon(*m_pPixmap));
}
