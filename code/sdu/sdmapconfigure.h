#pragma once

#ifndef SDMAPCONFIGURE_H
#define SDMAPCONFIGURE_H
#include "sdtype.h"
#include "sdhashmap.h"
namespace Crown
{

    class CSDMapConfigure
    {
    public:

    	virtual ~CSDMapConfigure() {}

	virtual bool ReadConfig( const char * /*pCfgFile*/, const char * /*pLocate*/ = nullptr )
        {
            return true;
        }

	virtual bool WriteConfig( const char * /*pCfgFile*/ )
        {
            return true;
        }

        virtual std::string GetProperty( const std::string & prop )
        {
            PropertyMap::iterator itr = m_properties.find(prop);
            if (itr != m_properties.end())
            {
                return itr->second;
            }
            return "";
        }

        virtual void SetProperty( const std::string & prop, const std::string & value )
        {
            m_properties.insert(std::make_pair(prop, value));
        }

    private:
		typedef std::map<std::string, std::string> PropertyMap;
        PropertyMap m_properties;
    };
}

#endif // 
