#include "proc_response.h"
#include "feedreader.h"
#include "local_error.h"
#include <fstream>
#include <cstring>
#include <iostream>

int removeHeaderAndCheckIfXML() {
    ifstream t(RAW_DATA_FILENAME);
    ofstream CleanXML(CLEAN_XML_FILENAME);
    string str;

    // allocate enough space for the large string (server response into string)
    t.seekg(0, ios::end);
    str.reserve(t.tellg());
    t.seekg(0, ios::beg);

    str.assign((istreambuf_iterator<char>(t)),
               istreambuf_iterator<char>());

    string httpHeaderEnd = "\r\n\r\n";
    size_t httpHeaderEndLen = 4;
    string cleanXMLString = str.substr(str.find(httpHeaderEnd) + httpHeaderEndLen);
    if (cleanXMLString.substr(0, 5) != "<?xml") // provided server response is not valid XML
        return handleError(ERR_XML_NOT_DETECTED);
    CleanXML << cleanXMLString;
    CleanXML.close();
    return 0;
}

void printFeedTitle(xmlNode *node, xmlDoc *doc) {
    xmlNode *currNode;
    for (currNode = node; currNode; currNode = currNode->next) {
        if (!xmlStrcmp(currNode->name, (const xmlChar *) "title")) {
            xmlChar *content = xmlNodeListGetString(doc, currNode->xmlChildrenNode, 1);
            printf("*** %s ***\n", content);
            xmlFree(content);
            return;
        }
        if (currNode->type == XML_ELEMENT_NODE) {
            printFeedTitle(currNode->children, doc);
        }
    }
}

void printInsideEntryOrItem(SourceInfo *info) {
    // entry should always have a title
    if (info->title == nullptr)
        return;
    printf("%s\n", info->title);

    if (info->link != nullptr)
        printf("URL: %s\n", info->link);

    if (info->author != nullptr) {
        if (info->email != nullptr) {
            // both author and email was provided
            printf("Autor: %s (%s)\n", info->author, info->email);
        } else {
            printf("Autor: %s\n", info->author);
        }
    }

    if (info->email != nullptr && info->author == nullptr) {
        printf("Autor: %s\n", info->email);
    }

    if (info->timestamp != nullptr) {
        printf("Aktualizace: %s\n", info->timestamp);
    }
    printf("\n");
}

int getFileType(xmlNode *root) {
    bool isRss = !xmlStrcmp(root->name, (const xmlChar *) "rss");
    bool isFeed = !xmlStrcmp(root->name, (const xmlChar *) "feed");
    bool isEntry = !xmlStrcmp(root->name, (const xmlChar *) "entry");

    if (isRss)
        return FILE_FORMAT_RSS;
    // Atom format can consist of a singular entry
    return (isFeed || isEntry) ? FILE_FORMAT_ATOM : ERR_FILE_FORMAT;
}

void getInsideEntry(SourceInfo *info, xmlDoc *doc, xmlNode *entryLevelNode, userData *userData) {
    xmlNode *cN;
    // traversing through XML on the entryLevelNode level
    for (cN = entryLevelNode; cN; cN = xmlNextElementSibling(cN)) {
        bool isTitle = !xmlStrcmp(cN->name, (const xmlChar *) "title");
        bool isLink = !xmlStrcmp(cN->name, (const xmlChar *) "link");
        bool isUpdated = !xmlStrcmp(cN->name, (const xmlChar *) "updated");
        bool isAuthor = !xmlStrcmp(cN->name, (const xmlChar *) "author");

        if (isTitle)
            info->title = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1);

        if (isLink && userData->uFlag)
            info->link = xmlGetProp(cN, (const xmlChar *) "href");

        // checking for name and email, either of them or both can be provided (both in an <author> tag)
        if (isAuthor && userData->aFlag) {
            xmlNode *avl = xmlFirstElementChild(cN);
            if (avl != nullptr) {
                xmlNode *it = nullptr;
                for (it = avl; it; it = it->next) {
                    if (!xmlStrcmp(it->name, (const xmlChar *) "name")) {
                        info->author = xmlNodeListGetString(doc, it->xmlChildrenNode, 1);
                    } else if (!xmlStrcmp(it->name, (const xmlChar *) "email")) {
                        info->email = xmlNodeListGetString(doc, it->xmlChildrenNode, 1);
                    }
                }
            }
        }

        if (isUpdated && userData->TFlag) {
            info->timestamp = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1);
        }
    }
}

