/******************************************************************************
 * Copyright 2019 Silas Parker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *****************************************************************************/
 
#ifndef MUNINNODE_H
#define MUNINNODE_H

#include <functional>
#include <WiFiServer.h>
#include <WiFiClient.h>

/** Maximum number of concurrent connections to the node. */
#define MAX_MUNIN_CLIENTS (2)

/** Munin Node server for ESP8266 or similar.
 * The server will listen on port 4949 (the default Munin port).
 * Only supports a single hostname (can't emulate other virtual nodes).
 */
class MuninNode
{
  public:
    /** Callback for the plugins "config" and "fetch" requests.
     * Callback is passed the plugin name and a stream that the results should
     * be written to.
     */
    typedef std::function<void(const char*,Stream&)> PluginCallback;
    
    /** Plugin for the munin node. */
    struct Plugin
    {
      const char* name;
      PluginCallback config;
      PluginCallback fetch;
    };
  
    /** Create the node with a list of plugins.
     * The plugin list must be terminated by a "null" plugin (one with a null
     * name). If the hostname is not provided, the system hostname will be used.
     */
    MuninNode(const Plugin* plugins, const char* hostname = NULL);
    
    /** Start the server (call within `setup`). */
    void begin();
    
    /** Process requests (call within `loop`). */
    void process();
    
    /** Helper function to output a line to the stream. */
    static void println(Stream& stream, const __FlashStringHelper* line);
    
    /** Helper function to output a name and value item to the stream. */
    template <class T>
    static void println(Stream& stream, const __FlashStringHelper* name, T value)
    {
      stream.print(name);
      stream.print(' ');
      stream.print(value);
      stream.print('\n');
    }
    
  private:
    /** Print a line with just a full stop (used to terminate lists). */
    static void printdotln(Stream& stream);
  
    /** Process a client requests. */
    void processClient(WiFiClient& client);
  
    WiFiServer      mServer;
    const Plugin*   mPlugins;
    String          mHostname;
    WiFiClient      mClients[MAX_MUNIN_CLIENTS];
};


#endif /* MUNINNODE_H */
