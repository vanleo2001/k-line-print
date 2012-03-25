
// KLinePrintDoc.h : CKLinePrintDoc 类的接口
//
#include <string>
#include "KLineCollection.h"

using namespace std;

#pragma once

class CKLinePrintDoc : public CDocument
{
protected: // 仅从序列化创建
	CKLinePrintDoc();
	DECLARE_DYNCREATE(CKLinePrintDoc)

// 属性
public:

	KLineCollection klc15s;
	KLineCollection klc1min;
	KLineCollection klcday;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CKLinePrintDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	string m_CurCsvFile;
	string m_CurDayFile;

	string GetNeighborCsvFile(string path, bool bPrev, bool bZhuLi = FALSE);
	void ViewNeighborDate(BOOL bPrev);
	void ReloadByDate(int nDate);

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
};


