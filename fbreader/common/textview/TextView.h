/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

#include <vector>
#include <string>

#include <abstract/ZLOptions.h>
#include <abstract/ZLView.h>

#include "TextElement.h"
#include "Word.h"
#include "TextStyle.h"
#include "ParagraphCursor.h"

class TextModel;
class Paragraph;
class TextMark;

class TextView : public ZLView {

public:
	static ZLColorOption TreeLinesColorOption;
	static ZLBooleanOption AutoHyphenationOption;

	static ZLBooleanOption ShowPositionIndicatorOption;
	static ZLBooleanOption IsIndicatorSensitiveOption;
	static ZLColorOption PositionIndicatorColorOption;
	static ZLIntegerOption PositionIndicatorHeightOption;
	static ZLIntegerOption PositionIndicatorOffsetOption;

	static ZLIntegerOption OverlappingTypeOption;
	static ZLIntegerOption LinesToOverlapOption;
	static ZLIntegerOption LinesToScrollOption;
	static ZLIntegerOption PercentToScrollOption;
	
public:
	enum OverlappingType {
		NO_OVERLAPPING,
		NUMBER_OF_OVERLAPPED_LINES,
		NUMBER_OF_SCROLLED_LINES,
		PERCENT_OF_SCROLLED
	};

private:
	class ViewStyle {

	public:
		ViewStyle(ZLPaintContext &context) VIEW_SECTION;
		~ViewStyle() VIEW_SECTION;

		void reset() VIEW_SECTION;
		void setStyle(const TextStylePtr style) VIEW_SECTION;
		void applyControl(const ControlElement &control) VIEW_SECTION;
		void applyControls(const WordCursor &begin, const WordCursor &end) VIEW_SECTION;

		const ZLPaintContext &context() const VIEW_SECTION;
		const TextStylePtr style() const VIEW_SECTION;
		int elementWidth(const WordCursor &cursor) const VIEW_SECTION;
		int elementHeight(const WordCursor &cursor) const VIEW_SECTION;
		int textAreaHeight() const VIEW_SECTION;

		int wordWidth(const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) const VIEW_SECTION;

	private:
		TextStylePtr myStyle;
		ZLPaintContext &myContext;
		mutable int myWordHeight;
	};

	struct LineInfo {
		LineInfo(const WordCursor &word, TextStylePtr style) VIEW_SECTION;
		~LineInfo() VIEW_SECTION;

		WordCursor Start;
		WordCursor End;
		bool IsVisible;
		int Width;
		int Height;
		int SpaceCounter;
		TextStylePtr StartStyle;
	};

protected:
	struct ParagraphPosition {
		int ParagraphNumber;
		int YStart, YEnd;
		ParagraphPosition(int paragraphNumber, int yStart, int yEnd) VIEW_SECTION;
		~ParagraphPosition() VIEW_SECTION;
	};

	struct TextElementPosition {
		int ParagraphNumber, TextElementNumber;
		TextElement::Kind Kind;
		int XStart, XEnd, YStart, YEnd;
		TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) VIEW_SECTION;
		~TextElementPosition() VIEW_SECTION;
	};

protected:
	TextView(ZLPaintContext &context) VIEW_SECTION;
	virtual ~TextView() VIEW_SECTION;

public:
	void clearCaches() VIEW_SECTION;
	virtual void paint() VIEW_SECTION;

	void scrollPage(bool forward) VIEW_SECTION;
	void gotoMark(TextMark mark) VIEW_SECTION;
	virtual void gotoParagraph(int num, bool last = false) VIEW_SECTION;

	virtual void setModel(const TextModel *model, const std::string &name) VIEW_SECTION;

	void search(const std::string &text, bool ignoreCase, bool wholeText, bool backward) VIEW1_SECTION;
	bool canFindNext() const VIEW1_SECTION;
	void findNext() VIEW1_SECTION;
	bool canFindPrevious() const VIEW1_SECTION;
	void findPrevious() VIEW1_SECTION;

	bool onStylusPress(int x, int y) VIEW1_SECTION;

