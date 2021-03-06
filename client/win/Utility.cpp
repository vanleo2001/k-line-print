#include "StdAfx.h"
#include "KLinePrint.h"
#include "Utility.h"
#include "direct.h"
#include "io.h"
#include <fstream>
#include <sstream>

using namespace std;

Utility::Utility(void)
{
}

Utility::~Utility(void)
{
}

int CStringToInt(const CString& str)
{
	int ip = atoi((LPCTSTR)str);	
	return ip;
}

string CStringTostring(const CString& str)
{
	return string(LPCTSTR(str));
}

CString IntToCString(int nInt)
{
	CString str;

	str.Format(_T("%d"), nInt);

	return str;
}


int GetFileSize(string dir, string filename)
{
	vector<string> vecFiles;
	
	if(_chdir(dir.c_str())) return 0;
	else
	{
		long hFile;
		_finddata_t fileinfo;
		
		if((hFile = _findfirst(filename.c_str(), &fileinfo))!= -1)
		{
			_findclose(hFile);
			return fileinfo.size;			
		}

		return 0;
	}
}

vector<string> GetFiles(string dir, string filter, bool bDir)
{
	vector<string> vecFiles;
	
	if(_chdir(dir.c_str())) return vecFiles;
	else
	{
		long hFile;
		_finddata_t fileinfo;
		
		if((hFile = _findfirst(filter.c_str(), &fileinfo))!= -1)
		{
			do
			{
				if(!strcmp(".", fileinfo.name)) continue;
				if(!strcmp("..", fileinfo.name)) continue;
				
				if((bDir && (fileinfo.attrib & _A_SUBDIR))
					|| (!bDir && !(fileinfo.attrib & _A_SUBDIR)))
				{
					vecFiles.push_back(string(fileinfo.name));
				}
				
			} while(_findnext(hFile, &fileinfo) == 0);
			
			_findclose(hFile);
		}
	}
	
	return vecFiles;
}

string Utility::GetProgramPath()
{
	char szPath[MAX_PATH];   
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	string PathName(szPath);
	size_t npos = PathName.find_last_of('\\');
	return PathName.substr(0, npos + 1);
}

void Utility::GetInfoByPath(string path, string& rootdir, 
								string& market, string& contract, int& date)
{

	size_t posDot = path.find_last_of('.');
	size_t posBackSlash = path.find_last_of('\\');

	string filename = path.substr(posBackSlash+1, posDot - posBackSlash - 1);

	string sub1 = path.substr(0, posBackSlash);
	posBackSlash = sub1.find_last_of('\\');

	string sub2 = sub1.substr(0, posBackSlash);
	posBackSlash = sub2.find_last_of('\\');
	
	string sub3 = sub2.substr(0, posBackSlash);
	posBackSlash = sub3.find_last_of('\\');

	market = sub3.substr(sub3.size()-2, 2);

	rootdir = sub3.substr(0, posBackSlash);

	size_t posUnderScore = filename.find_last_of('_');

	string dateTmp = filename.substr(posUnderScore + 1);

	date = atoi(dateTmp.c_str());
	contract = filename.substr(0, posUnderScore);
}

vector<string> Utility::GetAllContractPath(string path)
{
	int date;
	char buf[512];

	vector<string> contracts;

	string rootdir, contract, var, market, final;

	GetInfoByPath(path, rootdir, market, contract, date);

	//	合约最后两位是月份
	var = contract.substr(0, contract.size() - 2);

	//	处理跨年合约
	if(var[var.size() - 1] == 'X'
		|| var[var.size() - 1] == 'Y')
	{
		var = var.substr(0, var.size() - 1);
	}

	// 在该目录搜索最大的文件(需要过滤掉MI,PI,VI文件)
	vector<string> vecFiles;

	sprintf(buf, "%s\\%s\\%s%d\\%d\\",
		rootdir.c_str(),
		market.c_str(),
		market.c_str(),
		date/100,
		date);

	string dir = buf;

	sprintf(buf, "%s*_%d.csv",
		var.c_str(),
		date);

	vecFiles = GetFiles(dir, buf, false);

	for(int i = 0; i < vecFiles.size(); i++)
	{
		// 过滤PI/MI/VI文件

		if((vecFiles[i].find("PI_") != string::npos)
		|| (vecFiles[i].find("MI_") != string::npos)
		|| (vecFiles[i].find("VI_") != string::npos)) continue;

		contracts.push_back(dir + vecFiles[i]);
	}

	return contracts;
}

