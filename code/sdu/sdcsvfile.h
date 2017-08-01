#pragma once

#ifndef __SD_CSVFILE_H_
#define __SD_CSVFILE_H_
#include "sdtype.h"

namespace Crown 
{
	class  CSDCsvBase
	{
	public:
		CSDCsvBase(){}

		~CSDCsvBase(){}

		void    Clear(){m_vLine.resize(0);}

		// get line count
		int32 GetLineCount();

		// get item count of line
		int32 GetItemCount(int32 line);

		// insert a empty line in the "line" pos
		int32 InsertLine(int32 line);

		// insert a empty line to the end
		int32 AddLine();

		// insert a item data
		int32 InsertItem(int32 line, int32 item, const char *data);

		// delete a item data
		int32 DelItem(int32 line, int32 item);

		// delete one line data
		int32 DelLine(int32 line);

		//empty item
		int32 EmptyItem(int32 line, int32 item);

		// change a item data
		int32 ModifyItem(int32 line, int32 item, const char *data);

		// get a item data
		int32 ReadData(int32 line, int32 item, char *data, int32 size);

		template <typename T>
		int32 ReadData(int32 line, int32 item, T* obItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, T& obItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, char& cItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, byte& byItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, int16& shItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, uint16& wItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, int32& nItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, uint32& dwItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, real& fItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, double& fItem);

		template <typename T>
		int32 ReadData(int32 line, int32 item, std::string& strItem);

	protected:
		std::vector<std::vector<std::string> > m_vLine;
	private:
	};

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 /*line*/, int32 /*item*/, T& /*obItem*/ )
	{
		return -1;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 /*line*/, int32 /*item*/, T* /*obItem*/ )
	{
		return -1;  
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, char& cItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		cItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, byte& byItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		byItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, int16& shItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		shItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, uint16& wItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		wItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, int32& nItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		nItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, uint32& dwItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		dwItem = atoi(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, real& fItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		fItem = static_cast<real>(atof(itemvec[item].c_str()));
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, double& fItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		fItem = atof(itemvec[item].c_str());
		return 0;
	}

	template <typename T>
	int32 CSDCsvBase::ReadData( int32 line, int32 item, std::string& strItem )
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}
		std::vector<std::string> & itemvec = m_vLine[line];
		//get item
		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		strItem = itemvec[item];
		return 0;
	}

	class CSDCsvString:public CSDCsvBase
	{
		/*
		字符串格式:
		{.,..,...,...}...{.,..,...,...}
		*/
	public:
		CSDCsvString(){}
		~CSDCsvString(){}

		// init CCsvfile for a string
		int32 Init(const std::string& strData);

		//save data to a string
		int32 FlushData(std::string& strOutPut);

	protected:
	private:
	};

	class CSDCsvfile:public CSDCsvBase
	{
	public:
		CSDCsvfile(){}
		~CSDCsvfile(){}

		// init CCsvfile for a .csv file path
		int32 Init(const char *path);

		//save data to db file immediately
		int32 FlushData();

		int32 ReadCsvString(int32 line, int32 item, CSDCsvString& obCsvString);

	private:
		//file path
		std::string m_strFilePath;
	};
}
#endif //#ifndef _CSVFILE_H_
