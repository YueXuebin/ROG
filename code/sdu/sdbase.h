#pragma once

#ifndef SDBASE_H
#define SDBASE_H
/**
* @file sdbase.h
* @author wangkui
* @brief Crown基础类
*  
**/
#include "sdtype.h"

namespace Crown {

//
// Version structure define of namespace Crown.
// Every module of Crown has its own version information.Make sure to get the 
// right version DLL and Head file.Otherwise there may be some exceptions
// while using the modules.
// Crown version information has the following part:
// Major Version     :This field will increase by 1 when the module was changed
//                    from beta version to final version.              
// Minor Version     :This field will increase by 1 every time the module's 
//                    interface changed and conflicted with the last version.
// Compatible Version:This field will increase by 1 every time the module's
//                    interface changed but compatible with the last version.
// Build Number      :This field will increase by 1 every official build.And
//                    never back to zero.
//
struct SSGDPVersion
{
	uint16 wMajorVersion;
	uint16 wMinorVersion;
	uint16 wCompatibleVersion;
	uint16 wBuildNumber;
};


const SSGDPVersion SGDP_VERSION = { 3, 0, 1, 2 };

const SSGDPVersion SDCOMMON_MODULE_VERSION = { 5, 0, 0, 1 }; 
//
// Name     : ISDBase
// Function : The base class of modules in namespace Crown.
//            Every module that exports from DLL was inherited from this class.
//
class ISDBase
{
public:
    virtual SDAPI ~ISDBase(void){}

    //
    // Name     : AddRef
    // Function : Add the reference of the module instance.
    //            This method should be called every time the CreateInstance
    //            method of the module was called.
    //
    virtual void SDAPI AddRef(void) = 0;

    //
    // Name         : QueryRef
    // Function     : Query the reference time of the module instance.
    // Return Values: QueryRef returns the reference number of the instance.
    //
    virtual uint32  SDAPI QueryRef(void) = 0;

    //
    // Name     : Release
    // Function : Decrease the reference number of the module by 1. When 
    //            decrease to zero, the instance of the module will be destroyed
    //
    virtual void SDAPI Release(void) = 0;

    //
    // Name         : GetVersion
    // Function     : Get the version information of the module.
    // Return Values: GetVersion returns the version structure of the module.
    //
    virtual SSGDPVersion SDAPI GetVersion(void) = 0;

    //
    // Name         : GetModuleName
    // Function     : Get the name of the module.
    // Return Values: GetModuleName returns the name string of the module.
    //
    virtual const char * SDAPI GetModuleName(void) = 0;
};



}

#endif