string Utility::GetMajorContractPath(string path)
{
	int date;
	char buf[512];
	string rootdir, contract, var, market, final;

	GetInfoByPath(path, rootdir, market, contract, date);

	//	合约最后两位是月份
	var = contract.substr(0, contract.size() - 2);

	//	处理跨年合约
	if(var[var.size() - 1] == 'X'
		|| var[var.size() - 1] == 'Y')
	{
		var = var.substr(0, var.size() - 1);
	}

	// 在该目录搜索最大的文件(需要过滤掉MI,PI,VI文件)
	vector<string> vecFiles;

	sprintf(buf, "%s\\%s\\%s%d\\%d\\",
		rootdir.c_str(),
		market.c_str(),
		market.c_str(),
		date/100,
		date);

	string dir = buf;

	sprintf(buf, "%s*_%d.csv",
		var.c_str(),
		date);

	vecFiles = GetFiles(dir, buf, false);

	int nMaxFileSize = 0;
	int nMaxFileIndex = -1;

	for(int i = 0; i < vecFiles.size(); i++)
	{
		// 过滤PI/MI/VI文件

		if((vecFiles[i].find("PI_") != string::npos)
		|| (vecFiles[i].find("MI_") != string::npos)
		|| (vecFiles[i].find("VI_") != string::npos)) continue;

		int nFileSize = GetFileSize(dir, vecFiles[i]);

		if(nFileSize > nMaxFileSize)
		{
			nMaxFileSize = nFileSize;
			nMaxFileIndex = i;
		}	
	}

	if(nMaxFileIndex == -1) return "";
	else return dir + vecFiles[nMaxFileIndex];
}

string Utility::GetDayLinePath(string path)
{
	int date;
	char buf[512];
	string rootdir, contract, market;

	GetInfoByPath(path, rootdir, market, contract, date);

	sprintf(buf, "%s\\DAY\\%s%s.TXT", 
		rootdir.c_str(),
		market.c_str(),
		contract.c_str());

	return string(buf);
}

int Utility::GetDateByPath(string path)
{
	int date;
	string rootdir, contract, market;

	if(!path.size()) return -1;

	GetInfoByPath(path, rootdir, market, contract, date);
	return date;
}

string Utility::GetPathByDate(string org_path, int date)
{
	char buf[512];
	int tmp;
	string rootdir, contract, market;

	GetInfoByPath(org_path, rootdir, market, contract, tmp);

	sprintf(buf, "%s\\%s\\%s%d\\%d\\%s_%d.csv",
		rootdir.c_str(),
		market.c_str(),
		market.c_str(),
		date/100,
		date,
		contract.c_str(),
		date);

	return string(buf);
}

string Utility::GetNeighborCsvFile(string path, bool bPrev, bool bZhuLi)
{
	int date;
	char buf[512];
	string rootdir, contract, market;

	Utility::GetInfoByPath(path, rootdir, market, contract, date);

	int nNeighDate;

	if(bPrev)
		nNeighDate = CALENDAR.GetPrev(date);
	else
		nNeighDate = CALENDAR.GetNext(date); 

	if(nNeighDate < 0) return "";

	sprintf(buf, "%s\\%s\\%s%d\\%d\\%s_%d.csv", 
		rootdir.c_str(),
		market.c_str(),
		market.c_str(),
		nNeighDate/100,
		nNeighDate,
		contract.c_str(),
		nNeighDate);

	if(bZhuLi) /* 搜索主力合约 */
	{
		return Utility::GetMajorContractPath(buf);
	}
	else
	{
		return string(buf);
	}
}

int Utility::ConvDispTimeToContTime(int disp)
{
	int tmphour, tmpmin, tmpsec;

	tmphour = disp / 10000;
	tmpmin = disp % 10000 / 100;
	tmpsec = disp % 10000 % 100;

	return tmphour * 3600 + tmpmin * 60 + tmpsec;
}

