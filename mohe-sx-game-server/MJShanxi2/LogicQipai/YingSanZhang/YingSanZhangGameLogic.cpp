#include "StdAfx.h"
#include "YingSanZhangGameLogic.h"

//////////////////////////////////////////////////////////////////////////

//�˿�����
BYTE CGameLogic::m_cbCardListData[52]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K
};

//����ģʽ
BYTE CGameLogic::m_cbCardListData_BigCard[20]=
{
	0x01, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	0x11, 0x1A, 0x1B, 0x1C, 0x1D,	//÷�� A - K
	0x21, 0x2A, 0x2B, 0x2C, 0x2D,	//���� A - K
	0x31, 0x3A, 0x3B, 0x3C, 0x3D	//���� A - K
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	
	if (cbCardCount==MAX_COUNT)
	{
		//��������
		bool cbSameColor=true,bLineCard=true;
		BYTE cbFirstColor=GetCardColor(cbCardData[0]);
		BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

		//���η���
		for (BYTE i=1;i<cbCardCount;i++)
		{
			//���ݷ���
			if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
			if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) bLineCard=false;

			//�����ж�
			if ((cbSameColor==false)&&(bLineCard==false)) break;
		}

		//����A32
		if(!bLineCard)
		{
			bool bOne=false,bTwo=false,bThree=false;
			for(BYTE i=0;i<MAX_COUNT;i++)
			{
				if(GetCardValue(cbCardData[i])==1)		bOne=true;
				else if(GetCardValue(cbCardData[i])==2)	bTwo=true;
				else if(GetCardValue(cbCardData[i])==3)	bThree=true;
			}
			if(bOne && bTwo && bThree)bLineCard=true;
		}

		//˳������
		if ((cbSameColor)&&(bLineCard)) return CT_SHUN_JIN;

		//˳������
		if ((!cbSameColor)&&(bLineCard)) return CT_SHUN_ZI;

		//������
		if((cbSameColor)&&(!bLineCard)) return CT_JIN_HUA;

		//���η���
		bool bDouble=false,bPanther=true;

		//���Ʒ���
		for (BYTE i=0;i<cbCardCount-1;i++)
		{
			for (BYTE j=i+1;j<cbCardCount;j++)
			{
				if (GetCardLogicValue(cbCardData[i])==GetCardLogicValue(cbCardData[j])) 
				{
					bDouble=true;
					break;
				}
			}
			if(bDouble)break;
		}

		//����(����)����
		for (BYTE i=1;i<cbCardCount;i++)
		{
			if (bPanther && cbFirstValue!=GetCardLogicValue(cbCardData[i])) bPanther=false;
		}

		//���Ӻͱ����ж�
		if (bDouble==true) return (bPanther)?CT_BAO_ZI:CT_DOUBLE;

		//����235
		bool bTwo=false,bThree=false,bFive=false;
		for (BYTE i=0;i<cbCardCount;i++)
		{
			if(GetCardValue(cbCardData[i])==2)	bTwo=true;
			else if(GetCardValue(cbCardData[i])==3)bThree=true;
			else if(GetCardValue(cbCardData[i])==5)bFive=true;			
		}	
		if (bTwo && bThree && bFive) return CT_SPECIAL;
	}

	return CT_SINGLE;
}

//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//ת����ֵ
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//����λ��
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//�����˿�-����ģʽѡ��        by wyz  1����ͨģʽ   2������ģʽ
void CGameLogic::RandCardList_PlayType(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE playType)
{
	if (playType==1)
		RandCardList( cbCardBuffer,  cbBufferCount);
	else if (playType==2)
		RandCardList_BigCard(cbCardBuffer, cbBufferCount);
	return;

}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);

	//����׼��
	BYTE cbCardData[sizeof(m_cbCardListData)];
	CopyMemory(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

	//�����˿�
	BYTE bRandCount=0, bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//�����˿�-����ģʽ
void CGameLogic::RandCardList_BigCard(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData_BigCard)];
	CopyMemory(cbCardData, m_cbCardListData_BigCard, sizeof(m_cbCardListData_BigCard));

	//�����˿�
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData_BigCard)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData_BigCard)-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	return (bCardValue==1)?(bCardValue+13):bCardValue;
}

