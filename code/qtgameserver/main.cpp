#include "qtgameserver_pch.h"
#include "qtgameserver.h"
//#include <QtGui/QApplication>

uint32 gServerID = 1;

void ParseCmd(int argc, char* argv[])
{
    while(1) 
    {
        int c = getopt(argc, argv, "-i:");
        if(c == -1) break;
        switch(c) 
        {        
        case 'i': 
            gServerID = arg_to_int(optarg, 1, "i");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    ParseCmd(argc, argv);
    QApplication a(argc, argv);
    qtgameserver w;
    w.show();
    return a.exec();
}