int Utility::ConvContTimeToDispTime(int cont)
{
	int tmphour, tmpmin, tmpsec;

	tmphour = cont / 3600;
	tmpmin = cont % 3600 / 60;
	tmpsec = cont % 3600 % 60;

	return tmphour * 10000 + tmpmin * 100 + tmpsec;
	
}

int Utility::GetWeekDayByDate(int nDate)
{
	// w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

	int c = nDate / 1000000;
	int y = nDate % 1000000 / 10000;
	int m = nDate % 1000000 % 10000 / 100;
	int d = nDate % 1000000 % 10000 % 100;

	if(m <= 2)
	{
		m += 12;
		y -= 1;
	}

	int weekday =  (y + y/4 + c/4 - 2*c + 26*(m+1)/10 + d-1) % 7;

	if(weekday < 0)
	{
		weekday += 7;
	}

	return weekday;
}

#define CONFIG_FILE ((Utility::GetProgramPath() + "klinep.ini").c_str())

void WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
{
	char buf[64];
	sprintf(buf, "%d", nDefault);
	WritePrivateProfileStringA(lpAppName, lpKeyName, buf, lpFileName);
}

int Utility::ReadBalance()
{
	return GetPrivateProfileIntA("Exchange","Balance", 50000, CONFIG_FILE);
}

void Utility::WriteBalance(int nBalance)
{
	WritePrivateProfileIntA("Exchange","Balance", nBalance, CONFIG_FILE);
}

//	读入交易参数
TradeParam Utility::ReadExchangeConfig()
{
	TradeParam tp;

	tp.nFee = GetPrivateProfileIntA("Exchange","Fee", 10, CONFIG_FILE);
	tp.nMarginRate = GetPrivateProfileIntA("Exchange","Margin", 12, CONFIG_FILE);
	tp.nUnitsPerSlot = GetPrivateProfileIntA("Exchange","UnitsPerSlot", 5, CONFIG_FILE);
	tp.nDefaultSlots = GetPrivateProfileIntA("Exchange","DefaultSlots", 1, CONFIG_FILE);
	tp.nMaxLossStop = GetPrivateProfileIntA("Trade","MaxLoss", 200, CONFIG_FILE);
	tp.nMaxProfitStop = GetPrivateProfileIntA("Trade","MaxProfit", 200, CONFIG_FILE);
	tp.nTimeStop = GetPrivateProfileIntA("Trade","TimeStop", 120, CONFIG_FILE);
	tp.nMaxOpenTimes = GetPrivateProfileIntA("Trade","MaxOpenTimes", 3, CONFIG_FILE);
	tp.nMaxLossPerDay = GetPrivateProfileIntA("Trade","MaxLossPerDay", 1000, CONFIG_FILE);
	tp.bForceCloseOnBreak = GetPrivateProfileIntA("Trade","CloseOnBreak", 1, CONFIG_FILE);

	return tp;
}

void Utility::WriteExchangeConfig(TradeParam tp)
{
	WritePrivateProfileIntA("Exchange","Fee", tp.nFee, CONFIG_FILE);
	WritePrivateProfileIntA("Exchange","Margin", tp.nMarginRate, CONFIG_FILE);
	WritePrivateProfileIntA("Exchange","UnitsPerSlot", tp.nUnitsPerSlot, CONFIG_FILE);
	WritePrivateProfileIntA("Exchange","DefaultSlots", tp.nDefaultSlots, CONFIG_FILE);
	WritePrivateProfileIntA("Trade","MaxLoss", tp.nMaxLossStop, CONFIG_FILE);
	WritePrivateProfileIntA("Trade","MaxProfit", tp.nMaxProfitStop, CONFIG_FILE);
	WritePrivateProfileIntA("Trade","TimeStop", tp.nTimeStop, CONFIG_FILE);
	WritePrivateProfileIntA("Trade","MaxOpenTimes", tp.nMaxOpenTimes, CONFIG_FILE);
	WritePrivateProfileIntA("Trade","MaxLossPerDay", tp.nMaxLossPerDay, CONFIG_FILE);
	WritePrivateProfileIntA("Trade","CloseOnBreak", tp.bForceCloseOnBreak, CONFIG_FILE);
}

