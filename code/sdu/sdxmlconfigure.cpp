#include "SDuPCH.h"
#include "sdxmlconfigure.h"
#include <string>
/*
namespace Crown
{

    bool CSDXmlConfigure::ReadConfig(const char * pCfgFile, const char* pLocate)
    {
        m_xmlDoc = new TiXmlDocument(pCfgFile);
        m_xmlDoc->LoadFile();
        m_curTree = & m_configTree;
        m_xmlDoc->Accept(this);
        return true;
    }

    bool CSDXmlConfigure::ReadConfigFromMem(const char * pBuffer)
    {
        m_xmlDoc = new TiXmlDocument();
        m_xmlDoc->Parse(pBuffer);
        m_curTree = & m_configTree;
        m_xmlDoc->Accept(this);
        return true;
    }

    bool CSDXmlConfigure::WriteConfig(const char *pCfgFile)
    {
        return true;
    }

    bool CSDXmlConfigure::Visit( const TiXmlText& text )
    {
        //std::cout << GetPropertyPath(&text) << std::endl;
        //std::cout << "value :" << text.Value() << std::endl;
        return true;
    }

    std::string CSDXmlConfigure::GetProperty(const std::string & prop)
    {
        PropertyMap::iterator itr = m_propertyMap.find(prop);
        if (itr != m_propertyMap.end())
        {
            return itr->second ;
        }
        return "";
    }

    void CSDXmlConfigure::SetProperty(const std::string & prop, const std::string & value)
    {
        m_propertyMap.insert(std::make_pair(prop,value));
    }

/// Visit an element.
    bool CSDXmlConfigure::VisitEnter( const TiXmlElement& element, const TiXmlAttribute* firstAttribute )
    {
        const TiXmlNode * pNode = element.FirstChild();

//           ConfigTree * pCfgTree = FindConfigTree(m_curTree,element.Value());
//           if (pCfgTree != nullptr)
//           {
//           m_curTree= pCfgTree;
//           }
//           else
        {
            ConfigTree * pTree = new ConfigTree();
            pTree->SetName (element.Value());

            if (m_curTree->GetXPath().empty())
            {
                pTree->SetXPath(element.Value());
            }
            else
            {
                pTree->SetXPath(m_curTree->GetXPath()+ "."+element.Value());
            }

            if (pNode->Type() == TiXmlNode::TEXT)
            {
                pTree->SetValue(element.GetText());
                m_propertyMap.insert(std::make_pair(pTree->GetXPath(), pTree->GetValue()));
            }

            m_curTree->AddChild( pTree );
            m_curTree = pTree;
        }
        return true;
    }

/// Visit an element.
    bool CSDXmlConfigure::VisitExit( const TiXmlElement& element )
    {
        if ( m_curTree->GetParent() != nullptr)
        {
            m_curTree = m_curTree->GetParent();
        }
        return true;
    }

    ConfigTree *  CSDXmlConfigure::FindConfigTree(ConfigTree * pTree, const std::string & name)
    {
        if (m_curTree != nullptr)
        {
            ConfigTree::ConfigTreeVec & children = pTree->GetChildren();
            for (ConfigTree::ConfigTreeVec::iterator itr = children.begin();
                    itr != children.end(); ++itr )
            {
                if ((*itr)->GetName() == name )
                {
                    return *itr;
                }
            }
        }
        return nullptr;
    }

    std::string  CSDXmlConfigure::GetPropertyPath(const TiXmlNode * node)
    {
        std::string xPath ;
        const TiXmlNode * pParent = node->Parent();
        while (pParent != nullptr)
        {
            if (xPath.empty())
            {
                xPath =  std::string(pParent->Value()) ;
            }
            else
            {
                xPath =  (std::string(pParent->Value()) + ".") + xPath;
            }
            pParent = pParent->Parent();
            if (pParent->Parent() == nullptr)
            {
                break;
            }

        }
        return xPath;
    }
} // namespace Crown
*/
