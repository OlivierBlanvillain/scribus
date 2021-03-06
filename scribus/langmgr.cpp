/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
    begin                : Jan 2005
    copyright            : (C) 2005 by Craig Bradney
    email                : cbradney@zip.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QObject>
#include <QStringList> 
#include <QTextStream>

#include "scconfig.h"
#include "langmgr.h"
#include "scpaths.h"

LanguageManager * LanguageManager::m_instance = 0;

LanguageManager * LanguageManager::instance()
{
	if(!m_instance)
	{
		m_instance = new LanguageManager;
		Q_ASSERT(m_instance);
		m_instance->init();
	}
	return m_instance;
}

void LanguageManager::deleteInstance()
{
	if (m_instance)
		delete m_instance;
	m_instance = 0;
}

void LanguageManager::languageChange()
{
	QList<LangDef> oldLangList = m_langTable;
	generateLangList();

	for (int i = 0; i < m_langTable.count(); ++i)
	{
		m_langTable[i].m_hyphAvailable = oldLangList[i].m_hyphAvailable;
		m_langTable[i].m_hyphFile = oldLangList[i].m_hyphFile;

		m_langTable[i].m_spellAvailable = oldLangList[i].m_spellAvailable;
		m_langTable[i].m_spellFile = oldLangList[i].m_spellFile;

		m_langTable[i].m_transAvailable = oldLangList[i].m_transAvailable;
		m_langTable[i].m_transFile = oldLangList[i].m_transFile;
	}
}

void LanguageManager::init(bool generateInstalledList)
{
	//generateUntranslatedLangList();
	generateLangList();
	if (generateInstalledList)
	{
		generateInstalledGUILangList();
		generateInstalledSpellLangList();
		generateInstalledHyphLangList();
	}
}

