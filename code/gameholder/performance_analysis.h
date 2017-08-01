/*
 *	性能分析管理器
 */
#ifndef performance_analysis_manager_h__
#define performance_analysis_manager_h__

#ifdef WIN
#define PERFORMANCE_ANALYSIS
#endif

#ifdef PERFORMANCE_ANALYSIS
    #define PERFORM_BEGIN() PerformanceManager::Instance()->Begin(__FUNCTION__)
    #define PERFORM_END() PerformanceManager::Instance()->End(__FUNCTION__)
    #define PERFORM_NAME_BEGIN(x) PerformanceManager::Instance()->Begin(x)
    #define PERFORM_NAME_END(x) PerformanceManager::Instance()->End(x)
    #define PERFORM_HELPER  PerformanceHelper one(__FUNCTION__)
#else
    #define PERFORM_BEGIN()
    #define PERFORM_END()
    #define PERFORM_NAME_BEGIN(x)
    #define PERFORM_NAME_END(x)
    #define PERFORM_HELPER
#endif

#include "performance_manager.h"

class PerformanceHelper
{
public:
    PerformanceHelper(const std::string& name);
    ~PerformanceHelper();

private:
    std::string m_name;
};


#endif // performance_analysis_manager_h__
