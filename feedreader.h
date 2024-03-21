#ifndef FEEDREADER_FEEDREADER_H
#define FEEDREADER_FEEDREADER_H


#include "local_error.h"
#include <string>
#include <libxml/parser.h>


using namespace std;

#define BUF_SIZE 2048
#define RAW_DATA_FILENAME "raw_data.tmp"

#define FILE_FORMAT_ATOM 100
#define FILE_FORMAT_RSS 101

struct Link {
    string hostname;
    string port;
    string resource;
};

struct SourceInfo {
    xmlChar *link = NULL;
    xmlChar *title = NULL;
    xmlChar *author = NULL;
    xmlChar *email = NULL;
    xmlChar *timestamp = NULL;
};

struct userData {
    bool TFlag = false;
    bool aFlag = false;
    bool uFlag = false;
    bool helpFlag = false;
    string url;
    string certFile;
    string certAddress;
    string feedFilePath;
};

/**
 * Check for program parameters (arguments) that weren't processed yet - mostly URL or invalid arguments
 * @param argc arg count
 * @param argv arg values
 * @param userData data
 */
void checkOtherParamsProvided(int argc, char **argv, userData *userData);

/**
 * Parses arguments and fills the userData structure with provided information
 * @param argc arg count
 * @param argv arg values
 * @param userData data to fill
 */
void argParser(int argc, char **argv, userData *userData);

/**
 * Runs the feedreader for each link in the feedfile
 * @param userData data
 * @return 0 OK
 * @return 1 ERROR
 */
int processFeedFile(userData *userData);


#endif // FEEDREADER_FEEDREADER_H