void LanguageManager::generateLangList()
{
	// So internally language would always be manipulated as a code and otherwise presented translated.
	//Build table;
	m_langTable.clear();
	m_langTable.append(LangDef("af",     "af_ZA", "Afrikaans",              QObject::tr( "Afrikaans" )));
	m_langTable.append(LangDef("an_ES",  "",      "Aragonese",              QObject::tr( "Aragonese" )) );
	m_langTable.append(LangDef("ar",     "",      "Arabic",                 QObject::tr( "Arabic" )) );
	m_langTable.append(LangDef("be_BY",  "",      "Belarusian",             QObject::tr( "Belarusian" )) );
	m_langTable.append(LangDef("bg",     "bg_BG", "Bulgarian",              QObject::tr( "Bulgarian" )) );
	m_langTable.append(LangDef("blo",    "",      "Anii",                   QObject::tr( "Anii" )) );
	m_langTable.append(LangDef("bn",     "",      "Bengali",                QObject::tr( "Bengali" )) );
	m_langTable.append(LangDef("bn_BD",  "",      "Bengali (Bangladesh)",   QObject::tr( "Bengali (Bangladesh)" )) );
	m_langTable.append(LangDef("br",     "br_FR", "Breton",                 QObject::tr( "Breton" )) );
	m_langTable.append(LangDef("bs_BA",  "bs_BA", "Bosnian",                QObject::tr( "Bosnian" )) );
	m_langTable.append(LangDef("ca",     "",      "Catalan",                QObject::tr( "Catalan" )) );
	m_langTable.append(LangDef("cs",     "cs_CZ", "Czech",                  QObject::tr( "Czech" )) );
	m_langTable.append(LangDef("cy",     "cy_GB", "Welsh",                  QObject::tr( "Welsh" )) );
	m_langTable.append(LangDef("da",     "da_DK", "Danish",                 QObject::tr( "Danish" )) );
	m_langTable.append(LangDef("de",     "de_DE", "German",                 QObject::tr( "German" )) );
	m_langTable.append(LangDef("de_1901","",      "German (Trad.)",         QObject::tr( "German (Trad.)" )) );
	m_langTable.append(LangDef("de_AT",  "",      "German (Austria)",       QObject::tr( "German (Austria)" )) );
	m_langTable.append(LangDef("de_CH",  "",      "German (Swiss)",         QObject::tr( "German (Swiss)" )) );
	m_langTable.append(LangDef("dz",     "",      "Dzongkha",               QObject::tr( "Dzongkha" )) );
	m_langTable.append(LangDef("el",     "",      "Greek",                  QObject::tr( "Greek" )) );
	m_langTable.append(LangDef("en_GB",  "en",    "English (UK)",           QObject::tr( "English (UK)" )) );
	m_langTable.append(LangDef("en_AU",  "",      "English (Australia)",    QObject::tr( "English (Australia)" )) );
	m_langTable.append(LangDef("en_CA",  "",      "English (Canada)",       QObject::tr( "English (Canada)" )) );
	m_langTable.append(LangDef("en_NZ",  "",      "English (New Zealand)",  QObject::tr( "English (New Zealand)" )) );
	m_langTable.append(LangDef("en_US",  "",      "English (US)",           QObject::tr( "English (US)" )) );
	m_langTable.append(LangDef("en_ZA",  "",      "English (South Africa)", QObject::tr( "English (South Africa)" )) );
	m_langTable.append(LangDef("eo",     "",      "Esperanto",              QObject::tr( "Esperanto" )) );
	m_langTable.append(LangDef("es",     "es_ES", "Spanish",                QObject::tr( "Spanish" )) );
	m_langTable.append(LangDef("es",     "es_ANY","Spanish",                QObject::tr( "Spanish" )) );
	m_langTable.append(LangDef("es_AR",  "",      "Spanish (Argentina)",    QObject::tr( "Spanish (Argentina)" )) );
	m_langTable.append(LangDef("es_LA",  "",      "Spanish (Latin)",        QObject::tr( "Spanish (Latin)" )) );
	m_langTable.append(LangDef("et",     "et_EE", "Estonian",               QObject::tr( "Estonian" )) );
	m_langTable.append(LangDef("eu",     "",      "Basque",                 QObject::tr( "Basque" )) );
	m_langTable.append(LangDef("fa_IR",  "",      "Persian",                QObject::tr( "Persian" )) );
	m_langTable.append(LangDef("fi",     "",      "Finnish",                QObject::tr( "Finnish" )) );
	m_langTable.append(LangDef("fr",     "fr_FR", "French",                 QObject::tr( "French" )) );
	m_langTable.append(LangDef("gd",     "gd_GB", "Scottish Gaelic",        QObject::tr( "Scottish Gaelic" )) );
	m_langTable.append(LangDef("gl",     "gl_ES", "Galician",               QObject::tr( "Galician" )) );
	m_langTable.append(LangDef("gu",     "gu_IN", "Gujarati",               QObject::tr( "Gujarati" )) );
	m_langTable.append(LangDef("he",     "he_IL", "Hebrew",                 QObject::tr( "Hebrew" )) );
	m_langTable.append(LangDef("hi_IN",  "",      "Hindi",                  QObject::tr( "Hindi" )) );
	m_langTable.append(LangDef("hr",     "",      "Croatian",               QObject::tr( "Croatian" )) );
	m_langTable.append(LangDef("hu",     "hu_HU", "Hungarian",              QObject::tr( "Hungarian" )) );
	m_langTable.append(LangDef("ia",     "",      "Latin",                  QObject::tr( "Latin" )) );
	m_langTable.append(LangDef("id",     "",      "Indonesian",             QObject::tr( "Indonesian" )) );
	m_langTable.append(LangDef("is",     "",      "Icelandic",              QObject::tr( "Icelandic" )) );
	m_langTable.append(LangDef("it",     "it_IT", "Italian",                QObject::tr( "Italian" )) );
	m_langTable.append(LangDef("ja",     "",      "Japanese",               QObject::tr( "Japanese" )) );
	m_langTable.append(LangDef("kab",    "",      "Kabyle",                 QObject::tr( "Kabyle" )) );
	m_langTable.append(LangDef("km",     "",      "Khmer",                  QObject::tr( "Khmer" )) );
	m_langTable.append(LangDef("ko",     "",      "Korean",                 QObject::tr( "Korean" )) );
	m_langTable.append(LangDef("kn_IN",  "",      "Kannada",                QObject::tr( "Kannada" )) );
	m_langTable.append(LangDef("ku",     "",      "Kurdish",                QObject::tr( "Kurdish" )) );
	m_langTable.append(LangDef("la",     "",      "Latin",                  QObject::tr( "Latin" )) );
	m_langTable.append(LangDef("lb",     "",      "Luxembourgish",          QObject::tr( "Luxembourgish" )) );
	m_langTable.append(LangDef("lo",     "",      "Lao",                    QObject::tr( "Lao" )) );
	m_langTable.append(LangDef("lt",     "lt_LT", "Lithuanian",             QObject::tr( "Lithuanian" )) );
	m_langTable.append(LangDef("mn_MN",  "",      "Mongolian",              QObject::tr( "Mongolian" )) );
	m_langTable.append(LangDef("ne_NP",  "",      "Nepali",                 QObject::tr( "Nepali" )) );
	m_langTable.append(LangDef("nb",     "nb_NO", "Norwegian (Bokmål)",     QObject::trUtf8( "Norwegian (Bokm\303\245l)" )) );
	m_langTable.append(LangDef("nl",     "nl_NL", "Dutch",                  QObject::tr( "Dutch" )) );
	m_langTable.append(LangDef("nn",     "nn_NO", "Norwegian (Nnyorsk)",    QObject::tr( "Norwegian (Nnyorsk)" )) );
	m_langTable.append(LangDef("no",     "no_NO", "Norwegian",              QObject::tr( "Norwegian" )) );
	m_langTable.append(LangDef("pl",     "pl_PL", "Polish",                 QObject::tr( "Polish" )) );
	m_langTable.append(LangDef("pt",     "pt_PT", "Portuguese",             QObject::tr( "Portuguese" )) );
	m_langTable.append(LangDef("pt_BR",  "",      "Portuguese (BR)",        QObject::tr( "Portuguese (BR)" )) );
	m_langTable.append(LangDef("ro",     "",      "Romanian",               QObject::tr( "Romanian" )) );
	m_langTable.append(LangDef("ru",     "ru_RU", "Russian",                QObject::tr( "Russian" )) );
	m_langTable.append(LangDef("sa",     "",      "Sanskrit",               QObject::tr( "Sanskrit" )) );
	m_langTable.append(LangDef("sk",     "sk_SK", "Slovak",                 QObject::tr( "Slovak" )) );
	m_langTable.append(LangDef("sl",     "sl_SL", "Slovenian",              QObject::tr( "Slovenian" )) );
	m_langTable.append(LangDef("so",     "",      "Somali",                 QObject::tr( "Somali" )) );
	m_langTable.append(LangDef("sq",     "",      "Albanian",               QObject::tr( "Albanian" )) );
	m_langTable.append(LangDef("sr",     "",      "Serbian",                QObject::tr( "Serbian" )) );
	m_langTable.append(LangDef("sr-Latn","sr-Latn","Serbian (Latin)",       QObject::tr( "Serbian (Latin)" )) );
	m_langTable.append(LangDef("sv",     "",      "Swedish",                QObject::tr( "Swedish" )) );
	m_langTable.append(LangDef("te",     "te_IN", "Telugu",                 QObject::tr( "Telugu" )) );
	m_langTable.append(LangDef("th",     "th_TH", "Thai",                   QObject::tr( "Thai" )) );
	m_langTable.append(LangDef("tr",     "tr_TR", "Turkish",                QObject::tr( "Turkish" )) );
	m_langTable.append(LangDef("ur",     "",      "Urdu",                   QObject::tr( "Urdu" )) );
	m_langTable.append(LangDef("uk",     "uk_UA", "Ukranian",               QObject::tr( "Ukranian" )) );
	m_langTable.append(LangDef("vi",     "",      "Vietnamese",             QObject::tr( "Vietnamese" )) );
	m_langTable.append(LangDef("zh",     "",      "Chinese",                QObject::tr( "Chinese" )) );
	m_langTable.append(LangDef("zh_TW",  "",      "Chinese (Trad.)",        QObject::tr( "Chinese (Trad.)" )) );
}

