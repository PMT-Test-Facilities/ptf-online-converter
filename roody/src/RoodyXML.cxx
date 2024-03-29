//
// RoodyXML.cxx
//
// $Id: RoodyXML.cxx 242 2012-06-14 17:31:22Z olchansk $
//

#include <iostream>
#include <ios>
#include <time.h>
#include <string.h>

#include "RoodyXML.h"

RoodyXML::RoodyXML()
{
  version_ = "1.0";
  encoding_ = "ISO-8859-1";
}

RoodyXML::~RoodyXML()
{}

std::ofstream &RoodyXML::OpenFileForWrite( char const *file )
{
  if( outputFile_.is_open() )outputFile_.close();
  std::ios_base::openmode mode = std::ios_base::out|std::ios_base::trunc;
  outputFile_.open( file, mode );
  //
  if( outputFile_ )
  {
    outputFile_ << "<?xml version=\"" << version_
                << "\" encoding=\"" << encoding_ << "\"?>\n";
    /*
      << "<!DOCTYPE roody [\n"
      << "  <!ELEMENT roody (online?,file*,graphics?,canvas*,group*)>\n"
      << "  <!ELEMENT online (host,port?)>\n"
      << "  <!ELEMENT graphics (zones?,refresh?)>\n"
      << "  <!ELEMENT zones (#PCDATA)>\n"
      << "  <!ELEMENT refresh (#PCDATA)>\n"
      << "  <!ELEMENT canvas (zones?,histogram*)>\n"
      << "  <!ELEMENT group (name,histogram*)>\n"
      << "  <!ELEMENT name (#PCDATA)>\n"
      << "  <!ELEMENT histogram (name,source)>\n"
      << "  <!ELEMENT source (#PCDATA)>\n"
      << "  <!ELEMENT host (#PCDATA)>\n"
      << "  <!ELEMENT port (#PCDATA)>\n"
      << "  <!ELEMENT file (#PCDATA)>\n"
      << "]>\n";
    */
    time_t now;
    time( &now );
    std::string temp( ctime(&now) );
    temp.erase( temp.begin()+temp.size()-1 );
    outputFile_ << "<!-- created by Roody on " << temp << " -->\n";
  }
  return outputFile_;
}

std::string RoodyXML::Encode( const std::string &src )
{
  std::string result;
  std::size_t length = src.size();
  for( std::size_t i=0; i<length; ++i )
  {
    switch ( src[i] )
    {
      case '<':
        result += "&lt;";
        break;
      case '>':
        result += "&gt;";
        break;
      case '&':
        result += "&amp;";
        break;
      case '\"':
        result += "&quot;";
        break;
      case '\'':
        result += "&apos;";
        break;
      default:
        result += src[i];
    }
  }  
  return result;
}

bool RoodyXML::OpenFileForRead( const char *file )
{
  char error[256];
  int errline = 0;
  documentNode_ = mxml_parse_file( const_cast<char*>(file), error, sizeof(error), &errline );
  if( !documentNode_ )
  {
    std::cerr << error << std::endl;
    return false;
  }
  int i = FindNode( documentNode_, 0, "roody" );
  if( i < 0 )
  {
    std::cerr << "roody node not found in file: \"" << file << "\"" << std::endl;
    return false;
  }
  roodyNode_ = &documentNode_->child[i];
  return true;
}

int RoodyXML::FindNode( PMXML_NODE parent, int startIndex, char const *nameToFind )
{
  int numberOfSiblings = parent->n_children;
  for( int i=startIndex; i<numberOfSiblings; ++i )
  {
    if( !strcmp(parent->child[i].name,nameToFind) )return i;
  }
  return -1;
}

PMXML_NODE RoodyXML::GetNode(PMXML_NODE parent, int index)
{
  if (parent==0 || index<0)
    return 0;

  return &parent->child[index];
}

std::string RoodyXML::GetNodeString(PMXML_NODE parent, int index)
{
  if (parent==0 || index<0)
    return "";

  const char* s = parent->child[index].value;
  if (!s)
    s = "";

  return s;
}

/*
void RoodyXML::PrintTree( int level )
{
  PrintTree( level, documentNode_ );
}

void RoodyXML::PrintTree( int level, PMXML_NODE current )
{
  while( current )
  {
    for( int i=0; i<level; ++i )std::cout << "  ";
    std::cout << "current " << current->name << ", type " << current->node_type
              << ", children " << (void*)current << "\n";
    if( current->children )PrintTree( level+1, current->children );
    current = current->next;
  }
}
*/
// end of file
