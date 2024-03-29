// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#include "executable.h"
#include "file_utils.h"
#include "log.h"
#include "mongoose.h"
#include "settings.h"
#include "string_utils.h"

std::string g_webServerUrl;
struct mg_context* g_mongooseContext = 0;

static int log_message(const struct mg_connection* conn, const char *message) {
    LOG_WARNING << message;
    return 0;
}
static void end_request(const struct mg_connection* conn, int reply_status_code) {
    mg_request_info* request = mg_get_request_info(const_cast<mg_connection*>(conn));
    std::string message;
    message.append(request->request_method);
    message.append(" ");
    message.append(IntToString(reply_status_code));
    message.append(" ");
    message.append(request->uri);
    if (request->query_string) {
        message.append("?");
        message.append(request->query_string);
    }
    LOG_INFO << message;
}
bool StartWebServer() {
    LOG_INFO << "Starting Mongoose web server";
    json_value* settings = GetApplicationSettings();

    // Web server url from settings.
    std::string ipAddress = (*settings)["web_server"]["listen_on"][0];
    std::string port = (*settings)["web_server"]["listen_on"][1];
    long portInt = (*settings)["web_server"]["listen_on"][1];
    if (portInt)
        port = IntToString(portInt);
    if (ipAddress.empty() || port.empty()) {
        ipAddress = "127.0.0.1";
        port = "54007";
    }
    g_webServerUrl = "http://" + ipAddress + ":" + port + "/";
    LOG_INFO << "Web server url: " << g_webServerUrl;

    // WWW directory from settings.
    std::string wwwDirectory = (*settings)["web_server"]["www_directory"];
    if (wwwDirectory.empty())
        wwwDirectory = "www";
    wwwDirectory = GetExecutableDirectory().append("\\").append(wwwDirectory);
    // Mongoose won't accept "..\\" in a path, need a real path.
    wwwDirectory = GetRealPath(wwwDirectory);
    LOG_INFO << "WWW directory: " << wwwDirectory;

    // Index files from settings.
    const json_value indexFilesArray = (*settings)["web_server"]["index_files"];
    std::string indexFiles;
    for (int i = 0; i < 32; i++) {
        const char* file = indexFilesArray[i];
        if (strlen(file)) {
            if (indexFiles.length())
                indexFiles.append(",");
            indexFiles.append(file);
        }
    }
    if (indexFiles.empty())
        indexFiles = "index.html,index.php";
    LOG_INFO << "Index files: " << indexFiles;

    // CGI interpreter from settings.
    std::string cgiInterpreter = (*settings)["web_server"]["cgi_interpreter"];
    if (cgiInterpreter.empty())
        cgiInterpreter = "php\\php-cgi.exe";
    cgiInterpreter.insert(0, GetExecutableDirectory() + "\\");
    cgiInterpreter = GetRealPath(cgiInterpreter);
    LOG_INFO << "CGI interpreter: " << cgiInterpreter;

    // CGI extensions from settings.
    const json_value cgiExtensions =
            (*settings)["web_server"]["cgi_extensions"];
    std::string cgiPattern;
    for (int i = 0; i < 32; i++) {
        const char* extension = cgiExtensions[i];
        if (strlen(extension)) {
            if (cgiPattern.length())
                cgiPattern.append("|");
            cgiPattern.append("**.").append(extension).append("$");
        }
    }
    if (cgiPattern.empty())
        cgiPattern = "**.php$";
    LOG_INFO << "CGI pattern: " << cgiPattern;

    // Mongoose web server.
    std::string listening_ports = ipAddress + ":" + port;
    const char* options[] = {
        "document_root", wwwDirectory.c_str(),
        "listening_ports", listening_ports.c_str(),
        "index_files", indexFiles.c_str(),
        "cgi_interpreter", cgiInterpreter.c_str(),
        "cgi_pattern", cgiPattern.c_str(),
        NULL
    };
    mg_callbacks callbacks = {0};
    callbacks.log_message = &log_message;
    callbacks.end_request = &end_request;
    g_mongooseContext = mg_start(&callbacks, NULL, options);
    if (g_mongooseContext == NULL)
        return false;
    else
        return true;
}
void StopWebServer() {
    if (g_mongooseContext) {
        LOG_INFO << "Stopping Mongoose web server";
        mg_stop(g_mongooseContext);
    }
}
