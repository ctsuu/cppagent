//
// Copyright Copyright 2009-2019, AMT – The Association For Manufacturing Technology (“AMT”)
// All rights reserved.
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#include "Cuti.h"

#include <iostream>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>
#include "test_globals.hpp"

using namespace std;
using namespace mtconnect;

string getFile(string file)
{
  string path = string(PROJECT_ROOT_DIR "/test/resources/") + file;
  ifstream ifs(path.c_str());
  stringstream stream;
  stream << ifs.rdbuf();
  return stream.str();
}


void fillErrorText(string &errorXml, const string &text)
{
  size_t pos = errorXml.find("</Error>");
  
  // Error in case </Error> was not found
  if (pos == string::npos)
  {
    return;
  }
  
  // Trim everything between >....</Error>
  size_t gT = pos;
  
  while (errorXml[gT - 1] != '>')
  {
    gT--;
  }
  
  errorXml.erase(gT, pos - gT);
  
  // Insert new text into pos
  pos = errorXml.find("</Error>");
  errorXml.insert(pos, text);
}


void fillAttribute(
                   string &xmlString,
                   const string &attribute,
                   const string &value
                   )
{
  size_t pos = xmlString.find(attribute + "=\"\"");
  
  if (pos == string::npos)
  {
    return;
  }
  else
  {
    pos += attribute.length() + 2;
  }
  
  xmlString.insert(pos, value);
}


void xpathTest(xmlDocPtr doc, const char *xpath,
               const char *expected,
               const std::string &file, int line,
               XCTestCase *self)
{
  xmlNodePtr root = xmlDocGetRootElement(doc);
  
  string path(xpath), attribute;
  size_t pos = path.find_first_of('@');
  
  while (pos != string::npos && attribute.empty())
  {
    if (xpath[pos - 1] != '[')
    {
      attribute = path.substr(pos + 1);
      path = path.substr(0, pos);
    }
    
    pos = path.find_first_of('@', pos + 1);
  }
  
  xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
  
  bool any = false;
  
  for (xmlNsPtr ns = root->nsDef; ns; ns = ns->next)
  {
    if (ns->prefix)
    {
      xmlXPathRegisterNs(xpathCtx, ns->prefix, ns->href);
      any = true;
    }
  }
  
  if (!any)
    xmlXPathRegisterNs(xpathCtx, BAD_CAST "m", root->ns->href);
  
  
  xmlXPathObjectPtr obj = xmlXPathEvalExpression(BAD_CAST path.c_str(), xpathCtx);
  
  if (!obj || !obj->nodesetval || obj->nodesetval->nodeNr == 0)
  {
    stringstream message;
    message << file << "(" << line << "): " <<
    "Xpath " << xpath << " did not match any nodes in XML document";
    CPPUNIT_FAIL(message.str());
    
    if (obj)
      xmlXPathFreeObject(obj);
    
    xmlXPathFreeContext(xpathCtx);
    return;
  }
  
  // Special case when no children are expected
  xmlNodePtr first = obj->nodesetval->nodeTab[0];
  
  if (expected == 0)
  {
    bool has_content = false;
    stringstream message;
    
    if (attribute.empty())
    {
      message << "Xpath " << xpath << " was not supposed to have any children.";
      
      for (xmlNodePtr child = first->children; !has_content && child; child = child->next)
      {
        if (child->type == XML_ELEMENT_NODE)
        {
          has_content = true;
        }
        else if (child->type == XML_TEXT_NODE)
        {
          string res = (const char *) child->content;
          has_content = !trim(res).empty();
        }
      }
    }
    else
    {
      message << "Xpath " << xpath << " was not supposed to have an attribute.";
      xmlChar *text = xmlGetProp(first, BAD_CAST attribute.c_str());
      
      if (text)
      {
        message << "Value was: " << text;
        has_content = true;
        xmlFree(text);
      }
    }
    
    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(xpathCtx);
    
    failIf(has_content, message.str(), file, line, self);
    return;
  }
  
  string actual;
  xmlChar *text = nullptr;
  
  switch (first->type)
  {
    case XML_ELEMENT_NODE:
      if (attribute.empty())
      {
        text = xmlNodeGetContent(first);
      }
      else if (first->properties)
      {
        text = xmlGetProp(first, BAD_CAST attribute.c_str());
      }
      
      if (text)
      {
        actual = (const char *) text;
        xmlFree(text);
      }
      
      break;
      
    case XML_ATTRIBUTE_NODE:
      actual = (const char *) first->content;
      break;
      
      
    case XML_TEXT_NODE:
      actual = (const char *) first->content;
      break;
      
    default:
      cerr << "Cannot handle type: " << first->type << endl;
  }
  
  trim(actual);
  
  string message = (string) "Incorrect value for path " + xpath;
  
  if (expected[0] != '!')
  {
    failNotEqualIf(actual != expected,
                   expected,
                   actual,
                   message,
                   file, line);
  }
  else
  {
    expected += 1;
    failNotEqualIf(actual == expected,
                   expected,
                   actual,
                   message, file, line);
  }
}