void LanguageManager::generateInstalledGUILangList()
{
	QString path = ScPaths::instance().translationDir();
	QString langAbbrev;
	QMap<QString, langPair>::Iterator it;
	QDir dir(path , "*.*", QDir::Name, QDir::Files | QDir::NoSymLinks);
	if (dir.exists() && (dir.count() != 0))
	{
		for (uint i = 0; i < dir.count(); ++i) 
		{
			QFileInfo file(path + dir[i]);
			if (file.suffix().toLower() == "qm")
			{
				langAbbrev = file.completeSuffix().remove(".qm");
				int j=langTableIndex(langAbbrev);
				if (j!=-1)
				{
					m_langTable[j].m_transAvailable=true;
					m_langTable[j].m_transFile=file.absoluteFilePath();
					//qDebug()<<"Found installed GUI translation file:"<<langAbbrev<<" : "<<langTable[j].m_transFile<<langTable[j].m_transName;
				}
			}
		}
	}
}

void LanguageManager::generateInstalledHyphLangList()
{
	QStringList dictionaryPaths;
	bool dictPathFound=findHyphDictionaries(dictionaryPaths);
	if (!dictPathFound)
	{
//		qDebug()<<"No preinstalled hyphenation dictonaries or paths found";
		return;
	}
	QMap<QString, QString> dictionaryMap;
	findHyphDictionarySets(dictionaryPaths, dictionaryMap);
	if (dictionaryMap.count()==0)
		return;

	QMap<QString, QString>::iterator it = dictionaryMap.begin();
//	qDebug()<<"Installed Hyphenation Dictonaries:";
	while (it != dictionaryMap.end())
	{
		int j=langTableIndex(it.key());
		if (j!=-1)
		{
			m_langTable[j].m_hyphAvailable=true;
			m_langTable[j].m_hyphFile=it.value();
			//qDebug()<<"Found installed hyphenation dictionary:"<<it.key()<<" : "<<it.value();
		}
		++it;
	}
}

