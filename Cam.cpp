#include "StdAfx.h"
#include "Cam.h"
#include "GCode.h"
#include "math.h"
#pragma comment(lib, "v4501v.lib")

CCam::CCam(void)
: m_dDelta0(0)
, m_dDelta01(0)
, m_dDelta1(0)
, m_dDelta02(0)
, m_dh(0)
, m_de(0)
, m_dr0(0)
, m_drr0(0)

, m_nIndexMotion(0)
, m_nIndexFollower(0)
{
	//�����˶�����
	//�Ƴ�
//	tt[0]=0;tt[1]=0;tt[2]=0;tt[3]=0.5;
//	tt[4]=0.5;tt[5]=1;tt[6]=1;tt[7]=1;
	//�س�
//	th[0]=0;th[1]=0;th[2]=0.5;th[3]=0.5;
//	th[4]=0.5;th[5]=0.5;th[6]=1;th[7]=1;
	
//	Motion[0]="����";
//	Motion[1]="�ȼ���";
//	Motion[2]="����";


	m_bDataRise=false;
	m_bDataReturn=false;
	m_bData2Rise=false;
	m_bData2Return=false;
	m_bGCode=false;

	
}

CCam::~CCam(void)
{
	if (m_bDataRise)
	{
		delete t;
	if(m_bDataReturn)
		delete h;
	}

	if (m_bData2Rise)
	{
		delete t2;
	}
	if(m_bData2Return)
	{
		delete h2;
	}
	if(m_bGCode)
		delete gcode;
	
}

// ����Ƴ��˶���
double CCam::GetDelta0(void)
{
	return m_dDelta0;
}

// ���Զ��ֹ��
double CCam::GetDelta01(void)
{
	return m_dDelta01;
}

// ��ûس��˶���
double CCam::GetDelta1(void)
{
	return m_dDelta1;
}

// ��ý���ֹ��
double CCam::GetDelta02(void)
{
	return m_dDelta02;
}

// ���ƫ�ľ�
double CCam::Gete(void)
{
	return m_de;
}

// ��û�Բ�뾶
double CCam::Getr0(void)
{
	return m_dr0;
}

// ��ù����г�
double CCam::Geth(void)
{
	return m_dh;
}

// ��ù��Ӱ뾶
double CCam::Getrr0(void)
{
	return m_drr0;
}

// �������
void CCam::SetParameter(double Delta0, double Delta1, 
						double Delta01, double Delta02, 
						double h, double r0, 
						double e, double rr0,
						int nIndexMotion, int nIndexMotionReturn,
						CArray<double>* t,CArray<double>* hh,
						int nIndexFollower)
{
	//�����������
	m_dDelta0=Delta0;
	m_dDelta1=Delta1;
	m_dDelta01=Delta01;
	m_dDelta02=Delta02;
	m_dh=h;
	m_de=e;
	m_dr0=r0;
	m_drr0=rr0;
	//�����˶����ɵ�tֵ
	tt[0]=0;
	th[0]=0;
	for (int i=0;i<7;i++)
		tt[i+1]=t->GetAt(i);
	for (int i=0;i<7;i++)
		th[i+1]=hh->GetAt(i);
	//��ص�����ֵ
	m_nIndexMotion=nIndexMotion;
	m_nIndexMotionReturn=nIndexMotionReturn;
	m_nIndexFollower=nIndexFollower;
	

}

// �������������
void CCam::CalcPoint(void)
{
	//��ʼ��matcom4.5
	initM(MATCOM_VERSION);
	Mm delta;
	Mm ss=zeros(NUM,1);
	for (int i=0;i<NUM;i++)
		ss.r(i+1)=m_ds[i];
	//��������
	delta=colon(1*pi/180,2*pi/NUM,2*pi);
	//ת��
	delta=ctranspose(delta);
	Mm x;
	Mm y;
	//��������
	x=times(sqrt(m_dr0*m_dr0-m_de*m_de)+ss,sin(delta))+times(m_de,cos(delta));
	y=times(sqrt(m_dr0*m_dr0-m_de*m_de)+ss,cos(delta))-times(m_de,sin(delta));
	
	for (int i=0;i<NUM;i++)
	{
		px[i]=x.r(i+1);
		py[i]=y.r(i+1);
	}
	exitM();
}