protected:
	const ParagraphPosition *paragraphByCoordinate(int y) const VIEW1_SECTION;
	const TextElementPosition *elementByCoordinates(int x, int y) const VIEW1_SECTION;

	void rebuildPaintInfo(bool strong) VIEW_SECTION;

	const WordCursor &startCursor() const VIEW_SECTION;
	const WordCursor &endCursor() const VIEW_SECTION;
	void setStartCursor(ParagraphCursor *cursor) VIEW_SECTION;
	void moveStartCursor(int paragraphNumber, int wordNumber, int charNumber) VIEW_SECTION;
	void moveStartCursor(int paragraphNumber, bool start) VIEW_SECTION;
	void moveEndCursor(int paragraphNumber, int wordNumber, int charNumber) VIEW_SECTION;
	void moveEndCursor(int paragraphNumber, bool start) VIEW_SECTION;

	bool empty() const VIEW_SECTION;

private:
	void clear() VIEW_SECTION;

	int paragraphHeight(const WordCursor &cursor, bool beforeCurrentPosition) VIEW_SECTION;
	void skip(WordCursor &paragraph, int height) VIEW_SECTION;
	LineInfo processTextLine(const WordCursor &start, const WordCursor &end) VIEW_SECTION;
	void drawTextLine(const LineInfo &info) VIEW_SECTION;
	void drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) VIEW1_SECTION;
	void drawString(int x, int y, const char *str, int len, const Word::WordMark *mark, int shift) VIEW1_SECTION;
	void drawTreeNode(TreeElement::TreeElementKind kind, int height) VIEW1_SECTION;

	void preparePaintInfo() VIEW_SECTION;

	WordCursor findLineFromStart(unsigned int overlappingValue) const VIEW_SECTION;
	WordCursor findLineFromEnd(unsigned int overlappingValue) const VIEW_SECTION;
	WordCursor findPercentFromStart(unsigned int percent) const VIEW_SECTION;

	WordCursor findStart(const WordCursor &end) VIEW_SECTION;
	WordCursor buildInfos(const WordCursor &start) VIEW_SECTION;

protected:
	const TextModel *myModel;
	std::string myName;

private:
	enum {
		NOTHING_TO_PAINT,
		READY,
		START_IS_KNOWN,
		END_IS_KNOWN,
		TO_SCROLL_FORWARD,
		TO_SCROLL_BACKWARD
	} myPaintState;
	WordCursor myStartCursor;
	WordCursor myEndCursor;
	std::vector<LineInfo> myLineInfos;
	std::map<WordCursor,LineInfo> myLineInfoCache;

	OverlappingType myOverlappingType;
	unsigned int myOverlappingValue;

	int myOldWidth, myOldHeight;

	std::vector<ParagraphPosition> myParagraphMap;
	std::vector<TextElementPosition> myTextElementMap;

	std::vector<size_t> myTextSize;
	size_t myFullTextSize;

	ViewStyle myStyle;
};

inline TextView::ViewStyle::~ViewStyle() {}
inline const ZLPaintContext &TextView::ViewStyle::context() const { return myContext; }
inline const TextStylePtr TextView::ViewStyle::style() const { return myStyle; }

inline TextView::LineInfo::LineInfo(const WordCursor &word, TextStylePtr style) : Start(word), End(word), IsVisible(false), Width(0), Height(0), SpaceCounter(0), StartStyle(style) {}
inline TextView::LineInfo::~LineInfo() {}

inline TextView::ParagraphPosition::ParagraphPosition(int paragraphNumber, int yStart, int yEnd) : ParagraphNumber(paragraphNumber), YStart(yStart), YEnd(yEnd) {}
inline TextView::ParagraphPosition::~ParagraphPosition() {}

inline TextView::TextElementPosition::TextElementPosition(int paragraphNumber, int textElementNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) : ParagraphNumber(paragraphNumber), TextElementNumber(textElementNumber), Kind(kind), XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}
inline TextView::TextElementPosition::~TextElementPosition() {}

inline bool TextView::empty() const { return myPaintState == NOTHING_TO_PAINT; }
inline const WordCursor &TextView::startCursor() const { return myStartCursor; }
inline const WordCursor &TextView::endCursor() const { return myEndCursor; }

#endif /* __TEXTVIEW_H__ */