void LanguageManager::generateInstalledSpellLangList()
{
	QStringList dictionaryPaths;
	bool dictPathFound=findSpellingDictionaries(dictionaryPaths);
	if (!dictPathFound)
	{
		qDebug()<<"No preinstalled spelling dictonaries or paths found";
		return;
	}
	QMap<QString, QString> dictionaryMap;
	findSpellingDictionarySets(dictionaryPaths, dictionaryMap);
	if (dictionaryMap.count()==0)
		return;

	QMap<QString, QString>::iterator it = dictionaryMap.begin();
//	qDebug()<<"Installed Spelling Dictonaries:";
	while (it != dictionaryMap.end())
	{
		int j=langTableIndex(it.key());
		if (j!=-1)
		{
			m_langTable[j].m_spellAvailable=true;
			m_langTable[j].m_spellFile=it.value();
//			qDebug()<<"Found installed spelling dictionary:"<<it.key()<<" : "<<it.value();
		}
		++it;
	}
}



int LanguageManager::langTableIndex(const QString &abbrev)
{
//	qDebug()<<"langTableIndex: Trying to find:"<<abbrev;
	for (int i = 0; i < m_langTable.size(); ++i)
	{
//		qDebug()<<abbrev<<langTable[i].m_priAbbrev<<langTable[i].m_altAbbrev;
		if (m_langTable[i].m_priAbbrev==abbrev || m_langTable[i].m_altAbbrev==abbrev)
			return i;
	}
	return -1;
}

