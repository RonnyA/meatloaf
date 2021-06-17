// Hypertext Transfer Protocol

#ifndef MEATFILE_DEFINES_FSHTTP_H
#define MEATFILE_DEFINES_FSHTTP_H

#include "meat_io.h"
#include "../../include/make_unique.h"
#include "../../include/global_defines.h"
#include <ESP8266HTTPClient.h>


/********************************************************
 * File implementations
 ********************************************************/


class HttpFile: public MFile {

public:
    HttpFile(std::string path): MFile(path) {};

    bool isDirectory() override;
    MIstream* inputStream() override ; // has to return OPENED stream
    MOstream* outputStream() override ; // has to return OPENED stream
    time_t getLastWrite() override ;
    time_t getCreationTime() override ;
    bool rewindDirectory() override { return false; };
    MFile* getNextFileInDir() override { return nullptr; };
    bool mkDir() override { return false; };
    bool exists() override ;
    size_t size() override ;
    bool remove() override { return false; };
    bool rename(const char* dest) { return false; };
    MIstream* createIStream(MIstream* src);
    //void addHeader(const String& name, const String& value, bool first = false, bool replace = true);
};


/********************************************************
 * Streams
 ********************************************************/

class HttpIOStream: public MIstream, MOstream {
public:
    HttpIOStream(std::string& path) {
        url = path;
    }
    ~HttpIOStream() {
        close();
    }

    void close() override;
    bool open() override;

    // MStream methods
    bool seek(uint32_t pos, SeekMode mode) override;
    bool seek(uint32_t pos) override;
    size_t position() override;
    void flush() override;
    int available() override;
    size_t read(uint8_t* buf, size_t size) override;
    size_t write(const uint8_t *buf, size_t size) override;
    bool isOpen();

protected:
    std::string url;
    bool m_isOpen;
    int m_length;
    WiFiClient m_file;
    //WiFiClient m_client;
	HTTPClient m_http;
    int m_bytesAvailable = 0;
    int m_position = 0;
};


class HttpIStream: public MIstream {

public:
    HttpIStream(std::string path) {
        m_http.setUserAgent(USER_AGENT);
        m_http.setTimeout(10000);
        m_http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        m_http.setRedirectLimit(10);
        url = path;
    }
    // MStream methods
    bool seek(uint32_t pos, SeekMode mode) override;
    bool seek(uint32_t pos) override;
    size_t position() override;
    void close() override;
    bool open() override;
    ~HttpIStream() {
        close();
    }

    // MIstream methods
    int available() override;
    size_t read(uint8_t* buf, size_t size) override;
    bool isOpen();

protected:
    std::string url;
    bool m_isOpen;
    int m_length;
    WiFiClient m_file;
    //WiFiClient m_client;
	HTTPClient m_http;
    int m_bytesAvailable = 0;
    int m_position = 0;
    bool isFriendlySkipper = false;
};


class HttpOStream: public MOstream {

public:
    // MStream methods
    HttpOStream(std::string path) {
        m_http.setUserAgent(USER_AGENT);
        m_http.setTimeout(10000);
        m_http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        m_http.setRedirectLimit(10);
        m_http.setReuse(true);

        url = path;
    }
    bool seek(uint32_t pos, SeekMode mode) override;
    bool seek(uint32_t pos) override;
    size_t position() override;
    void close() override;
    bool open() override;
    ~HttpOStream() {
        close();
    }

    // MOstream methods
    size_t write(const uint8_t *buf, size_t size) override;
    void flush() override;
    bool isOpen();

protected:
    std::string url;
    bool m_isOpen;
    WiFiClient m_file;
    //WiFiClient m_client;
	HTTPClient m_http;
};


/********************************************************
 * FS
 ********************************************************/

class HttpFileSystem: public MFileSystem 
{
    MFile* getFile(std::string path) override {
        return new HttpFile(path);
    }

    bool handles(std::string name) {
        std::string pattern = "http:";
        return mstr::equals(name, pattern, false);
    }
public:
    HttpFileSystem(): MFileSystem("http") {};
};


#endif