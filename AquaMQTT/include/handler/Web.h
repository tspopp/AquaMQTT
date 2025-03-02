#ifndef AQUAMQTT_WEB_H
#define AQUAMQTT_WEB_H
#include <WebServer.h>
#include <WiFi.h>
#include "config/config.h"
namespace aquamqtt
{
class WebHandler
{
public:
    WebHandler();
    void setup();
    void loop();

private:
    // static void webServerHandleClient();
    static void      handleRoot();
    static void      handleNotFound();
    static void      handleMQTT();
    static void      handleSaveConfig();
    static void      handleTools();
    static void      handleReboot();
    static void      handleUpdate();
    static void      handleFSbrowser();
    static void      handleReadfile();
    static void      handleAPIWeb();
    static void      handleAPI();
    static void      handleSavePanel();
    static WebServer serverWeb;

    // private:
    // WiFiServer serverWeb;
    // WebServer serverWeb;
};
}  // namespace aquamqtt
#endif  // AQUAMQTT_WEB_H