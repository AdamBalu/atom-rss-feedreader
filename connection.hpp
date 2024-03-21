#ifndef FEEDREADER_CONNECTION_H
#define FEEDREADER_CONNECTION_H

#include "feedreader.hpp"

/**
 * initializes the OpenSSL library which is used to set up a connection
 */
void initOpensslLib();

/**
 * Finds and returns the port of given URL
 * @param link link
 * @param hN hostname with resource (link without http:// | https:// prefix)
 * @param secureConn is connection secure
 * @return PORT
 */
void getPort(Link *link, const string& hN, bool secureConn);

/**
 * Fills the Link structure with suitable information
 * @param link Link to be filled
 * @param url
 * @param isSecure security, set to true if the connection is secure
 * @return 0 OK
 * @return 1 ERROR
 */
int setHostnamePortResource(Link *link, const string& url, bool *isSecure);

/**
 * Sets up a connection to a URL and writes the data which is returned by the server into a file
 * @param userData data
 * @return 0 OK
 * @return 1 ERROR
 */
int establishConnectionAndGetData(userData *userData);

/**
 * Sets up an unsecured connection to a provided server
 * @param link url
 * @return 0 OK
 * @return 1 ERROR
 */
int establishUnsecuredConnectionAndGetData(const Link& link);

/**
 * Checks if server provided response OK (HTTP/1.1|1.0 200 OK)
 * @return 0 OK
 * @return 1 ERROR
 */
int checkServerResponse();

/**
 * Sets up a secured connection to a provided server
 * @param link url
 * @param userData data
 * @return 0 OK
 * @return 1 ERROR
 */
int establishSecuredConnectionAndGetData(const Link& link, userData *userData);

#endif // FEEDREADER_CONNECTION_H
