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
 
#include "MuninNode.h"
#include <ESP8266WiFi.h>


MuninNode::MuninNode(const Plugin* plugins, const char* hostname) :
  mServer(4949),
  mPlugins(plugins),
  mHostname(hostname),
  mClients()
{}


void MuninNode::begin()
{
  if (mHostname.length() == 0)
    mHostname = WiFi.hostname();
  mServer.begin();
}


void MuninNode::process()
{
  for (uint8_t i = 0; i != MAX_MUNIN_CLIENTS; ++i)
  {
    if (mClients[i].status() == CLOSED)
    {
      if (mServer.hasClient())
      {
        // Current client slot is closed and there's a new client connecting,
        // get the new client, return the opening comment, then process any
        // incoming request.
        mClients[i] = mServer.available();
        println(mClients[i], F("# munin node at"), mHostname);
        processClient(mClients[i]);
      }
    }
    else
    {
      processClient(mClients[i]);
    }
  }
}


void MuninNode::processClient(WiFiClient& client)
{
  if (client.available() < 3)
    return;
  
  String cmd = client.readStringUntil('\n');
  if (cmd == "." || cmd == "quit" || cmd == "quit ")
  {
    client.stop();
  }
  else if (cmd.startsWith("cap"))
  {
    println(client, F("cap"));
  }
  else if (cmd == "nodes")
  {
    client.print(mHostname);
    client.print('\n');
    printdotln(client);
  }
  else if (cmd == "version")
  {
    client.print(F("munins node on "));
    client.print(mHostname);
    client.print(F(" version:"));
    client.print("0.0.1");
    client.print('\n');
  }
  else if (cmd.startsWith("list"))
  {
    for (const Plugin* p = mPlugins; p != NULL && p->name != NULL; ++p)
    {
      if (p != mPlugins)
        client.print(' ');
      client.print(p->name);
    }
    client.print('\n');
  }
  else if (cmd.startsWith("config "))
  {
    const char* s = cmd.c_str() + 7;
    for (const Plugin* p = mPlugins; p != NULL && p->name != NULL; ++p)
    {
      if (strcmp(p->name, s) == 0)
      {
        if (p->config)
          p->config(p->name, client);
        printdotln(client);
        break;
      }
    }
  }
  else if (cmd.startsWith("fetch "))
  {
    const char* s = cmd.c_str() + 6;
    for (const Plugin* p = mPlugins; p != NULL && p->name != NULL; ++p)
    {
      if (strcmp(p->name, s) == 0)
      {
        if (p->fetch)
          p->fetch(p->name, client);
        printdotln(client);
        break;
      }
    }
  }
  else
  {
    println(client, F("# Unknown command. Try cap, list, nodes, config, fetch, version or quit"));
  }
  client.flush();
}


void MuninNode::println(Stream& stream, const __FlashStringHelper* line)
{
  stream.print(line);
  stream.print('\n');
}


void MuninNode::printdotln(Stream& stream)
{
  stream.print(".\n");
}


