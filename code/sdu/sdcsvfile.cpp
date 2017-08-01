#include "SDuPCH.h"
#include "sdcsvfile.h"
#include <iostream>
#include <fstream>

namespace Crown 
{
	int32 CSDCsvBase::GetLineCount()
	{
		return ((int32)m_vLine.size());
	}

	int32 CSDCsvBase::GetItemCount( int32 line )
	{
		if ((int32)m_vLine.size() <= line)
		{
			return -1;
		}

		return (int32)m_vLine[line].size();
	}

	int32 CSDCsvBase::InsertLine(int32 line)
	{
		std::vector<std::string> vec;
		if (line >= (int32)m_vLine.size())
		{
			//line too big, insert more lines
			int32 oldsize = (int32)m_vLine.size();
			int32 dif = line + 1 - oldsize;
			for(int32 i=0; i<dif; i++)
			{
				m_vLine.push_back(vec);
			}
		}
		else
		{
			//insert one line in the "line" place
			m_vLine.insert(m_vLine.begin()+line, vec);
		}
		return ((int32)m_vLine.size());
	}

	int32 CSDCsvBase::AddLine()
	{
		std::vector<std::string> vec;
		m_vLine.insert(m_vLine.begin()+m_vLine.size(), vec);
		return ((int32)m_vLine.size());
	}