void xpathTestCount(xmlDocPtr doc, const char *xpath, int expected,
                    const std::string &file, int line, XCTestCase *self)

{
  xmlNodePtr root = xmlDocGetRootElement(doc);
  
  xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
  
  bool any = false;
  
  for (xmlNsPtr ns = root->nsDef; ns; ns = ns->next)
  {
    if (ns->prefix)
    {
      xmlXPathRegisterNs(xpathCtx, ns->prefix, ns->href);
      any = true;
    }
  }
  
  if (!any)
    xmlXPathRegisterNs(xpathCtx, BAD_CAST "m", root->ns->href);
  
  
  xmlXPathObjectPtr obj = xmlXPathEvalExpression(BAD_CAST xpath, xpathCtx);
  
  if (!obj || !obj->nodesetval)
  {
    stringstream message;
    message << "Xpath " << xpath << " did not match any nodes in XML document";
    failIf(true, message.str(), file, line);
    
    if (obj)
      xmlXPathFreeObject(obj);
    
    xmlXPathFreeContext(xpathCtx);
    return;
  }
  
  string message = (string) "Incorrect count of elements for path " + xpath;
  
  int actual = obj->nodesetval->nodeNr;
  failNotEqualIf(actual != expected,
                 intToString(expected),
                 intToString(actual),
                 message, file, line, self);
}


string &trim(string &str)
{
  char const *delims = " \t\r\n";
  
  // trim leading whitespace
  string::size_type not_white = str.find_first_not_of(delims);
  
  if (not_white == string::npos)
    str.erase();
  else if (not_white > 0)
    str.erase(0, not_white);
  
  if (not_white != string::npos)
  {
    // trim trailing whitespace
    not_white = str.find_last_not_of(delims);
    
    if (not_white < (str.length() - 1))
      str.erase(not_white + 1);
  }
  
  return str;
}

void failIf(bool condition, const std::string &message,
            const std::string &file, int line, XCTestCase *self)
{
#ifndef CUTI_NO_INTEGRATION
  stringstream msg;
  msg << file << "(" << line << "): Failed " << message;
  CPPUNIT_ASSERT_MESSAGE(msg.str(), !condition);
#else
  CPPUNIT_NS::Asserter::failIf(condition,
                               message,
                               CPPUNIT_NS::SourceLine(file, line));
#endif
}

void failNotEqualIf(bool condition,
                    const std::string &expected,
                    const std::string &actual,
                    const std::string &message,
                    const std::string &file, int line, XCTestCase *self)
{
#ifndef CUTI_NO_INTEGRATION
  stringstream msg;
  msg << file << "(" << line << "): Failed not equal " << message << "\n"
  << "  Expected: " << expected << "\n"
  << "  Actual: " << actual << endl;
  CPPUNIT_ASSERT_MESSAGE(msg.str(), !condition);
#else
  CPPUNIT_NS::Asserter::failNotEqualIf(condition,
                               expected,
                               actual,
                               CPPUNIT_NS::SourceLine(file, line),
                               message
                               );
#endif
}

void assertIf(bool condition, const std::string &message,
              const std::string &file, int line, XCTestCase *self)
{
#ifndef CUTI_NO_INTEGRATION
  stringstream msg;
  msg << file << "(" << line << "): Failed " << message;
  CPPUNIT_ASSERT_MESSAGE(msg.str(), condition);
#else
  CPPUNIT_NS::Asserter::failIf(!condition,
                               message,
                               CPPUNIT_NS::SourceLine(file, line));
#endif
}
