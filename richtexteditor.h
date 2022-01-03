#ifndef RICHTEXTEDITOR_H
#define RICHTEXTEDITOR_H

#include <QFrame>
#include <QString>

class QWidget;
class QPixmap;

namespace Ui
{
  class CRichTextEditor;
}

class CRichTextEditor : public QFrame
{
  Q_OBJECT
public:
  CRichTextEditor(QWidget *pWidget = 0);
  ~CRichTextEditor();
  QString toHtml();
  void setHtml(const QString& sText);
  void clear();

private:
  Ui::CRichTextEditor *ui;
  QPixmap *m_pPixmap;

protected:
  void showEvent( QShowEvent *pEvent );

private:
  void updateTBIcon();

private slots:
  void on_pTextEditor_cursorPositionChanged();
  void on_pTBJustify_clicked(bool checked);
  void on_pTBRight_clicked(bool checked);
  void on_pTBLeft_clicked(bool checked);
  void on_pTBCenter_clicked(bool checked);
  void on_pTBUnderline_clicked(bool checked);
  void on_pTBItalic_clicked(bool checked);
  void on_pTBBold_clicked(bool checked);
  void on_pTBColor_clicked();
  void on_pCBSize_currentIndexChanged(int iIndex);

signals:
  void textEdited();
};

#endif // RICHTEXTEDITOR_H