// �����˶����ɲ���
void CCam::CalcParameter(void)
{
		
	//Զ��ֹλ��
	for(int i=(int)m_dDelta0/TIMES;i<(int)(m_dDelta0+m_dDelta01)/TIMES;i++)
		m_ds[i]=m_dh;
	//����ֹλ��
	for(int i=(int)(m_dDelta0+m_dDelta01+m_dDelta1)/TIMES;i<NUM;i++)
		m_ds[i]=0;
	//�Ƴ�λ��
	if(m_nIndexMotion==2)
	{
		for(int i=0;i<(int)m_dDelta0/TIMES;i++)
			m_ds[i]=m_dh*i*TIMES/m_dDelta0;
	}
	else
	{
		t=new CData(tt,m_dDelta0/TIMES,m_dh);
		m_bDataRise=true;
		for(int i=0;i<(int)m_dDelta0/TIMES;i++)
			m_ds[i]=t->s[i];

	}

	//�س�λ��
	if (m_nIndexMotionReturn==2)
	{
		for(int i=(int)(m_dDelta0+m_dDelta01)/TIMES;i<(int)(m_dDelta0+m_dDelta01+m_dDelta1)/TIMES;i++)
			m_ds[i]=m_dh*(1-(i-(m_dDelta0+m_dDelta01)/TIMES)/(m_dDelta1/TIMES));
	}
	else
	{
	
		h=new CData(th,m_dDelta1/TIMES,m_dh);
		m_bDataReturn=true;
		for(int i=(int)(m_dDelta0+m_dDelta01)/TIMES;i<(int)(m_dDelta0+m_dDelta01+m_dDelta1)/TIMES;i++)
			m_ds[i]=m_dh-h->s[i-(int)(m_dDelta0+m_dDelta01)/TIMES];
	}
}

// ������Ƶ�����
void CCam::CalcCtrlPoint(void)
{
	initM(MATCOM_VERSION);
	Mm node;
	//�����㣬���һ�㼴Ϊ��һ����
	node=zeros(NUM+1,2);
	Mm m_node;
	for (int i=0;i<NUM;i++)
	{
		node.r(i+1,1)=px[i];
		node.r(i+1,2)=py[i];
	}
	node.r(NUM+1,1)=node.r(1,1);
	node.r(NUM+1,2)=node.r(1,2);

	//���Ƶ�
	m_node=zeros(NUM+1,2);
	Mm B;
	B=zeros(NUM+1,NUM+3);
	for (int i=1;i<=NUM+1;i++)
	{
		B.r(i,i)=mrdivide(1.0,6.0);
		B.r(i,i+1)=mrdivide(4.0,6.0);
		B.r(i,i+2)=mrdivide(1.0,6.0);

	}

	Mm C;
	C=zeros(NUM+3,NUM+1);
	for (int i2=1;i2<=NUM+1;i2++)
		C.r(i2,i2)=1;

	C.r(NUM+2,1)=-1;
	C.r(NUM+2,2)=1;
	C.r(NUM+2,NUM+1)=1;
	C.r(NUM+3,1)=-1;
	C.r(NUM+3,3)=1;
	C.r(NUM+3,NUM+1)=1;

	Mm D;
	D=zeros(NUM+1);
	D=B*C;
	m_node=mldivide(D,node);
	
	Mm a=zeros(2,2);

	a.r(1,1)=m_node.r(NUM+1,1)-m_node.r(1,1)+m_node.r(2,1);
	a.r(1,2)=m_node.r(NUM+1,2)-m_node.r(1,2)+m_node.r(2,2);

	a.r(2,1)=m_node.r(NUM+1,1)+m_node.r(3,1)-m_node.r(1,1);
	a.r(2,2)=m_node.r(NUM+1,2)+m_node.r(3,2)-m_node.r(1,2);
	m_node=vertcat(m_node,a);

		
	for (int i3=0;i3<NUM+3;i3++)
	{
		mpx[i3]=m_node.r(i3+1,1);
		mpy[i3]=m_node.r(i3+1,2);
	}
	exitM();
}

