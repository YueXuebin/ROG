#include "commonlib_pch.h"
#include "dbdriver.h"

// ==============================
// DBRecorder
// ==============================
DBRecorder::DBRecorder()
{
    mRow = 0;
    mCol = 0;
    mUnit = 0;
    mResult = 0;
}

DBRecorder::DBRecorder(QueryUnit* unit)
{
    mRow = 0;
    mCol = 0;

    mUnit = 0;
    mResult = 0;

    if(unit)
    {
        mUnit = unit;
        mResult = &unit->m_res;
    }
}

DBRecorder::~DBRecorder()
{
    Free();
}

DBRecorder& DBRecorder::operator=(QueryUnit* unit)
{
    Free();

    if(unit)
    {
        mUnit = unit;
        mResult = &unit->m_res;
    }
    return *this;
}

void DBRecorder::Free()
{
    mRow = 0;
    mCol = 0;

    if(mUnit)
    {
        CnDelete mUnit;
        mUnit = 0;
        mResult = 0;
    }
}

BOOL DBRecorder::HasRecord()
{
    if(!HasExecut())
    {
        return FALSE;
    }

    if(0 == GetRowNum())
    {
        return FALSE;
    }

    return TRUE;
}

uint64 DBRecorder::GetRowNum()
{
    if ( mResult == NULL )
    {
        return 0;
    }

    return mResult->mRowNum;
}

int64 DBRecorder::GetAffNum()
{
    if ( mResult == NULL )
    {
        return -1L;
    }

    return mResult->mAffNum;
}