void getInsideItem(SourceInfo *info, xmlDoc *doc, xmlNode *node, userData *userData) {
    xmlNode *cN;
    for (cN = node; cN; cN = xmlNextElementSibling(cN)) {
        bool isTitle = !xmlStrcmp(cN->name, (const xmlChar *) "title");
        bool isLink = !xmlStrcmp(cN->name, (const xmlChar *) "link");
        bool isPublished = !xmlStrcmp(cN->name, (const xmlChar *) "pubDate");
        bool isUpdated = !xmlStrcmp(cN->name, (const xmlChar *) "lastBuildDate");
        bool isEditor = !xmlStrcmp(cN->name, (const xmlChar *) "managingEditor");

        if (isTitle) { info->title = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1); }

        if (isLink && userData->uFlag) info->link = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1);

        if (isEditor && userData->aFlag) info->author = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1);

        if (isPublished && userData->TFlag) info->timestamp = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1);
        if (isUpdated && userData->TFlag) info->timestamp = xmlNodeListGetString(doc, cN->xmlChildrenNode, 1);
    }
}

void freeSourceInfo(SourceInfo *info) {
    xmlFree(info->title);
    xmlFree(info->author);
    xmlFree(info->email);
    xmlFree(info->timestamp);
    xmlFree(info->link);
}

void printAtomFeed(xmlNode *node, xmlDoc *doc, userData *userData) {
    // iterate over Atom entries and print titles and additional information
    bool isFeed = !xmlStrcmp(node->name, (const xmlChar *) "feed");
    if (isFeed)
        printAtomFeed(xmlFirstElementChild(node), doc, userData);

    xmlNode *currNode;
    SourceInfo info;
    for (currNode = node; currNode; ) {
        bool isEntry = !xmlStrcmp(currNode->name, (const xmlChar *) "entry");
        if (isEntry) {
            getInsideEntry(&info, doc, xmlFirstElementChild(currNode), userData);
            printInsideEntryOrItem(&info);
            freeSourceInfo(&info);
        }
        currNode = xmlNextElementSibling(currNode);
    }
}

void printRssFeed(xmlNode *node, xmlDoc *doc, userData *userData) {
    // iterate over RSS entries and print titles and additional information
    bool isChannel = !xmlStrcmp(node->name, (const xmlChar *) "channel");
    bool isRssTag = !xmlStrcmp(node->name, (const xmlChar *) "rss");
    if (isRssTag)
        printRssFeed(xmlFirstElementChild(xmlFirstElementChild(node)), doc, userData);
    if (isChannel)
        printRssFeed(xmlFirstElementChild(node), doc, userData);

    xmlNode *currNode;
    SourceInfo info;
    for (currNode = node; currNode; currNode = xmlNextElementSibling(currNode)) {
        bool isItem = !xmlStrcmp(currNode->name, (const xmlChar *) "item");
        if (isItem) {
            getInsideItem(&info, doc, xmlFirstElementChild(currNode), userData);
            printInsideEntryOrItem(&info);
            freeSourceInfo(&info);
        }
    }
}

int processXmlAndPrint(userData *userData) {
    if (removeHeaderAndCheckIfXML())
        return 1;

    // parsing actual XML file
    xmlDoc *doc = xmlReadFile(CLEAN_XML_FILENAME, NULL, 0);
    if (doc == nullptr)
        return handleError(ERR_XML_FORMAT);

    xmlNode *root= xmlDocGetRootElement(doc);

    int fileType = getFileType(root);

    // print main title only first
    printFeedTitle(root, doc);

    // print remaining titles with infos
    if (fileType == FILE_FORMAT_ATOM) {
        printAtomFeed(root, doc, userData);
    } else if (fileType == FILE_FORMAT_RSS) {
        printRssFeed(root, doc, userData);
    } else {
        return handleError(ERR_FILE_FORMAT);
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}