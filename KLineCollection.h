#pragma once

#include "TickReader.h"
#include <vector>
#include <map>
using namespace std;

class KLine
{
public:
	int time;
	int high;
	int low;
	int open;
	int close;
	int vol;		//	当前K线的成交量
	int vol_acc;	//	累积成交量
	int avg;		//	均价线
	double ma5;
	double ma10;
	double ma20;
	double ma60;
};

class KLineCollection : public vector<KLine>
{
private:

	//	根据时间建立索引，用于快速查找
	map<int, int>		m_mapTime2Idx;

	//	用于在分钟图上显示日线MA
	map<int, string>	m_mapKeyPrice;

	//	K线周期(以秒为单位, -1为日线)
	int					m_nKLinePeriod;
	
public:
	KLineCollection(void);
	~KLineCollection(void);

	//	获取指定时间/日期的K线数据
	KLine GetKLineByTime(int nTime);

	//	设置K线周期(以秒为单位, -1为日线)
	void SetPeriod(int nPeriod) { m_nKLinePeriod = nPeriod; }

	//	统一接口用于创建索引
	void AddToTail(KLine kline);

	//	获取价格和成交量的最大最小值
	void GetPriceVolRange(int nStartIdx, int nEndIdx, int& nHighPr, int& nLowPr, int& nMaxVol);

	//	开始接收分笔数据
	void StartQuote(Tick tick);

	//	接收分笔数据
	void Quote(Tick tick);

	//	结束接收分笔数据
	void EndQuote(Tick tick);

	//	从分笔数据生成指定周期(以秒为单位)的K线数据
	void Generate(TickCollection& ticks, int seconds);

	//	从分笔数据生成指定周期(以秒为单位)的K线数据
	void Generate(TickCollection& ticks, int seconds, KLine prevDayLine);

	//	添加关键价格
	void AddKeyPrice(int price, string desc) { m_mapKeyPrice.insert(pair<int,string>(price, desc)); }

	map<int, string>& GetKeyPrices() { return m_mapKeyPrice; }

	void Clear();
};
