#include "CodeEditor.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
	this->lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() {
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
	if (dy)
		this->lineNumberArea->scroll(0, dy);
	else
		this->lineNumberArea->update(0, rect.y(), this->lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	this->lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QColor lineColor = QColor(Qt::yellow).lighter(160);

		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::setHighlightGray(int lineNumber) {
	this->setHighlightColor(QColor(Qt::darkGray).lighter(180), lineNumber);
}

void CodeEditor::setHighlightRed(int lineNumber) {
	this->setHighlightColor(QColor(Qt::red).lighter(180), lineNumber);
}

void CodeEditor::setHighlightColor(QColor lineColor, int lineNumber) {
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        
		// Select the whole line [0 .. N - 1]
		QTextBlock block = this->document()->findBlockByLineNumber(lineNumber - 1);
		QTextCursor cursor(this->document());
		cursor.setPosition(block.position());
		selection.cursor = cursor;
		
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::clearManualSelections() {
	QList<QTextEdit::ExtraSelection> extraSelections;
	setExtraSelections(extraSelections);
	this->highlightCurrentLine();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
	QPainter painter(this->lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, this->lineNumberArea->width(),
				fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

std::string CodeEditor::getText() {
	QString text = this->toPlainText();
	QTextStream str(&text, QIODevice::ReadOnly);
	return text.toUtf8().constData();
}
