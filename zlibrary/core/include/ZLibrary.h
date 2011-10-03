/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLIBRARY_H__
#define __ZLIBRARY_H__

#include <string>

#include <ZLFSManager.h>

class ZLApplication;
class ZLPaintContext;

class ZLibrary {

public:
	static const std::string FileNameDelimiter;
	static const std::string PathDelimiter;
	static const std::string EndOfLine;
	static std::string Language();
	static std::string Country();

	static const std::string BaseDirectory;
        static std::string ZLibraryDirectory();

        static std::string ImageDirectory();
        static std::string ApplicationName();
        static std::string ApplicationImageDirectory();
        static std::string ApplicationDirectory();
        static std::string DefaultFilesPathPrefix();

        static std::string ApplicationWritableDirectory();

public:
	static bool init(int &argc, char **&argv);
	static void parseArguments(int &argc, char **&argv);
	static ZLPaintContext *createContext();
	static void run(ZLApplication *application);
	static void shutdown();

private:
	static void initLocale();

private:
	static bool ourLocaleIsInitialized;
	static std::string ourLanguage;
	static std::string ourCountry;
	static std::string ourZLibraryDirectory;

	static std::string ourImageDirectory;
	static std::string ourApplicationImageDirectory;
	static std::string ourApplicationName;
	static std::string ourApplicationDirectory;
	static std::string ourApplicationWritableDirectory;
	static std::string ourDefaultFilesPathPrefix;

private:
	static std::string replaceRegExps(const std::string &pattern);
	static void initApplication(const std::string &name);

private:
	ZLibrary();

friend class ZLApplicationBase;
};

#endif /* __ZLIBRARY_H__ */