// ʹ������B�������߻���͹��
void CCam::DrawCam3BSpline(CDC* pDC, double * mpx,
						   double * mpy, int nCount,CRect r1)
{
	double x = ( mpx[0] + 4.0 * mpx[1] + mpx[2] ) / 6.0+(r1.right-r1.left)/2;
	double y = -( mpy[0] + 4.0 * mpy[1] + mpy[2] ) / 6.0+(r1.bottom-r1.top)/2;
	
	pDC->MoveTo((long)x,(long)y);
	HPEN hPen;
	HPEN hPenOld;
	hPen=CreatePen( PS_SOLID, 0, RGB(0,100,0));
	hPenOld = ( HPEN )SelectObject(pDC->m_hDC,hPen);

		for ( int i = 0; i < nCount; i++ )
	{
		// �ҵ�������С�����꣬������ķ�Χ���Ӷ�ȷ��u����С����
		long xMax, xMin, yMax, yMin;
		xMax = xMin = (long)mpx[i];
		yMax = yMin = (long)mpy[i];
		if ( xMax < mpx[i+1] ) xMax = (long)mpx[i+1];
		if ( xMin > mpx[i+1] ) xMin = (long)mpx[i+1];
		if ( xMax < mpx[i+2] ) xMax = (long)mpx[i+2];
		if ( xMin > mpx[i+2] ) xMin =(long) mpx[i+2];
		if ( xMax < mpx[i+3] ) xMax =(long) mpx[i+3];
		if ( xMin > mpx[i+3] ) xMin = (long)mpx[i+3];

		if ( yMax < mpy[i+1] ) yMax =(long) mpy[i+1];
		if ( yMin > mpy[i+1] ) yMin = (long)mpy[i+1];
		if ( yMax < mpy[i+2] ) yMax = (long)mpy[i+2];
		if ( yMin > mpy[i+2] ) yMin = (long)mpy[i+2];
		if ( yMax < mpy[i+3] ) yMax =(long) mpy[i+3];
		if ( yMin > mpy[i+3] ) yMin = (long)mpy[i+3];
		double fMax = (double)(xMax - xMin);
		if ( fMax < (double)(yMax - yMin) ) fMax = (double)(yMax - yMin);
		double	u, uStep = 3.0 / fMax ;		// uֵ����
		
		
		
		for ( u = uStep; u<=1.0; u+= uStep )
		{
			
			x = (1.0-u)*(1.0-u)*(1.0-u)
					* (double)(mpx[i])
				+ ( 3.0*u*u*u - 6.0*u*u + 4.0 )
					* (double)(mpx[i+1])
				+ ( -3.0*u*u*u + 3.0*u*u + 3.0*u + 1.0 )
					* (double)(mpx[i+2])
				+ u*u*u
					* (double)(mpx[i+3]);
			y = (1.0-u)*(1.0-u)*(1.0-u)
					* (double)(mpy[i])
				+ ( 3.0*u*u*u - 6.0*u*u + 4.0 )
					* (double)(mpy[i+1])
				+ ( -3.0*u*u*u + 3.0*u*u + 3.0*u + 1.0 )
					* (double)(mpy[i+2])
				+ u*u*u
					* (double)(mpy[i+3]);
			x = x / 6.0;	y = y / 6.0;
			if (pDC)
			{
				
				pDC->LineTo((long)(x+r1.CenterPoint().x), (long)(-y+r1.CenterPoint().y));
			}
			
		}
	}
//		for(int i=0;i<100;i++)
//		pDC->LineTo((long)(px[0]+(r1.right-r1.left)/2), (long)(-py[0]-i+(r1.bottom-r1.top)/2));
	SelectObject(pDC->m_hDC,hPenOld);
	DeleteObject( hPen );



}

// ʹ��ֱ�߻���͹������
void CCam::DrawCamLine(CDC* pDC, double* px, double* py, int nCount, CRect r1, int mode)
{
	pDC->MoveTo((int)px[0]+r1.CenterPoint().x,-(int)py[0]+r1.CenterPoint().y);
	for (int i=1;i<nCount;i++)
		pDC->LineTo((int)px[i]+r1.CenterPoint().x,-(int)py[i]+r1.CenterPoint().y);
	if (mode==1)
		pDC->LineTo((int)px[0]+r1.CenterPoint().x,-(int)py[0]+r1.CenterPoint().y);

}