	int32 CSDCsvBase::InsertItem(int32 line, int32 item, const char *data)
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}

		std::vector<std::string> & itemvec = m_vLine[line];
		std::string str = data;
		std::string s = "";
		if (item >= (int32)itemvec.size())
		{
			//item too big, insert some empty string
			int32 dif = item - (int32)itemvec.size();
			for (int32 i=0; i<dif; i++)
			{
				itemvec.push_back(s);
			}
			itemvec.push_back(str);
		}
		else
		{
			//insert string in the "item" place
			itemvec.insert(itemvec.begin()+item, str);
		}

		return ((int32)itemvec.size());
	}

	int32 CSDCsvBase::DelItem(int32 line, int32 item)
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}

		std::vector<std::string> &itemvec = m_vLine[line];

		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		itemvec.erase(itemvec.begin()+item);

		return ((int32)itemvec.size());
	}

	int32 CSDCsvBase::DelLine(int32 line)
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}

		std::vector<std::string> &itemvec = m_vLine[line];
		itemvec.clear();
		m_vLine.erase(m_vLine.begin()+line+1);

		return ((int32)itemvec.size());
	}

	int32 CSDCsvBase::ModifyItem(int32 line, int32 item, const char * data)
	{
		if (line >= (int32)m_vLine.size())
		{
			return -1;
		}

		std::vector<std::string> &itemvec = m_vLine[line];

		if (item >= (int32)itemvec.size())
		{
			return -1;
		}

		itemvec[item] = data;

		return ((int32)itemvec.size());
	}

	int32 CSDCsvBase::EmptyItem(int32 line, int32 item)
	{
		return ModifyItem(line, item, "");
	}

	int32 CSDCsvBase::ReadData( int32 line, int32 item, char *data, int32 size )
	{
		//get line
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
		if (size <= (int32)itemvec[item].size())
		{
			size = (int32)itemvec[item].size() - 1;
		}
		else
		{
			size = (int32)itemvec[item].size();
		}

		memset(data, 0, size);
		memcpy(data, itemvec[item].c_str(), size);
		data[size] = 0;
		return ((int32)itemvec.size()-item-1);
	}

	int32 CSDCsvString::Init( const std::string& strData )
	{
		std::string  strSubData;
		std::string  strTemp;
		int32   nStart  = 0;
		int32   nEnd    = 0;
		int32   nSubStart  = 0;
		int32   nSubEnd  = 0;
		nStart    = (int32)strData.find("{", nStart);
		std::vector<std::string> vecItem;
		while (nStart != (int32)strData.npos)
		{
			vecItem.resize(0);
			nStart ++;
			nEnd = (int32)strData.find("}", nStart);
			if (nEnd != (int32)strData.npos)
			{
				strSubData = strData.substr(nStart,nEnd - nStart);
				if (strSubData.length() > 0)
				{
					nSubStart = 0;
					do 
					{
						nSubEnd  = (int32)strSubData.find(",", nSubStart);
						strTemp = strSubData.substr(nSubStart,nSubEnd - nSubStart);
						vecItem.push_back(strTemp);
						nSubStart = nSubEnd + 1;
					} while (nSubEnd != (int32)strSubData.npos);
				}   
			}
			else
			{
				return -1;
			}
			m_vLine.push_back(vecItem);
			nStart  = (int32)strData.find("{", nEnd);
		}
		return ((int32)m_vLine.size());
	}

	int32 CSDCsvString::FlushData( std::string& /*strOutPut*/ )
	{
		return 0;
	}

	int32 CSDCsvfile::Init(const char *path)
	{
		try
		{
			std::locale loc = std::locale::global(std::locale(""));

			std::ifstream in;
			in.open(path);
			if(!in)
			{
				return -1;
			}
			in.seekg(4);
			while(!in.eof())
			{
				std::string linestr; 
				getline(in, linestr);
				if (linestr.empty())
				{
					continue;
				}

				//item vector, a item is one string
				std::vector<std::string> itemvec;
				//a item string start pos
				int32 start = 0;
				//a item string end pos
				int32 end = 0;
				while(true)
				{
					//item string
					std::string itemstr;
					//string begin has "
					if (linestr[start] == '"')
					{
						end = (int32)linestr.find("\"", end+1);
						if (end == (int32)std::string::npos)
						{
							itemstr = linestr.substr(start);
							itemvec.push_back(itemstr);
							break;
						}
						//find a match ", erase them from string
						else
						{
							start++;
							linestr.erase(end, 1);
						}
					}
					//find ","
					end = (int32)linestr.find(",", end);
					if (end == (int32)std::string::npos)
					{
						itemstr = linestr.substr(start);
						itemvec.push_back(itemstr);
						break;
					}
					itemstr = linestr.substr(start, end-start);
					itemvec.push_back(itemstr);
					start = ++end;
				}
				m_vLine.push_back(itemvec);
			}
			in.close();

			std::locale::global(loc);
		}
		catch (const std::exception& err)
		{//ifstream open file may throw ios_base::failure exception
			std::cerr << err.what() << std::endl;
			return -1;
		}
		m_strFilePath = path;
		return ((int32)m_vLine.size());
	}

	int32 CSDCsvfile::FlushData()
	{
		if (m_strFilePath.empty())
		{
			return -1;
		}
		try
		{
			std::ofstream out;
			out.open(m_strFilePath.c_str(), std::ios_base::trunc);
			if (!out)
			{
				return -1;
			}
			char szBOM[4] = {(char)0xEF, (char)0xBB, (char)0xBF, 0};   
			out << szBOM[3] << szBOM[2] << szBOM[1] << szBOM[0];
			for (int32 i=0; i < (int32)m_vLine.size(); i++)
			{
				std::vector<std::string> &itemvec = m_vLine[i];

				int32 j=0;
				for (j=0; j < (int32)itemvec.size()-1; j++)
				{
					std::string &itemstr = itemvec[j];
					int32 indexch;
					if ( (indexch = itemstr.find(",")) == (int32)std::string::npos)
					{
						out << "\"" << itemstr << "\"" << ",";
					}
					else
					{
						out << itemstr << ",";
					}
				}
				std::string &itemstr = itemvec[j];
				int32 indexch;
				if ( (indexch = (int32)itemstr.find(",")) == (int32)std::string::npos)
				{
					out << "\"" << itemstr << "\"" << "\n";
				}
				else
				{
					out << itemstr << "\n";
				}
			}
			out.close();
		}
		catch (const std::exception& err)
		{//ifstream open file may throw ios_base::failure exception
			std::cerr << err.what() << std::endl;
			return -1;
		}
		return 1;
	}

	int32 CSDCsvfile::ReadCsvString( int32 line, int32 item, CSDCsvString& obCsvString )
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
		obCsvString.Clear();
		return obCsvString.Init(itemvec[item]);
	}

}
