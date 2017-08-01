/*
 *	寻路用数据结构
 */
#ifndef path_data_h__
#define path_data_h__

const int32 MAX_PATH_PIN = 20;      // 最大路径点

// 注意：pin点的id不要使用0

struct PathData
{
public:
    int32   CheckPointList[MAX_PATH_PIN];

    PathData()
    {
        Clear();
    }

    void Clear()
    {
        memset(CheckPointList, 0, sizeof(CheckPointList));
    }
};


PathData ParsePath(const char* pathString);

#endif // path_data_h__