const QString LanguageManager::getLangFromAbbrev(QString langAbbrev, bool getTranslated)
{
//	qDebug()<<"Trying to find:"<<langAbbrev;
	int i=langTableIndex(langAbbrev);
//	qDebug()<<"Index of"<<langAbbrev<<":"<<i;
	if (i==-1)
	{
		if (langAbbrev.length()>5)
		{
			langAbbrev.truncate(5);
			i=langTableIndex(langAbbrev);
		}
	}
	if (i!=-1)
	{
		if (getTranslated)
			return m_langTable[i].m_transName;
		else
			return m_langTable[i].m_name;
	}
	//qDebug()<<langAbbrev<<"not found";
	return "";
}

const QString LanguageManager::getAbbrevFromLang(QString lang, bool getFromTranslated, bool useInstalled)
{
	QMap<QString, langPair>::Iterator it;
	if (lang == "English" || lang == QObject::tr( "English"))
		useInstalled = false;
	for (int i = 0; i < m_langTable.size(); ++i)
	{
		if (useInstalled)
		{
			if (m_langTable[i].m_transAvailable && (m_langTable[i].m_name==lang || m_langTable[i].m_transName==lang))
				return m_langTable[i].m_priAbbrev;
		}
		else
		{
			//qDebug()<<lang<<langTable[i].m_priAbbrev<<langTable[i].m_name<<langTable[i].m_transName;
			if (m_langTable[i].m_name==lang || m_langTable[i].m_transName==lang)
				return m_langTable[i].m_priAbbrev;
		}
	}
	return "";
}

const QString LanguageManager::getLangFromTransLang(QString transLang)
{
	for (int i = 0; i < m_langTable.size(); ++i)
	{
		if (m_langTable[i].m_transName==transLang)
			return m_langTable[i].m_name;
	}
	return "";
}

const QString LanguageManager::getTransLangFromLang(QString lang)
{
	// Seems something is missing here!
	QString enLang(QObject::tr( "English"));
	if ((lang == "English") || (lang == enLang))
		return enLang;
	
	for (int i = 0; i < m_langTable.size(); ++i)
	{
		if (m_langTable[i].m_name==lang)
			return m_langTable[i].m_transName;
	}
	return "";
}

const QString LanguageManager::getShortAbbrevFromAbbrev(QString langAbbrev)
{
	//	qDebug()<<"Trying to find:"<<langAbbrev;
	int i = langTableIndex(langAbbrev);
	//	qDebug()<<"Index of"<<langAbbrev<<":"<<i;
	if (i == -1)
	{
		if (langAbbrev.length()>5)
		{
			langAbbrev.truncate(5);
			i=langTableIndex(langAbbrev);
		}
	}
	if (i != -1)
		return m_langTable[i].m_priAbbrev;
	//qDebug()<<langAbbrev<<"not found";
	return "";
}

const QString LanguageManager::getAlternativeAbbrevfromAbbrev(QString langAbbrev)
{
	int i=langTableIndex(langAbbrev);
	if (i!=-1)
		return m_langTable[i].m_altAbbrev;
	return "";
}

void LanguageManager::fillInstalledStringList(QStringList *stringListToFill, bool addDefaults) 
{
	if (stringListToFill)
	{
		QMap<QString, QString>::Iterator it;

		if (addDefaults) 
		{
			stringListToFill->append("");
			stringListToFill->append( QObject::tr( "English" ));
		}

		for (int i = 0; i < m_langTable.size(); ++i)
			stringListToFill->append(m_langTable[i].m_transName);
	}
}

void LanguageManager::fillInstalledGUIStringList(QStringList *stringListToFill, bool addDefaults)
{
	if (stringListToFill)
	{
		if (addDefaults)
		{
			stringListToFill->append("");
			stringListToFill->append( QObject::tr( "English" ));
		}

		for (int i = 0; i < m_langTable.size(); ++i)
		{
			//qDebug()<<langTable[i].m_transName<<langTable[i].m_transAvailable;
			if (m_langTable[i].m_transAvailable)
				stringListToFill->append(m_langTable[i].m_transName);
		}
		stringListToFill->sort();
	}
}

