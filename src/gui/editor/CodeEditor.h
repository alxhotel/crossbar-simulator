#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <iostream>

#include <QString>
#include <QTextStream>
#include <QPainter>
#include <QTextBlock>
#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit {
	Q_OBJECT

public:
	CodeEditor(QWidget *parent = 0);

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();
	
	std::string getText();
	
	void setHighlightGray(int line_number);
	void setHighlightRed(int line_number);
	void setHighlightColor(QColor color, int line_number);
	void clearManualSelections();

protected:
	void resizeEvent(QResizeEvent *event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget {
public:
	LineNumberArea(CodeEditor *editor) : QWidget(editor) {
		codeEditor = editor;
	}

	QSize sizeHint() const {
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	CodeEditor *codeEditor;
};


#endif /* CODEEDITOR_H */
