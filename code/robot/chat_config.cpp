#include "robot_pch.h"
#include "chat_config.h"
#include <fstream>
using namespace std;

IMPLEMENT_SINGLETON(ChatConfig)

ChatConfig::ChatConfig()
{
    m_chatList.clear();
}

ChatConfig::~ChatConfig()
{

}

bool ChatConfig::LoadConfig()
{
    int32 Datalen = -1;

#ifdef _WIN32
    std::string chat_config_filename = "../robot/chat_config.txt";
#else
    std::string chat_config_filename = "../res/robot/chat_config.txt";
#endif
    ifstream fin(chat_config_filename.c_str());

    if(!fin)
    {
        cerr<<"Can not open char_config file "<< chat_config_filename <<endl;        
        return false;
    };

    while(!fin.eof())
    {
        Datalen++;
        if(Datalen > 500)
        {
            cerr<<"Too much Data!"<<endl;            
            return false;
        };
        std::string strChat;
        fin >> strChat;
        m_chatList[Datalen] = strChat;
    };

    fin.close();
    return true;
}