void LanguageManager::fillInstalledHyphStringList(QStringList *stringListToFill)
{
	if (stringListToFill)
	{
		for (int i = 0; i < m_langTable.size(); ++i)
		{
			//qDebug()<<langTable[i].m_transName<<langTable[i].m_hyphAvailable;
			if (m_langTable[i].m_hyphAvailable)
				stringListToFill->append(m_langTable[i].m_transName);
		}
	}
	stringListToFill->sort();
}

QStringList LanguageManager::languageList(bool getTranslated)
{
	QStringList sl;
	for (int i = 0; i < m_langTable.size(); ++i)
	{
		if (getTranslated)
			sl<<m_langTable[i].m_transName;
		else
			sl<<m_langTable[i].m_name;
	}
	sl.sort();
	return sl;
}

void LanguageManager::printInstalledList()
{
	QFile f;
	f.open(stderr, QIODevice::WriteOnly);
	QTextStream ts(&f);
	for (int i = 0; i < m_langTable.size(); ++i)
	{
		ts << "  " << m_langTable[i].m_priAbbrev.leftJustified(8) << ": " << m_langTable[i].m_name;
		endl(ts);
	}
	endl(ts);
	f.close();
}

QString LanguageManager::numericSequence(QString seq)
{
	QString retSeq;
	const int nsBengali=0,nsDevanagari=1,nsGujarati=2,nsGurumukhi=3,nsKannada=4,nsMalayalam=5,nsOriya=6,nsTamil=7,nsTelugu=8,nsTibetan=9,nsLepcha=10;
	switch (1)
	{
		case nsBengali:
			retSeq+="";
			break;
		case nsDevanagari:
			retSeq+="०१२३४५६७८९";
			break;
		case nsGujarati:
			retSeq+="૦૧૨૩૪૫૬૭૮૯";
			break;
		case nsGurumukhi:
			retSeq+="੦੧੨੩੪੫੬੭੮੯";
			break;
		case nsKannada:
			retSeq+="";
			break;
		case nsMalayalam:
			retSeq+="";
			break;
		case nsOriya:
			retSeq+="";
			break;
		case nsTamil:
			retSeq+="";
			break;
		case nsTelugu:
			retSeq+="";
			break;
		case nsTibetan:
			retSeq+="";
			break;
		case nsLepcha:
			retSeq+="";
			break;
		default:
			retSeq="0123456789";
			break;
	}
	return retSeq;
}

bool LanguageManager::findSpellingDictionaries(QStringList &sl)
{
	sl=ScPaths::instance().spellDirs();
	if (sl.count()==0)
		return false;
	return true;
}

