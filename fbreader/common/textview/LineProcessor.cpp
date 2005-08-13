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

#include <algorithm>

#include <abstract/ZLUnicodeUtil.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "Word.h"

#include "../hyphenation/Hyphenator.h"

TextView::LineInfo TextView::processTextLine(const WordCursor &start, const WordCursor &end) {
	std::map<WordCursor,LineInfo>::const_iterator it = myLineInfoCache.find(start);
	if (it != myLineInfoCache.end()) {
		const LineInfo &storedInfo = it->second;
		myStyle.applyControls(storedInfo.Start, storedInfo.End);
		return storedInfo;
	}

	LineInfo info(start, myStyle.style());

	TextStylePtr storedStyle = myStyle.style();
	WordCursor current = start;

	info.Width = myStyle.style()->leftIndent();
	int newWidth = info.Width;
	int newHeight = info.Height;
	int maxWidth = myStyle.context().width() - myStyle.style()->rightIndent();
	bool wordOccured = false;
	bool isVisible = false;
	int lastSpaceWidth = 0;
	int internalSpaceCounter = 0;
	int removeLastSpace = false;

	TextElement::Kind elementKind = current.element().kind();

	do {
		newWidth += myStyle.elementWidth(current);
		newHeight = std::max(newHeight, myStyle.elementHeight(current));
		switch (elementKind) {
			case TextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ControlElement&)current.element());
				break;
			case TextElement::WORD_ELEMENT:
			case TextElement::IMAGE_ELEMENT:
				wordOccured = true;
				isVisible = true;
				break;
			case TextElement::HSPACE_ELEMENT:
				if (wordOccured) {
					wordOccured = false;
					internalSpaceCounter++;
					lastSpaceWidth = myStyle.context().spaceWidth();
					newWidth += lastSpaceWidth;
				}
				break;
			case TextElement::EMPTY_LINE_ELEMENT:
			case TextElement::TREE_ELEMENT:
				isVisible = true;
			default:
				break;
		}

		if ((newWidth > maxWidth) && !info.End.sameElementAs(start)) {
			break;
		}

		current.nextWord();
		bool allowBreak = current.sameElementAs(end);
		if (!allowBreak) {
			elementKind = current.element().kind();
			allowBreak =
				(elementKind != TextElement::WORD_ELEMENT) &&
				(elementKind != TextElement::IMAGE_ELEMENT) &&
				(elementKind != TextElement::CONTROL_ELEMENT);
		}
		if (allowBreak) {
			info.IsVisible = isVisible;
			info.Width = newWidth;
			info.Height = std::max(info.Height, newHeight);
			info.End = current;
			storedStyle = myStyle.style();
			info.SpaceCounter = internalSpaceCounter;
			removeLastSpace = !wordOccured && (info.SpaceCounter > 0);
		}
	} while (!current.sameElementAs(end));

	if (TextView::AutoHyphenationOption.value() && myStyle.style()->allowHyphenations()) {
		if (!current.sameElementAs(end) && (current.element().kind() == TextElement::WORD_ELEMENT)) {
			newWidth -= myStyle.elementWidth(current);
			const Word &word = (Word&)current.element();
			int spaceLeft = maxWidth - newWidth;
			if ((word.Length > 3) && (spaceLeft > 2 * myStyle.context().spaceWidth())) {
				ZLUnicodeUtil::Ucs2String ucs2string;
				ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.Data, word.Size);
				HyphenationInfo hyphenationInfo = Hyphenator::instance().info(word);
				int hyphenationPosition = word.Length - 1;
				int subwordWidth = 0;
				for (; hyphenationPosition > 0; hyphenationPosition--) {
					if (hyphenationInfo.isHyphenationPossible(hyphenationPosition)) {
						subwordWidth = myStyle.wordWidth(word, 0, hyphenationPosition, ucs2string[hyphenationPosition - 1] != '-');
						if (subwordWidth <= spaceLeft) {
							break;
						}
					}
				}
				if (hyphenationPosition > 0) {
					info.IsVisible = true;
					info.Width = newWidth + subwordWidth;
					info.Height = std::max(info.Height, newHeight);
					info.End = current;
					storedStyle = myStyle.style();
					info.SpaceCounter = internalSpaceCounter;
					removeLastSpace = false;
					info.End.setCharNumber(hyphenationPosition);
				}
			}
		}
	}

	if (removeLastSpace) {
		info.Width -= lastSpaceWidth;
		info.SpaceCounter--;
	}

	myStyle.setStyle(storedStyle);

	myLineInfoCache.insert(std::pair<WordCursor,LineInfo>(info.Start, info));
	return info;
}