//�Ա��˿�
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE tianDiDragon)
{
	//���ñ���
	BYTE FirstData[MAX_COUNT],NextData[MAX_COUNT];
	CopyMemory(FirstData,cbFirstData,sizeof(FirstData));
	CopyMemory(NextData,cbNextData,sizeof(NextData));

	//��С����
	SortCardList(FirstData,cbCardCount);
	SortCardList(NextData,cbCardCount);

	//��ȡ����
	BYTE cbNextType=GetCardType(NextData,cbCardCount);
	BYTE cbFirstType=GetCardType(FirstData,cbCardCount);

	//�����������
	if((cbNextType+cbFirstType)==(CT_SPECIAL+CT_BAO_ZI))return (BYTE)(cbFirstType>cbNextType);

	//��ԭ��������
	if(cbNextType==CT_SPECIAL)cbNextType=CT_SINGLE;
	if(cbFirstType==CT_SPECIAL)cbFirstType=CT_SINGLE;

	//�����ж�
	if (cbFirstType!=cbNextType) return (cbFirstType>cbNextType)?1:0;

	//������
	switch(cbFirstType)
	{
	case CT_BAO_ZI:			//����
	case CT_SINGLE:			//����
	case CT_JIN_HUA:		//��
		{
			//�Ա���ֵ
			for (BYTE i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(NextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(FirstData[i]);
				if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue)?1:0;
			}
			return DRAW;
		}
	case CT_SHUN_ZI:		//˳��
	case CT_SHUN_JIN:		//˳�� 432>A32
		{		
			BYTE cbNextValue=GetCardLogicValue(NextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(FirstData[0]);

			if (tianDiDragon==1)
			{
				BYTE sNextType = GetSpecTypeOnShun(NextData);
				BYTE sFirstType = GetSpecTypeOnShun(FirstData);
				if (sNextType>0 && sFirstType>0)
				{
					return sFirstType > sNextType ? 1 : 0;
				}
				else if (sNextType > 0) return 0;
				else if (sFirstType > 0)return 1;
				else
				{
					return (cbFirstValue>cbNextValue) ? 1 : 0;
				}

			}

			//����A32
			if(cbNextValue==14 && GetCardLogicValue(NextData[cbCardCount-1])==2)
			{
				cbNextValue=3;
			}
			if(cbFirstValue==14 && GetCardLogicValue(FirstData[cbCardCount-1])==2)
			{
				cbFirstValue=3;
			}

			//�Ա���ֵ
			if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue)?1:0;
			return DRAW;
		}
	case CT_DOUBLE:			//����
		{
			BYTE cbNextValue=GetCardLogicValue(NextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(FirstData[0]);

			//���Ҷ���/����
			BYTE bNextDouble=0,bNextSingle=0;
			BYTE bFirstDouble=0,bFirstSingle=0;
			if(cbNextValue==GetCardLogicValue(NextData[1]))
			{
				bNextDouble=cbNextValue;
				bNextSingle=GetCardLogicValue(NextData[cbCardCount-1]);
			}
			else
			{
				bNextDouble=GetCardLogicValue(NextData[cbCardCount-1]);
				bNextSingle=cbNextValue;
			}
			if(cbFirstValue==GetCardLogicValue(FirstData[1]))
			{
				bFirstDouble=cbFirstValue;
				bFirstSingle=GetCardLogicValue(FirstData[cbCardCount-1]);
			}
			else 
			{
				bFirstDouble=GetCardLogicValue(FirstData[cbCardCount-1]);
				bFirstSingle=cbFirstValue;
			}

			if (bNextDouble!=bFirstDouble)return (bFirstDouble>bNextDouble)?1:0;
			if (bNextSingle!=bFirstSingle)return (bFirstSingle>bNextSingle)?1:0;
			return DRAW;
		}
	}

	return DRAW;
}

//��ȡ����--����
BYTE CGameLogic::GetCardTypeSord(BYTE cbCardData[], BYTE cbCardCount)
{

	if (cbCardCount!=MAX_COUNT)return 0;
	BYTE  tmpCardData[3];
	ZeroMemory(tmpCardData, sizeof(tmpCardData));
	CopyMemory(tmpCardData, cbCardData, cbCardCount);

	SortCardList(tmpCardData, cbCardCount);

	BYTE cardType =  GetCardType(tmpCardData, cbCardCount);
	//��ԭ��������
	if (cardType==CT_SPECIAL)cardType=CT_SINGLE;
	return cardType;
}

//��ȡ���ͣ���������
BYTE CGameLogic::GetCardTypeDragon(BYTE tempcbCardData[], BYTE cbCardCount)
{

	if (cbCardCount != MAX_COUNT)return 0;
	BYTE  cbCardData[MAX_COUNT];
	ZeroMemory(cbCardData, sizeof(cbCardData));
	CopyMemory(cbCardData, tempcbCardData, cbCardCount);

	SortCardList(cbCardData, cbCardCount);

	if (cbCardCount == MAX_COUNT)
	{
		//��������
		bool cbSameColor = true, bLineCard = true;
		BYTE cbFirstColor = GetCardColor(cbCardData[0]);
		BYTE cbFirstValue = GetCardLogicValue(cbCardData[0]);

		//���η���
		for (BYTE i = 1; i<cbCardCount; i++)
		{
			//���ݷ���
			if (GetCardColor(cbCardData[i]) != cbFirstColor) cbSameColor = false;
			if (cbFirstValue != (GetCardLogicValue(cbCardData[i]) + i)) bLineCard = false;

			//�����ж�
			if ((cbSameColor == false) && (bLineCard == false)) break;
		}

		//����A32
		if (!bLineCard)
		{
			bool bOne = false, bTwo = false, bThree = false;
			for (BYTE i = 0; i<MAX_COUNT; i++)
			{
				if (GetCardValue(cbCardData[i]) == 1)		bOne = true;
				else if (GetCardValue(cbCardData[i]) == 2)	bTwo = true;
				else if (GetCardValue(cbCardData[i]) == 3)	bThree = true;
			}
			if (bOne && bTwo && bThree)bLineCard = true;
		}

		//˳������
		if ((cbSameColor) && (bLineCard))
		{
			BYTE type = GetSpecTypeOnShun(cbCardData);
			if (type == 2)return CT_JIN_TIAN_DRAGON;
			else if (type == 1)return CT_JIN_DI_DRAGON;
			return CT_SHUN_JIN;
		}

		//˳������
		if ((!cbSameColor) && (bLineCard))
		{
			BYTE type = GetSpecTypeOnShun(cbCardData);
			if (type == 2)return CT_SHUN_TIAN_DRAGON;
			else if (type == 1)return CT_SHUN_DI_DRAGON;
			return CT_SHUN_ZI;
		}
		//������
		if ((cbSameColor) && (!bLineCard)) return CT_JIN_HUA;

		//���η���
		bool bDouble = false, bPanther = true;

		//���Ʒ���
		for (BYTE i = 0; i<cbCardCount - 1; i++)
		{
			for (BYTE j = i + 1; j<cbCardCount; j++)
			{
				if (GetCardLogicValue(cbCardData[i]) == GetCardLogicValue(cbCardData[j]))
				{
					bDouble = true;
					break;
				}
			}
			if (bDouble)break;
		}

		//����(����)����
		for (BYTE i = 1; i<cbCardCount; i++)
		{
			if (bPanther && cbFirstValue != GetCardLogicValue(cbCardData[i])) bPanther = false;
		}

		//���Ӻͱ����ж�
		if (bDouble == true) return (bPanther) ? CT_BAO_ZI : CT_DOUBLE;

		//����235
		bool bTwo = false, bThree = false, bFive = false;
		for (BYTE i = 0; i<cbCardCount; i++)
		{
			if (GetCardValue(cbCardData[i]) == 2)	bTwo = true;
			else if (GetCardValue(cbCardData[i]) == 3)bThree = true;
			else if (GetCardValue(cbCardData[i]) == 5)bFive = true;
		}
		if (bTwo && bThree && bFive)
			return CT_SINGLE;
			//return CT_SPECIAL;
	}
	//��ԭ��������
	//if (cardType == CT_SPECIAL)cardType = CT_SINGLE;
	//return cardType;

	return CT_SINGLE;
}

//�ж�˳�ӣ������������� -�������Ĭ��������������Ѿ���˳�ӣ��ҽ������� AKQJ109....
BYTE CGameLogic::GetSpecTypeOnShun(BYTE cbCardData[])
{
	if (GetCardLogicValue(cbCardData[0]) == 14 && GetCardLogicValue(cbCardData[1]) == 13 && GetCardLogicValue(cbCardData[2]) == 12)return 2;
	if (GetCardLogicValue(cbCardData[0]) == 14 && GetCardLogicValue(cbCardData[1]) == 3 && GetCardLogicValue(cbCardData[2]) == 2)return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////