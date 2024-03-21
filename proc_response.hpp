#ifndef FEEDREADER_PROC_RESPONSE_H
#define FEEDREADER_PROC_RESPONSE_H

#include <libxml/tree.h>
#include "feedreader.hpp"

#define CLEAN_XML_FILENAME "cleanXML.tmp"

using namespace std;

/**
 * Removes the HTTP response header and checks if the remainder is an XML file
 * @return 0 remainder is an XML file
 * @return 1 remainder is not an XML file
 */
int removeHeaderAndCheckIfXML();

/**
 * Prints the title of the feed in the desired format
 * @param node xml file main node
 */
void printFeedTitle(xmlNode *node, xmlDoc *doc);

/**
 * Prints the entry with all requested parametres in the desired format
 * @param info entry information
 */
void printInsideEntryOrItem(SourceInfo *info);

/**
 * Determines the type of the provided XML (Atom/RSS)
 * @param root xml file main node
 * @return file format
 */
int getFileType(xmlNode *root);

/**
 * Fills the info structure with entry information (for each entry)
 * @param info entry information to be filled
 * @param userData data
 */
void getInsideEntry(SourceInfo *info, xmlDoc *doc, xmlNode *entryLevelNode, userData *userData);

/**
 * Fills the info structure with entry information (for each item)
 * @param info entry information to be filled
 * @param userData data
 */
void getInsideItem(SourceInfo *info, xmlDoc *doc, xmlNode *node, userData *userData);

/**
 * Frees the info structure
 */
void freeSourceInfo(SourceInfo *info);

/**
 * Prints the whole atom feed in the desired format
 */
void printAtomFeed(xmlNode *node, xmlDoc *doc, userData *userData);

/**
 * Prints the whole RSS feed in the desired format
 */
void printRssFeed(xmlNode *node, xmlDoc *doc, userData *userData);

/**
 * Processes XML file which was obtained from the provided URL
 * @param userData data
 * @return 0 OK
 * @return 1 ERROR
 */
int processXmlAndPrint(userData *userData);

#endif // FEEDREADER_PROC_RESPONSE_H