void CCam::CalcParameter2(void)
{

	for(int i=(int)m_dDelta0;i<(int)(m_dDelta0+m_dDelta01);i++)
		m_ds2[i]=m_dh;
	for(int i=(int)(m_dDelta0+m_dDelta01+m_dDelta1);i<COUNT;i++)
		m_ds2[i]=0;

	//�����Ƴ̻س̲���
	if(m_nIndexMotion==2)
	{
		//�����г�
		for(int i=0;i<(int)m_dDelta0;i++)
			m_ds2[i]=m_dh*i/m_dDelta0;	
	}
	else
	{
		t2=new CData(tt,m_dDelta0,m_dh);
		m_bData2Rise=true;
		for(int i=0;i<(int)m_dDelta0;i++)
			m_ds2[i]=t2->s[i];
	}


	if (m_nIndexMotionReturn==2)
	{
		for(int i=(int)(m_dDelta0+m_dDelta01);i<(int)(m_dDelta0+m_dDelta01+m_dDelta1);i++)
			m_ds2[i]=m_dh*(1-(i-(m_dDelta0+m_dDelta01))/(m_dDelta1));
	}
	else
	{
		m_bData2Return=true;	
		h2=new CData(th,m_dDelta1,m_dh);
		for(int i=(int)(m_dDelta0+m_dDelta01);i<(int)(m_dDelta0+m_dDelta01+m_dDelta1);i++)
			m_ds2[i]=m_dh-h2->s[i-(int)(m_dDelta0+m_dDelta01)];

	}
}

void CCam::CalcPoint2(void)
{initM(MATCOM_VERSION);
	Mm delta;
	Mm ss=zeros(COUNT,1);
	for (int i=0;i<COUNT;i++)
		ss.r(i+1)=m_ds2[i];
	//��������
	delta=colon(1*Pi/180,2*Pi/COUNT,2*Pi);
	//ת��
	delta=ctranspose(delta);
	Mm x;
	Mm y;
	//��������
	x=times(sqrt(m_dr0*m_dr0-m_de*m_de)+ss,sin(delta))+times(m_de,cos(delta));
	y=times(sqrt(m_dr0*m_dr0-m_de*m_de)+ss,cos(delta))-times(m_de,sin(delta));
	
	for (int i=0;i<COUNT;i++)
	{
		px2[i]=x.r(i+1);
		py2[i]=y.r(i+1);
	}
	exitM();

}

void CCam::CalcCtrlPoint2(void)
{
initM(MATCOM_VERSION);
	Mm node2;
	//�����㣬���һ�㼴Ϊ��һ����
	node2=zeros(COUNT+1,2);
	Mm m_node2;
	for (int i=0;i<COUNT;i++)
	{
		node2.r(i+1,1)=px2[i];
		node2.r(i+1,2)=py2[i];
	}
	node2.r(COUNT+1,1)=node2.r(1,1);
	node2.r(COUNT+1,2)=node2.r(1,2);

	//���Ƶ�
	m_node2=zeros(COUNT+1,2);
	Mm B2;
	B2=zeros(COUNT+1,COUNT+3);
	for (int i=1;i<=COUNT+1;i++)
	{
		B2.r(i,i)=mrdivide(1.0,6.0);
		B2.r(i,i+1)=mrdivide(4.0,6.0);
		B2.r(i,i+2)=mrdivide(1.0,6.0);

	}

	Mm C2;
	C2=zeros(COUNT+3,COUNT+1);
	for (int i2=1;i2<=COUNT+1;i2++)
		C2.r(i2,i2)=1;

	C2.r(COUNT+2,1)=-1;
	C2.r(COUNT+2,2)=1;
	C2.r(COUNT+2,COUNT+1)=1;
	C2.r(COUNT+3,1)=-1;
	C2.r(COUNT+3,3)=1;
	C2.r(COUNT+3,COUNT+1)=1;

	Mm D2;
	D2=zeros(COUNT+1);
	D2=B2*C2;
	m_node2=mldivide(D2,node2);
	
	Mm a2=zeros(2,2);

	a2.r(1,1)=m_node2.r(COUNT+1,1)-m_node2.r(1,1)+m_node2.r(2,1);
	a2.r(1,2)=m_node2.r(COUNT+1,2)-m_node2.r(1,2)+m_node2.r(2,2);

	a2.r(2,1)=m_node2.r(COUNT+1,1)+m_node2.r(3,1)-m_node2.r(1,1);
	a2.r(2,2)=m_node2.r(COUNT+1,2)+m_node2.r(3,2)-m_node2.r(1,2);
	m_node2=vertcat(m_node2,a2);

		
	for (int i3=0;i3<COUNT+3;i3++)
	{
		mpx2[i3]=m_node2.r(i3+1,1);
		mpy2[i3]=m_node2.r(i3+1,2);
	}
	exitM();
}



