#pragma once

#ifndef XMLCONFIGURE_H
#define XMLCONFIGURE_H
#include <iostream>
#include <map>
#include <vector>
#include "sdtype.h"

namespace Crown
{
    class ConfigTree
    {
    public:
        typedef std::vector<ConfigTree*> ConfigTreeVec;

        ConfigTree()
        {
            m_parent = nullptr;
        }

        ~ConfigTree()
        {
            for (ConfigTreeVec::iterator itr = m_children.begin(); itr != m_children.end(); ++itr)
            {
                CnDelete *itr;
            }
            m_children.clear();
        }

        void AddChild( ConfigTree* pChild )
        {
            pChild->SetParent(this);
            m_children.push_back(pChild);
        }

        inline void SetName( const std::string & name )
        {
            m_name = name;
        }

        inline void SetValue( const std::string & value )
        {
            m_value = value;
        }

        inline std::string GetValue()
        {
            return m_value;
        }

        inline void SetXPath( const std::string & xPath )
        {
            m_xpath = xPath;
        }

        inline std::string GetXPath()
        {
            return m_xpath;
        }

        inline std::string GetName()
        {
            return m_name;
        }

        ConfigTreeVec & GetChildren()
        {
            return m_children;
        }

        ConfigTree * GetParent()
        {
            return m_parent ;
        }

        void SetParent( ConfigTree* pParent )
        {
            m_parent = pParent;
        }

    private:
        std::vector<ConfigTree *> m_children;
        std::string m_name;
        std::string m_value;
        std::string m_xpath;
        ConfigTree * m_parent;
    };

//    class CSDXmlConfigure : public TiXmlVisitor
//    {
//    public:
//        typedef std::multimap<std::string, std::string> PropertyMap;
//
//        CSDXmlConfigure ( const char * pCfgFile = nullptr )
//        {
//            if(nullptr == pCfgFile)
//            {
//                return;
//            }
//            this->ReadConfig(pCfgFile);
//        }
//
//        CSDXmlConfigure ( const CSDXmlConfigure & rhs )
//        {
//        }
//
//        bool ReadConfig( const char * pCfgFile, const char* pLocate = nullptr );
//
//        bool ReadConfigFromMem( const char * pBuffer );
//
//        bool WriteConfig( const char * pCfgFile );
//
//        virtual bool Visit( const TiXmlText& text );
//
//        std::string GetProperty( const std::string & prop );
//
//        void SetProperty( const std::string & prop, const std::string & value );
//
//        /// Visit an element.
//        virtual bool VisitEnter( const TiXmlElement& element, const TiXmlAttribute * firstAttribute );
//
//        /// Visit an element.
//        virtual bool VisitExit( const TiXmlElement& element );
//
//        ConfigTree * FindConfigTree( ConfigTree * pTree, const std::string & name );
//
//        std::string GetPropertyPath( const TiXmlNode * node );
//
//    private:
//        PropertyMap     m_propertyMap;
//        ConfigTree      m_configTree;
//        ConfigTree *    m_curTree;
//        //TiXmlDocument * m_xmlDoc;
//    };

}
#endif // 