BOOL DBRecorder::GetRecord(int8& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(uint8& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(int16& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(uint16& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(int32& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(uint32& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(int64& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(uint64& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(float& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(double& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(std::string& val, bool check)
{
    if(!GetRecord(mRow, mCol, val, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord(void* buf, uint32 bufLen, bool check)
{
    if(!GetRecord(mRow, mCol, buf, bufLen, check))
    {
        return FALSE;
    }

    Next();
    return TRUE;
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, int8& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_TINY) && !(mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = (int8)atoi(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (int8)atoi(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, uint8& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_TINY) && (mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = (uint8)strtoul(mysqlRow[col], NULL, 10);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (uint8)strtoul(mysqlRow[col], NULL, 10);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, int16& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_SHORT) && !(mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = (int16)atoi(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (int16)atoi(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, uint16& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_SHORT) && (mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = (uint16)strtoul(mysqlRow[col], NULL, 10);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (uint16)strtoul(mysqlRow[col], NULL, 10);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, int32& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( ((mResult->mFieldInfo[col].mType == MYSQL_TYPE_INT24) || (mResult->mFieldInfo[col].mType == MYSQL_TYPE_LONG)) && 
            !(mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = (int32)atoi(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (int32)atoi(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, uint32& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( ((mResult->mFieldInfo[col].mType == MYSQL_TYPE_INT24) || (mResult->mFieldInfo[col].mType == MYSQL_TYPE_LONG) || 
            (mResult->mFieldInfo[col].mType == MYSQL_TYPE_LONGLONG)) && (mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = (uint32)strtoul(mysqlRow[col], NULL, 10);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (uint32)strtoul(mysqlRow[col], NULL, 10);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, int64& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_LONGLONG) && !(mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = SDAtoi64(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = SDAtoi64(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, uint64& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_LONGLONG) && (mResult->mFieldInfo[col].mFlags & UNSIGNED_FLAG) )
        {
            val = Crown::SDAtou64(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = Crown::SDAtou64(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, float& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( mResult->mFieldInfo[col].mType == MYSQL_TYPE_FLOAT )
        {
            val = (float)atof(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (float)atof(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, double& val, bool check )
{
    MYSQL_ROW mysqlRow = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        if ( (mResult->mFieldInfo[col].mType == MYSQL_TYPE_DECIMAL) || (mResult->mFieldInfo[col].mType == MYSQL_TYPE_NEWDECIMAL) ||
            (mResult->mFieldInfo[col].mType == MYSQL_TYPE_DOUBLE) )
        {
            val = (double)atof(mysqlRow[col]);
            return TRUE;
        }
        else
        {
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
        }
    }
    else
    {
        val = (double)atof(mysqlRow[col]);
        return TRUE;
    }
}

BOOL DBRecorder::GetRecord(uint64 row, uint32 col, std::string& val, bool check)
{
    MYSQL_ROW mysqlRow = NULL;
    unsigned long * lengths = NULL;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if ( (lengths = mysql_fetch_lengths( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if(check)
    {
        switch( mResult->mFieldInfo[col].mType )
        {
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
            if(mysqlRow[col])
            {
                val = mysqlRow[col];
            }
            else
            {
                CnWarn("GetRecord row=%d col=% is NULL\n", row, col);
                val = "";
            }
            return TRUE;
            break;

        default:
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
            break;
        }
    }
    else
    {
        val = mysqlRow[col];
        return TRUE;
    }

}

BOOL DBRecorder::GetRecord( uint64 row, uint32 col, void* buf, uint32 bufLen, bool check )
{
    MYSQL_ROW mysqlRow = NULL;
    unsigned long * lengths = NULL;
    uint32 len = 0;

    if( !IsValidParam(row, col) )
    {
        OnError();
        return FALSE;
    }

    if ( !HasExecut() )
    {
        OnError();
        return FALSE;
    }

    memset( buf, 0, bufLen );

    mysql_data_seek( mResult->mReslut, row );

    if ( (mysqlRow = mysql_fetch_row( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if ( (lengths = mysql_fetch_lengths( mResult->mReslut )) == NULL )
    {
        OnError();
        CnError("%s: 查询的结果集为空. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
        return FALSE;
    }

    if ( bufLen < lengths[col] )
    {
        len = bufLen;
    }
    else
    {
        len = lengths[col];
    }

    if(check)
    {
        switch( mResult->mFieldInfo[col].mType )
        {
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
            memcpy( buf, mysqlRow[col], len );
            return TRUE;
            break;

        default:
            OnError();
            CnError("%s: 赋值字段类型和查询字段类型不匹配. 行号:%llu, 列号:%d\n", __FUNCTION__, row, col );
            return FALSE;
            break;
        }
    }
    else
    {
        memcpy( buf, mysqlRow[col], len );
        return TRUE;
    }
}

BOOL DBRecorder::HasExecut()
{
    //if ( mUnit->m_execute )
    //{
    //    //CnError("%s: 查询的结果集为空.\n", __FUNCTION__ );
    //    return FALSE;
    //}
    //return TRUE;

    if(mUnit)
    {
        return mUnit->m_execute;
    }

    return FALSE;
}

BOOL DBRecorder::IsValidParam( uint64 row, uint32 col )
{
    if ( (row<0) || (col<0) || (mResult==NULL) ||
        (row>=mResult->mRowNum) || (col>=mResult->mColNum) )
    {
        CnError("%s: 获取记录的参数无效. 行号:%llu, 列号:%d, 结果指针:%x\n",
            __FUNCTION__, row, col, (char*)mResult);
        return FALSE;
    }

    return TRUE;
}

void DBRecorder::Next()
{
    if((mCol+1) < mResult->mColNum)
    {
        mCol++;
    }
    else
    {
        if((mRow+1) < mResult->mRowNum)
        {
            mRow++;
            mCol = 0;
        }
    }
}

void DBRecorder::OnError()
{
    CnAssert(false);
}

// ==============================
// DBDriver
// ==============================

int IDBDriver::mCount = 0;

BOOL IDBDriver::IsConnected()
{
    return mIsConnected;
}

BOOL IDBDriver::Connect( const char* db, const char* server, const char* user, const char* password, uint32 port, const char* charset)
{
    my_bool reconnect = 1;

    // 如果已建立连接则断开连接
    if ( IsConnected() )
    {
        DisConnect();
    }

    // 初始化 MySQL 句柄
    if ( (mMySQL = mysql_init( NULL )) == NULL )
    {
        CnError("Error: %s init mysql handler failed.\n", __FUNCTION__);
        return FALSE;
    }

    // 判断 MySQL 是否初始化
    if ( mMySQL == NULL )
    {
        CnError("Error: %s mysql handler uninit.\n", __FUNCTION__);
        return FALSE;
    }

    // 配置连接参数
    if ( mysql_options( mMySQL, MYSQL_OPT_RECONNECT, &reconnect) )
    {
        CnError("Error: %s set mysql options reconnect falied.\n", __FUNCTION__);
        return FALSE;
    }

    // 设置字符集
    //if ( mysql_options( mMySQL, MYSQL_SET_CHARSET_NAME, charset) )
    //{
    //    CnError("Error: %s set mysql options set character falied.\n", __FUNCTION__);
    //    return FALSE;
    //}

    // 与数据库建立连接
    if ( !mysql_real_connect( mMySQL, server, user, password, db, port, NULL, CLIENT_MULTI_STATEMENTS ) )
    {
        printf( "Error: %s connect mysql failed: %s\n", __FUNCTION__, mysql_error( mMySQL ) );
        mysql_close (mMySQL);            // 不是有效连接，但包括传送给错误报告函数的错误信息
        mMySQL = NULL;
        mysql_library_end();            // 不调用，内存块仍保持分配状态（这不会增加应用程序使用的内存量，但某些内存泄漏检测器将抗议它）
        return FALSE;
    }

    if ( mysql_set_character_set(mMySQL, charset) )
    {
        mysql_close(mMySQL);
        mMySQL = NULL;

        CnError("Error: %s set mysql options set character falied.\n", __FUNCTION__);
        return FALSE;
    }

    mIsConnected = TRUE;

    return TRUE;
}

void IDBDriver::DisConnect()
{
    if(NULL != mMySQL)
    {
        mysql_close(mMySQL);
        mMySQL = NULL;
    }
    mIsConnected = FALSE;
}

BOOL IDBDriver::StartTransaction()
{
    if (mysql_autocommit(mMySQL, 0) != 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL IDBDriver::Rollback()
{
    if (mysql_rollback(mMySQL) != 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL IDBDriver::Commit()
{
    if (mysql_commit(mMySQL) != 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL IDBDriver::EndTransaction()
{
    if (mysql_autocommit(mMySQL, 1) != 0)
    {
        return FALSE;
    }
    return TRUE;
}

uint64 IDBDriver::EscapeString(char *to, const char *from, uint64 length)
{
    uint64 ret = mysql_real_escape_string(mMySQL, to, from, (unsigned long)length);
    
    uint32 toLen = Crown::SDStrlen(to);
    uint32 fromLen = Crown::SDStrlen(from);
    if(toLen >= MAX_SQLTXT_SAVE_LEN || fromLen >= MAX_SQLTXT_SAVE_LEN)
    {
        CnAssert(false);
        CnInfo("EscapeString error toLen=%d fromLen=%d\n", toLen, fromLen);
    }
    return ret;
}

bool IDBDriver::Query( QueryUnit* unit )
{
    if(!unit || !mIsConnected || !mMySQL )
    {
        CnError("%s: DBDriver 未初始化.\n", __FUNCTION__);
        return false;
    }

    // 测试是否与服务器断开，并重新发起连接
    if ( int32 ret = mysql_ping( mMySQL ) )
    {
        if(1 == ret)
        {
            CnError("%s: MySQL 超时重连.\n", __FUNCTION__);
        }
        else if ( ret == CR_COMMANDS_OUT_OF_SYNC )
        {
            CnError("%s: 命令以一个不适当的次序被执行.\n", __FUNCTION__);
            return false;
        }
        else if ( ret ==  CR_SERVER_GONE_ERROR )
        {
            CnError("%s: 客户端无法将问题发送至服务器.\n", __FUNCTION__);
            return false;
        }
        else if( ret  == CR_UNKNOWN_ERROR )
        {
            CnError("%s: 执行 SQL 语句时发生未知错误.\n",  __FUNCTION__);
            return false;
        }
        else
        {
            CnError("%s: 执行 mysql_ping 时返回一个未知错误码 %d.\n",  __FUNCTION__, ret);
            return false;
        }
    }

    // 判断操作是否正常执行
    if ( mysql_query( mMySQL, unit->m_sqlTxt ) != 0 )
    {
        uint32 error_id = mysql_errno(mMySQL);
        std::string error_str = mysql_error(mMySQL);

        bool isAllowableError = false;

        if(error_id == 1062)
        {
            std::string sql_str = unit->m_sqlTxt;
            // 重复的工会加入申请
            std::string sub_str = sql_str.substr(0, 25);
            if(sub_str == "INSERT INTO `guild_apply`")
            {
                isAllowableError = true;
            }
            // 创建相同的玩家名
            sub_str = sql_str.substr(0, 20);
            if(sub_str == "INSERT INTO `player`")
            {
                isAllowableError = true;
            }
        }

        if(!isAllowableError)
        {
            CnError("%s: error number: %d\n", __FUNCTION__, error_id );
            CnError("%s: error: %s\n", __FUNCTION__, error_str.c_str() );
            CnError("  %.510s\n", unit->m_sqlTxt );
        }
        else
        {
            CnInfo("%s\n", error_str.c_str());
            CnInfo("%s\n", unit->m_sqlTxt);
        }
        return false;
    }

    // 如果执行语句中包含多个结果集，则只保留最后的一个
    unit->m_res.mReslut = mysql_store_result( mMySQL ); // create table 这样的操作无返回结果集
    if ( unit->m_res.mReslut != NULL )
    {
        // 获得，行，列
        unit->m_res.mRowNum = mysql_num_rows( unit->m_res.mReslut );
        unit->m_res.mColNum = mysql_num_fields( unit->m_res.mReslut );
    }

    // ##################################################
    // CnError("# sqlTxt:%s\n\n", sqlTxt);
    // ##################################################

    // 获得 影响行
    unit->m_res.mAffNum = mysql_affected_rows(mMySQL);

    // [-1]成功无其他结果 [0]成功有其他结果 [>0] 错误
    for(;0 == mysql_next_result(mMySQL);)
    {
        MYSQL_RES* res = mysql_store_result( mMySQL );
        if(NULL != res)
        {
            mysql_free_result( res );
        }
    }

    // SELECT 无匹配, 或 INSERT, REPLACE, UPDATE, DELETE 会越过下面的操作
    // 因为他们有返回结果集，但是为 0 行A
    if ( unit->m_res.mRowNum > 0 )
    {
        // 创建存储结果集列信息组
        unit->m_res.mFieldInfo = CnNew FieldInfo[unit->m_res.mColNum];

        // 获得列信息
        for ( uint32 i=0; i<unit->m_res.mColNum; i++ )
        {
            MYSQL_FIELD *fd = mysql_fetch_field( unit->m_res.mReslut );
            unit->m_res.mFieldInfo[i].mType = fd->type;
            unit->m_res.mFieldInfo[i].mFlags = fd->flags;
        }
    }

    //DBRecorder recorder(res);
    //if(callback) callback(recorder, param);

    return true;
}

MYSQL* IDBDriver::GetMySQL()
{
    return mMySQL;
}

// ==============================
// DBDriver
// ==============================

DBDriverAsync::DBDriverAsync()
{
    mIsRun = false;
    m_WaitForHandle = NULL;
    mIsConnected = FALSE;
    mMySQL = NULL;
}

DBDriverAsync::~DBDriverAsync()
{
    DisConnect();
}

void    DBDriverAsync::SetExecutor(DBExecutor* executor)
{
    mExecutor = executor;
    mIsRun = true;
}

void DBDriverAsync::Run()
{
    QueryUnit* sqlcall = NULL;
    while(mIsRun)
    {
        m_WaitForHandle->Wait();

        if(NULL != (sqlcall = mExecutor->QueryPopFront()))
        {
            if(Query(sqlcall))
            {
                sqlcall->m_execute = TRUE;
            }
            else
            {
                sqlcall->m_execute = FALSE;
            }
            mExecutor->CallbackPushBack(sqlcall);
        }
        else
            mIsRun = false;          // 只有要停止线程时，才会只抛信号量不push数据库请求
    }
}

// ==============================
// DBDriverB
// ==============================

DBDriverBlock::DBDriverBlock()
{
    mIsConnected = FALSE;
    mMySQL = NULL;
}

DBDriverBlock::~DBDriverBlock()
{
    DisConnect();
}

// ==============================
// DBExecutor
// ==============================

DBExecutor::DBExecutor()
{
    m_Semaphore = NULL;
    mDBDriverAsyncList = 0;
    mDBDriverBlock = 0;
    mDBNum = 0;
    mConnNum = 0;
}

DBExecutor::~DBExecutor()
{
    UnInit();
}

BOOL    DBExecutor::Init(const char* signalname, uint8 connNum, const char* database, const char* ip, 
                         const char* user, const char* password, uint32 port, const char* charset)
{
    if(0 == connNum) return FALSE;

    memset(mSignalName, 0, 128);
    strncpy(mSignalName, signalname, 127);

    mConnNum = connNum;

    m_Semaphore = Crown::OpenNamedSDSemaphore(GetSignalName());
    if(!m_Semaphore)
    {
        m_Semaphore = Crown::CreateNamedSemaphore(GetSignalName());
        CnInfo("create semaphore %s\n", GetSignalName());
    }
    CnAssert(m_Semaphore);

    if(mDBDriverAsyncList)
    {
        CnDelete[] mDBDriverAsyncList;
        mDBDriverAsyncList = 0;
    }

    if(mDBDriverBlock)
    {
        CnDelete mDBDriverBlock;
        mDBDriverBlock = 0;
    }

    mDBDriverAsyncList = CnNew DBDriverAsync[connNum];
    mDBDriverBlock = CnNew DBDriverBlock;

    // 加载MySQL库
    if(!IDBDriver::Init()) goto _ERROR;

    // 初始化异步执行
    for (uint8 i=0; i<connNum; ++i)
    {
        if(!mDBDriverAsyncList[i].Connect(database, ip, user, password, port, charset))
        {
            goto _ERROR;
        }

        mDBDriverAsyncList[i].SetSemaphore(m_Semaphore);

        mDBDriverAsyncList[i].SetExecutor(this);
    }

    // 初始化阻塞执行
    if(!mDBDriverBlock->Connect(database, ip, user, password, port, charset))
    {
        goto _ERROR;
    }

    // 开始异步线程
    for (uint8 i=0; i<connNum; ++i)
    {
        mDBDriverAsyncList[i].Start();
    }

    return TRUE;

_ERROR:
    if(mDBDriverAsyncList)
    {
        CnDelete[] mDBDriverAsyncList;
        mDBDriverAsyncList = NULL;
    }
    if(mDBDriverBlock)
    {
        CnDelete mDBDriverBlock;
        mDBDriverBlock = NULL;
    }
    return FALSE;
}

void    DBExecutor::UnInit()
{
    // 停止异步线程
    for(int32 i=0; i<mConnNum; ++i)
    {
        m_Semaphore->Post();                // 抛信号量，不抛处理事件，使线程执行完所有事件后自动结束
    }

    for(int32 i=0; i<mConnNum; ++i)
    {
        mDBDriverAsyncList[i].Wait();       // 等待线程结束
    }

    // 清除查询列表
    for(std::list<QueryUnit*>::iterator it = mSQLList.begin(); it != mSQLList.end(); ++it)
    {
        CnDelete *it;
    }
    mSQLList.clear();

    // 清除回调列表
    for(std::list<QueryUnit*>::iterator it = mCallList.begin(); it != mCallList.end(); ++it)
    {
        CnDelete *it;
    }
    mCallList.clear();

    // 删除异步查询线程
    if(mDBDriverAsyncList)
    {
        CnDelete[] mDBDriverAsyncList;
    }

    // 删除阻塞查询线程
    CnDelete mDBDriverBlock;

    if(m_Semaphore)
    {
        DestoryNamedSemaphore(m_Semaphore);
        m_Semaphore = NULL;
    }
}

QueryUnit* DBExecutor::Query(const char *fmt, ...)
{
    QueryUnit* sqlcall = 0;
    int res = 0;
    va_list ap;
    if (!fmt) return 0;
    sqlcall = CnNew QueryUnit();
    va_start(ap, fmt);
    res = vsnprintf(sqlcall->m_sqlTxt, MAX_SQLTXT_SAVE_LEN, fmt, ap);
    va_end(ap);
    if (res < 0)
    {
        CnFatal("sql text truncated\n");
        goto _ERROR;
    }

    if(!mDBDriverBlock->Query(sqlcall))
    {
        goto _ERROR;
    }

    sqlcall->m_execute = TRUE;

    return sqlcall;
_ERROR:
    CnDelete sqlcall;
    sqlcall = 0;
    //CnAssert(false);        // 数据库调用错误
    return 0;
}

QueryUnit* DBExecutor::QueryMem(const char *fmt)
{
    QueryUnit* sqlcall = 0;
    int res = 0;

    if (!fmt)
    {
        return 0;
    }

    sqlcall = CnNew QueryUnit();

    res = strnlen(fmt, MAX_SQLTXT_SAVE_LEN-1);
    if (res < 1)
    {
        goto _ERROR;
    }

    memcpy(sqlcall->m_sqlTxt, fmt, res);
    sqlcall->m_sqlTxt[res] = '\0';

    if(!mDBDriverBlock->Query(sqlcall))
    {
        goto _ERROR;
    }

    sqlcall->m_execute = TRUE;

    return sqlcall;

_ERROR:
    CnDelete sqlcall;
    sqlcall = 0;
    //CnAssert(false);        // 数据库调用错误
    return 0;
}