//	读入回放配置
PlaybackConfig Utility::ReadPlaybackConfig()
{
	PlaybackConfig pc;
	
	pc.nStartDate = GetPrivateProfileIntA("Playback","StartDate", 0, CONFIG_FILE);
	pc.nEndDate = GetPrivateProfileIntA("Playback","EndDate", 0, CONFIG_FILE);
	pc.nStartTime = GetPrivateProfileIntA("Playback","StartTime", 0, CONFIG_FILE);
	pc.nEndTime = GetPrivateProfileIntA("Playback","EndTime", 0, CONFIG_FILE);

	pc.bDayOfWeek[1] = GetPrivateProfileIntA("Playback","Monday", 0, CONFIG_FILE);
	pc.bDayOfWeek[2] = GetPrivateProfileIntA("Playback","Tuesday", 0, CONFIG_FILE);
	pc.bDayOfWeek[3] = GetPrivateProfileIntA("Playback","Wednesday", 0, CONFIG_FILE);
	pc.bDayOfWeek[4] = GetPrivateProfileIntA("Playback","Thursday", 0, CONFIG_FILE);
	pc.bDayOfWeek[5] = GetPrivateProfileIntA("Playback","Friday", 0, CONFIG_FILE);

	pc.bRealTime = GetPrivateProfileIntA("Playback","RealTime", 1, CONFIG_FILE);
	pc.nPlaySpeed = GetPrivateProfileIntA("Playback","Speed", 1, CONFIG_FILE);

	return pc;
}

//	保存回放配置
void Utility::SavePlaybackConfig(PlaybackConfig pc)
{
	WritePrivateProfileIntA("Playback","RealTime", pc.bRealTime, CONFIG_FILE);
	WritePrivateProfileIntA("Playback","Speed", pc.nPlaySpeed, CONFIG_FILE);

	WritePrivateProfileIntA("Playback","StartDate", pc.nStartDate, CONFIG_FILE);
	WritePrivateProfileIntA("Playback","EndDate", pc.nEndDate, CONFIG_FILE);
	WritePrivateProfileIntA("Playback","StartTime", pc.nStartTime, CONFIG_FILE);
	WritePrivateProfileIntA("Playback","EndTime", pc.nEndTime, CONFIG_FILE);

	WritePrivateProfileIntA("Playback","Monday", (int)pc.bDayOfWeek[1], CONFIG_FILE);
	WritePrivateProfileIntA("Playback","Tuesday", (int)pc.bDayOfWeek[2], CONFIG_FILE);
	WritePrivateProfileIntA("Playback","Wednesday", (int)pc.bDayOfWeek[3], CONFIG_FILE);
	WritePrivateProfileIntA("Playback","Thursday", (int)pc.bDayOfWeek[4], CONFIG_FILE);
	WritePrivateProfileIntA("Playback","Friday", (int)pc.bDayOfWeek[5], CONFIG_FILE);
}

void Utility::WriteLog(string path, TradeRecord tr)
{
	ofstream outfile;

	outfile.open(path.c_str(),ios::app);
	
	outfile << tr.nRealDate << " " << tr.nRealTime << " " << tr.sFileName << " " 
		<< tr.nSimuTime << " " << tr.nPrice << " " 
		<< tr.bBuy << " " << tr.bOpen << " " << tr.nSlot << " "
		<< tr.nFee << " " << tr.nProfit << endl;

	outfile.close();
}

void Utility::ReadLog(string path, vector<TradeRecord>& trs)
{
#define SZ 1024

	ifstream in;

	char buf[SZ];
	TradeRecord tr(TRADE_CODE_SUCCESS);

	in.open(path.c_str());

	while(in.getline(buf,SZ))
	{
		stringstream ss(buf);
		
		ss >> tr.nRealDate >> tr.nRealTime >> tr.sFileName
			>> tr.nSimuTime >> tr.nPrice >> tr.bBuy >> tr.bOpen
			>> tr.nSlot >> tr.nFee >> tr.nProfit;

		trs.push_back(tr);
	}
}