// �������ؼӹ�����
void CCam::SetParameter2(CString KnifeID, CString KnifeFace, double Feed, int Speed)
{
	m_sKnifeID=KnifeID;
	m_sKnifeFace=KnifeFace;
	m_dFeed=Feed;
	m_nSpeed=Speed;
}

// ����ѹ����
void CCam::CalcPreAngle(void)
{
	//�����Ƴ̻س��ٶ�
	double *speed=new double[COUNT];
	if(m_nIndexMotion==0)
	{
		//�����ٶ�
		for(int i=0;i<(int)m_dDelta0;i++)
			speed[i]=m_dh/m_dDelta0;
		for(int i=(int)m_dDelta0;i<(int)(m_dDelta0+m_dDelta01);i++)
			speed[i]=0;
		for(int i=(int)(m_dDelta0+m_dDelta01);i<(int)(m_dDelta0+m_dDelta01+m_dDelta1);i++)
			speed[i]=-m_dh/m_dDelta1;
		for(int i=(int)(m_dDelta0+m_dDelta01+m_dDelta1);i<COUNT;i++)
			speed[i]=0;
	}
	else
	{
		t2=new CData(tt,m_dDelta0,m_dh);
		h2=new CData(th,m_dDelta1,m_dh);
		//�����г�
		for(int i=0;i<(int)m_dDelta0;i++)
			speed[i]=t2->v[i]*m_dh/m_dDelta0;
		for(int i=(int)m_dDelta0;i<(int)(m_dDelta0+m_dDelta01);i++)
			speed[i]=0;
		for(int i=(int)(m_dDelta0+m_dDelta01);i<(int)(m_dDelta0+m_dDelta01+m_dDelta1);i++)
			speed[i]=-h2->v[i-(int)(m_dDelta0+m_dDelta01)]*m_dh/m_dDelta1;
		for(int i=(int)(m_dDelta0+m_dDelta01+m_dDelta1);i<COUNT;i++)
			speed[i]=0;

	}
	for (int i=0;i<COUNT;i++)
		m_dPreAngle[i]=abs(atan((speed[i]-m_de)/(sqrt(m_dr0*m_dr0-m_de*m_de)+m_ds2[i]))*180/Pi);
	delete[] speed;
}

void CCam::ShowPresAngle(CDC* pDC,CRect r1)
{

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int wid=r1.Width();
	CString str;
	int height=tm.tmHeight+tm.tmInternalLeading;
	pDC->TextOutW(0,0,_T("ѹ������ʾ(�Ƕ���)"));
	str.Format(_T("�Ƴ���㣺%.2f"),m_dPreAngle[0]);
	pDC->TextOutW(0,height,str);
	str.Format(_T("Զ��ֹ��㣺%.2f"),m_dPreAngle[(int)m_dDelta0]);
	pDC->TextOutW(0,2*height,str);
	str.Format(_T("�س���㣺%.2f"),m_dPreAngle[(int)(m_dDelta0+m_dDelta01)]);
	pDC->TextOutW(0,3*height,str);
}

// ��������ѹ����
void CCam::SetParameter3(double AllowAlpha)
{
	m_dAllowAlpha=AllowAlpha;
}

void CCam::CreateGCode(void)
{
	gcode=new CGCode(m_dDelta0,m_dDelta01,m_dDelta1,m_dDelta02,
			    mpx2,mpy2,m_dr0,m_dh,m_sKnifeID,m_sKnifeFace,
			    m_dFeed,m_nSpeed);
	m_bGCode=true;
}