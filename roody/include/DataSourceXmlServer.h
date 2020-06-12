//
// DataSourceXmlServer.h
//
// $Id: DataSourceXmlServer.h 236 2010-10-19 18:41:15Z olchansk $
//

#include "DataSourceBase.h"

#include "mxml.h"

#include <stdint.h>

#include "TBufferXML.h"

extern "C" {
//#include "http-tiny-1.2/http_lib.h"
#include "http-tiny-1.2/http_lib.c"
}

class DataSourceXmlServer: public DataSourceBase
{
 private:
  std::string fBaseUrl;

 public:
  DataSourceXmlServer(const char* url) // ctor
    {
      fName = url;
      fBaseUrl = url;
    }

  ~DataSourceXmlServer() // dtor
    {
    }

  static std::string FetchUrl(const std::string& url)
  {
     char* data = NULL;
     int length = 0;
     char typebuf[256];
     char xurl[256];
     strcpy(xurl, url.c_str());
     char *filename = NULL;
     http_retcode rc = http_parse_url(xurl, &filename);
     if (rc != 0) {
        printf("Error: http_parse_url rc %d\n", rc);
        return "";
     }
     rc = http_get(filename, &data, &length, typebuf);
     if (rc != OK200) {
        printf("Error: http_get rc %d\n", rc);
        return "";
     }
     //printf("data %p, length %d, typebuf [%s]\n", data, length, typebuf);
     //printf("actual length %d\n", strlen(data));
     if (strlen(data) > length)
        data[length] = 0;
     std::string xdata = data;
     if (filename)
        free(filename);
     if (data)
        free(data);
     return xdata;
  }

#if 0
  static void ExecuteRemoteCommand(TSocket* socket, const char *line)
    {
      if (!socket->IsValid())
	return;
      
      // The line is executed by the CINT of the server
      socket->Send("Execute");
      socket->Send(line);
    }
#endif

  void Enumerate(ObjectList* list, const std::string& urlpath, int level, ObjectPath path)
  {
    std::string xmltext;
    if (level == 0)
       xmltext = FetchUrl(urlpath + "/index.xml");
    else
       xmltext = FetchUrl(urlpath + ".xml");
    if (xmltext.length() < 1)
       return;
    //printf("xmltext [%s]\n", xmltext.c_str());
    char error[256];
    int errline = 0;
    PMXML_NODE tree = mxml_parse_buffer(xmltext.c_str(), error, strlen(error), &errline);
    if (tree) {
       //mxml_write_tree("test.xml", tree);

       PMXML_NODE dir = mxml_find_node(tree, "xml/dir");
       int num_entries = 0;
       if (dir)
          num_entries = mxml_get_number_of_children(dir);
       for (int i=0; i<num_entries; i++) {
          PMXML_NODE node = mxml_subnode(dir, i);
          if (node) {
             //printf("subnode %d %p\n", i, node);
             const char* ntype     = mxml_get_name(node);
             const char* classname = mxml_get_value(mxml_find_node(node, "class"));
             const char* name      = mxml_get_value(mxml_find_node(node, "name"));
             
             printf("node type %s, class %s, name %s\n", ntype, classname, name);
             
             if (strcmp(ntype, "subdir")==0) {
                ObjectPath p = path;
                p.push_back(name);
                Enumerate(list, urlpath + "/" + name, level+1, p);
             } else {
                ObjectPath p = path;
                std::string s;
                s += name;
                //s += "(";
                //s += classname;
                //s += ")";
                p.push_back(s);
                list->push_back(p);
             }
          }
       }
          
       mxml_free_tree(tree);
    }
  }

  ObjectList GetList()
  {
    printf("GetList from %s\n", fName.c_str());
    ObjectList list;
    ObjectPath path;
    path.push_back(fBaseUrl);
    Enumerate(&list, fBaseUrl, 0, path);
    return list;
  }

  TObject* GetObject(const ObjectPath& path)
    {
      TObject *obj = NULL;
      const char *name = path.back().c_str();
      printf("Get object %s from %s\n", path.toString().c_str(), fName.c_str());

      std::string xmltext = FetchUrl(path.toString() + ".xml");
      if (xmltext.length() > 0) {
         //printf("xmltext [%s]\n", xmltext.c_str());
         char error[256];
         int errline = 0;
         PMXML_NODE tree = mxml_parse_buffer(xmltext.c_str(), error, strlen(error), &errline);
         if (tree) {
            //mxml_write_tree("test.xml", tree);
            PMXML_NODE xobj = mxml_find_node(tree, "xml/ROOTobject");
            if (xobj) {
               const char *s = strstr(xmltext.c_str(), "<Object");
               if (s) {
                  //printf("parse %s\n", s);
                  obj = TBufferXML::ConvertFromXML(s);
                  //printf("obj %p\n", obj);
                  if (obj) {
                     printf("Got object: ");
                     obj->Print();
                  }
               }
            }
            mxml_free_tree(tree);
         }
      }
      return obj;
    }

  void ResetAll()
  {
    printf("ResetAll %s\n", fName.c_str());
  }

  void ResetObject(const ObjectPath& path)
  {
    printf("ResetObject %s\n", path.toString().c_str());
  }

};

// end