void LanguageManager::findSpellingDictionarySets(QStringList &dictionaryPaths, QMap<QString, QString> &dictionaryMap)
{
	for (int i=0; i<dictionaryPaths.count(); ++i)
	{
		// Find the dic and aff files in the location
		QDir dictLocation(dictionaryPaths.at(i));
		QStringList dictFilters("*.dic");
		QStringList dictList(dictLocation.entryList(dictFilters, QDir::Files, QDir::Name));
		dictList.replaceInStrings(".dic","");

		//Ensure we have aff+dic file pairs, remove any hyphenation dictionaries from the list
		QString dictName;
		foreach(dictName, dictList)
		{
			if (!QFile::exists(dictionaryPaths.at(i)+dictName+".aff"))
				dictList.removeAll(dictName);
			else
			{
				if (!dictionaryMap.contains(dictName))
				{
					if (dictName.length()<=5)
					{
						//QString shortAbbrev(LanguageManager::getShortAbbrevFromAbbrev(dictName));
						//qDebug()<<"findSpellingDictionarySets"<<dictName<<shortAbbrev;
						dictionaryMap.insert(dictName, dictionaryPaths.at(i)+dictName);
						//dictionaryMap.insert(shortAbbrev, dictionaryPaths.at(i)+dictName);
					}
					//qDebug()<<"Spell Finder:"<<dictName<<dictionaryPaths.at(i)+dictName;
					if (dictName.length()>5)
					{
						QString shortAbbrev(LanguageManager::getShortAbbrevFromAbbrev(dictName));
						//qDebug()<<shortAbbrev;
						dictionaryMap.insert(shortAbbrev, dictionaryPaths.at(i)+dictName);
					}
				}
			}
		}
//		qDebug()<<"Number of dictionaries/AFFs found in"<<dictionaryPaths.at(i)<<":"<<dictList.count();
	}
	//Now rescan dictionary map for any extra languages we can support with the files we have
	QMap<QString, QString>::iterator it = dictionaryMap.begin();
	while (it != dictionaryMap.end())
	{
		QString lang(it.key());
		if (lang.length()==5)
		{
			QString shortAbbrev(LanguageManager::getShortAbbrevFromAbbrev(lang));
			if (!dictionaryMap.contains(shortAbbrev))
			{
				//qDebug()<<"Adding extra spelling definitions for:"<<lang<<":"<<shortAbbrev;
				dictionaryMap.insert(shortAbbrev, it.value());
			}
			//else
				//qDebug()<<"Short abbreviation:"<<shortAbbrev<<"already exists for:"<<lang;
		}
		if (lang.length()==2)
		{
			QString altAbbrev(LanguageManager::getAlternativeAbbrevfromAbbrev(lang));
			if (!dictionaryMap.contains(altAbbrev))
			{
				//qDebug()<<"Adding extra spelling definitions for:"<<lang<<":"<<altAbbrev;
				dictionaryMap.insert(altAbbrev, it.value());
			}
			//else
				//qDebug()<<"Alt. abbreviation:"<<altAbbrev<<"already exists for:"<<lang;
		}
		++it;
	}
}

bool LanguageManager::findHyphDictionaries(QStringList& sl)
{
	sl=ScPaths::instance().hyphDirs();
	if (sl.count()==0)
		return false;
	return true;
}

void LanguageManager::findHyphDictionarySets(QStringList& dictionaryPaths, QMap<QString, QString>& dictionaryMap)
{
	for (int i=0; i<dictionaryPaths.count(); ++i)
	{
		// Find the dic and aff files in the location
		QDir dictLocation(dictionaryPaths.at(i));
		QStringList dictFilters("hyph*.dic");
		if (dictionaryPaths.at(i)==ScPaths::getUserDictDir(ScPaths::Hyph, false))
				dictFilters.append("*.dic");
		QStringList dictList(dictLocation.entryList(dictFilters, QDir::Files, QDir::Name));
		dictList.replaceInStrings(".dic","");
		foreach(QString dn, dictList)
		{
//			qDebug()<<dn;
			QString dictName;
			if (dn.startsWith("hyph_"))
				dictName=dn.section('_',1);
			else
				dictName=dn;
			if (!dictionaryMap.contains(dictName))
			{
				if (dictName.length()<=2)
				{
					QString shortAbbrev(LanguageManager::getShortAbbrevFromAbbrev(dictName));
					dictionaryMap.insert(dictName, dictionaryPaths.at(i)+dn+".dic");
				}
				if (dictName.length()>2)
				{
					QString shortAbbrev(LanguageManager::getShortAbbrevFromAbbrev(dictName));
					dictionaryMap.insert(shortAbbrev, dictionaryPaths.at(i)+dn+".dic");
				}
			}
		}
	}
}

LanguageManager::~LanguageManager()
{
	m_langTable.clear();
}

const QString LanguageManager::getHyphFilename(const QString & langAbbrev)
{
	int j=langTableIndex(langAbbrev);
	if (j!=-1 && m_langTable[j].m_hyphAvailable)
	{
//		qDebug()<<"Found requested hyphenation dictionary:"<<langAbbrev<<" : "<<langTable[j].m_hyphFile;
		return m_langTable[j].m_hyphFile;
	}
	return QString();
}

