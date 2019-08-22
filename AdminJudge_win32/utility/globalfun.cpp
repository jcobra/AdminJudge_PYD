//globalfun.cpp 
#include "stdafx.h"
#include "globalfun.h"
#include <vector>
#include <math.h>



#define LLUnit 86400.0



#define	PI		3.1415926536
#define PARC	0.01745329252
#define	AE		6378137.0
#define	BE		6356752.3142
#define E0		0.081819191
#define E2		0.0409095955

#define	L0	104
#define	B0	18
#define	B1	25
#define	B2	47
#define F0  1.9983002813790927
#define r0  10563744.853716312
#define N0  0.59152989882401852



#define    EARTH_A   	6378245.000000
#define    EARTH_E12    0.006693421623

double r(double b)
{
	double sinb = sin(b);
	return(EARTH_A*cos(b)/sqrt(1.0-EARTH_E12*sinb*sinb));
}

double m(double bb)
{
	double mm;
	mm=sin(bb);
	mm=sqrt(1.0-EARTH_E12*mm*mm);
	return( 6335552.7170002/(mm*mm*mm) );
}

double arctg(double dy, double dx)
{	
	if( dx==0.0 && dy==0.0 ) return(0.0);
	return ( atan2( dy, dx ) );
}

double atanhhh(double x)
{
	return log((1.0+x)/(1.0-x))*0.5;
}

double q(double bb)
{
	double sinbb = sin(bb);
	return atanhhh(sinbb)-0.081813334*atanhhh(0.081813334*sinbb);
}

double invq(double q0 )
{
	double q1,q10,b0,dq;
	q10=q0;
	b0=asin(tanh(q0));
	q1=q(b0);
	do 
	{  dq=q0-q1;
	q10=q10+dq;
	b0=asin(tanh(q10));
	q1=q(b0);
	}  while( (fabs(dq)) > 0.0000000001);
	return(b0);
}

void rhumb_line(double bb1, double ll1, double bb2, double ll2, double *angle, double *length)
{
	double q1,q2,angl,len,bb0,dbb,dll;
	
	bb0=(bb1+bb2)/2.0; 
	dbb=(bb2-bb1); 
	dll=(ll2-ll1);
	q1=q(bb1);
	q2=q(bb2);
	angl=arctg( dll, (q2-q1) );
	
	if( fabs(fabs(angl)-PI/2.0) > PI/180.0 )  
	{   len=m(bb0)*dbb/cos(angl); }
	else
	{   len=r(bb0)*dll/sin(angl); } 
	*angle=angl; *length=len;
}

void inverse_rhumb( double b1,double l1,double angl,double len, double* b2,double *l2 )
{
	double sig0, sigi, dsig, sigii, err, qi, bi,li, dqi,q1, dli;
	short i=0;
	if( len < 1852 )
		err=0.001;
	else
		err=1.0;
	q1=q(b1);
	double tmp = invq(q1);
	sig0=len;
	sigi=len;
	sigii=len;
	do {
		sigii=sigi;
		
		dli=sigi/3437.746771/1852*sin(angl);
		li=l1+dli;
		
		dqi=sigi/3437.746771/1852*cos(angl);
		qi=q1+dqi;
		bi=invq(qi);
		
		rhumb_line(b1,l1,bi,li,&angl,&sigi);
		dsig=sig0-sigi;
		
		sigi=sigii+dsig;
		i=i+1;
	} while( dsig>err ) ;
	
	*b2=bi; *l2=li;
	
	return;
}

void LLDistance(double dLong1, double dLat1, double dLong2, double dLat2, double *angle, double *length)
{
	rhumb_line(dLat1 * 0.01745329252, dLong1 * 0.01745329252, dLat2 * 0.01745329252, dLong2 * 0.01745329252, angle, length );
	*angle *= 57.295779513;
	for(; *angle<0; *angle += 360);
	for(; *angle > 360; *angle -= 360);
	if((int(*angle*10)%10)>5)
		*angle += 1;
}

