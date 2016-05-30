//#include "StdAfx.h"
#include <iostream>
#include <fstream>
using namespace std;



#include "EssBitmap.h"
#include <malloc.h>
#include <math.h>
#include <string.h>

double exps[3][3][256]={0.0};
int inited=0;

EssBitmap * createEssBitmap(int bpp,int width,int height,unsigned char * bits){
	if(bpp==8 || bpp==24 || bpp==32){
		if(width>0 && height>0){
			int s=width*bpp/8;
			if(s%4>0)
				s+=4-s%4;
			EssBitmap * ret=NULL;
			ret=(EssBitmap *)malloc(sizeof(EssBitmap));
			if(ret==NULL)
				return NULL;
			ret->bits=NULL;
			ret->bits=(unsigned char *)malloc(s*height);
			if(ret->bits==NULL){
				free(ret);
				return NULL;
			}
			ret->bpp=bpp;
			ret->stride=s;
			ret->width=width;
			ret->height=height;
			if(bits==NULL)
				memset(ret->bits,0,s*height);
			else
				memcpy(ret->bits,bits,s*height);
			return ret;
		}
		else
			return NULL;
	}
	else
		return NULL;
}
void deleteEssBitmap(EssBitmap ** bitmap){
	if(bitmap==NULL)
		return ;
	if(*bitmap==NULL)
		return ;
	if((*bitmap)->bits!=NULL)
		free((*bitmap)->bits);
	free(*bitmap);
	*bitmap=NULL;
}

EssBitmap * isEssBitmap(EssBitmap * bitmap){
	if(bitmap==NULL)
		return NULL;
	if(bitmap->bits==NULL)
		return NULL;
	if(bitmap->bpp==8 || bitmap->bpp==24 || bitmap->bpp==32){
		if(bitmap->width>0 && bitmap->height>0){
			int s=bitmap->width * bitmap->bpp /8;
			if(s%4>0)s+=4-s%4;
			if(s!=bitmap->stride)
				return NULL;
			return bitmap;
		}
		else
			return NULL;
	}
	else
		return NULL;
}
int getEssBitmapBpp(EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	return bitmap->bpp;
}
int getEssBitmapStride(EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	return bitmap->stride;
}
int getEssBitmapWidth(EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	return bitmap->width;
}
int getEssBitmapHeight(EssBitmap *bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	return bitmap->height;
}
unsigned char * getEssBitmapBitsPtr(EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	return bitmap->bits;
}
int copyEssBitmapBits(unsigned char * outputBufferPtr,int outputBufferSize,EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int sizeBits=bitmap->stride*bitmap->height;
	int size=sizeBits<outputBufferSize?sizeBits:outputBufferSize;
	memcpy(outputBufferPtr,bitmap->bits,size);
	return size;
}
//
EssBitmap * copyEssBitmapNearPoint(EssBitmap * src,int x,int y,int width,int height){
	if(isEssBitmap(src)==NULL)
		return NULL;
	if(src->bpp!=8)
		return NULL;
	EssBitmap * dst=createEssBitmap(8,width,height,NULL);
	if(dst==NULL)
		return NULL;
	unsigned char * srcBit,* db,* srcPointer,* dstPointer;
	srcBit=src->bits;
	db=dst->bits;
	int srcStride=src->stride;
	int ds=dst->stride;
	int sx,sy,dx,dy;
	int left=x-width/2;
	int bottom=y-height/2;
	for(dy=0;dy<height;dy++){
		sy=dy+bottom;
		dstPointer=db+dy*ds;
		if(sy>=0 && sy<src->height){
			for(dx=0;dx<width;dx++){
				sx=dx+left;
				if(sx>=0 && sx<src->width){
					srcPointer=srcBit+sy*srcStride+sx;
					*dstPointer=*srcPointer;
					dstPointer++;
				}
			}
		}
	}
	return dst;
}
EssBitmap * getStretchGrayEssBitmap(EssBitmap * src,int dstBitmapWidth,int dstBitmapHeight){
	if(isEssBitmap(src)==NULL)
		return NULL;
	if(src->bpp!=8)
		return NULL;
	EssBitmap * dst=createEssBitmap(8,dstBitmapWidth,dstBitmapHeight,NULL);
	if(dst==NULL)
		return NULL;
	if(src->stride==dst->stride && src->height==dst->height){
		memcpy(dst->bits,src->bits,src->stride*dst->height);
		return dst;
	}
	int sx,sy,dx,dy,tx,ty,tv,tc,srcStride,ds,srcWidth,srcHeight,dstWidth,dstHeight;
	unsigned char *srcPointer,*dstPointer,*srcBit,*db;
	srcWidth=src->width;
	srcHeight=src->height;
	srcStride=src->stride;
	srcBit=src->bits;
	dstWidth=dstBitmapWidth;
	dstHeight=dstBitmapHeight;
	ds=dst->stride;
	db=dst->bits;
	double wr,hr;
	wr=(double) srcWidth /dstWidth;
	hr=(double)srcHeight/dstHeight;
	for(dy=0;dy<dstHeight;dy++){
		dstPointer=db + ds * dy;
		sy=(int)(hr*dy);
		for(dx=0;dx<dstWidth;dx++){
			sx=(int)(wr*dx);
			tv=tc=0;
			for(ty=sy-1;ty<=sy+1;ty++){
				if(sx>0)srcPointer=srcBit + srcStride * ty + sx-1;
				else srcPointer=srcBit + srcStride * ty + sx;
				if(ty>=0 && ty< srcHeight){
					for(tx=sx-1;tx<=sx+1;tx++){
						if(tx>=0 && tx< srcWidth ){
							tv+=*srcPointer;
							tc++;
							srcPointer++;
						}
					}
				}
			}
			tv/=tc;
			*dstPointer=(unsigned char)tv;
			dstPointer++;
		}
	}
	return dst;
}
int stretchGrayEssBitmap(EssBitmap * src,EssBitmap * dst){
	if(isEssBitmap(src)==NULL || isEssBitmap(dst)==NULL)
		return 0;
	if(src->bpp !=8)
		return 0;
	int sx,sy,dx,dy,srcWidth,srcHeight,dstWidth,dstHeight,srcStride,ds,dbpp;
	unsigned char *srcBit,*srcPointer,*db,*dstPointer;
	double wr,hr;
	srcWidth=src->width;
	srcHeight=src->height;
	srcStride=src->stride;
	srcBit=src->bits;
	dstWidth=dst->width;
	dstHeight=dst->height;
	ds=dst->stride;
	db=dst->bits;
	dbpp=dst->bpp;
	wr=(double) srcWidth /dstWidth;
	hr=(double)srcHeight/dstHeight;
	for(dy=0;dy<dstHeight;dy++){
		dstPointer=db + ds * dy;
		sy=(int)(hr * dy);
		if(sy< srcHeight){
			for(dx=0;dx<dstWidth;dx++){
				sx=(int)(wr*dx);
				srcPointer=srcBit + srcStride * sy + sx;
				if(sx< srcWidth ){
					*(dstPointer++)=*srcPointer;
					if(dbpp>8){
						*(dstPointer++)=*srcPointer;
						*(dstPointer++)=*srcPointer;
					}
					if(dbpp==32)
						*(dstPointer++)=255;
				}
			}
		}
	}
	return 1;
}
//
EssBitmap * getGrayEssBitmapFromeBuffer(int bpp,int width,int height,unsigned char * buf,double rate){
	if(width<1 || height<1 || buf==NULL || rate<=0.0)return NULL;
	int dstWidth=(int)(rate*width);
	int dstHeight=(int)(rate*height);
	EssBitmap * ret=NULL;
	if(bpp==8 || bpp==24 || bpp==32)
		ret=createEssBitmap(8,dstWidth,dstHeight,NULL);
	if(ret==NULL)
		return NULL;
	int x,y,s,rx,ry,rs,rb,tx,ty,ci,cv,pv,cc,pi;
	rb=bpp/8;
	rs=width*rb;
	if(rs%4>0)rs+=4-rs%4;
	s=ret->stride;
	unsigned char *dstPointer, *db, *srcPointer, * srcBit ;
	db=ret->bits;
	srcBit=buf;
	cc=rb;
	if(cc>3)cc=3;
	double wr=(double)width/dstWidth;
	double hr=(double)height/dstHeight;
	for(y=0;y<dstHeight;y++){
		dstPointer=db+s*y;
		ry=(int)(hr*y);
		for(x=0;x<dstWidth;x++){
			rx=(int)(wr*x);
			pv=0;
			pi=0;
			for(ty=ry-1;ty<ry+1;ty++){
				if(ty>=0 && ty < height){
					if(rx>0)srcPointer=srcBit+rs*ty+rb*(rx-1);
					else srcPointer=srcBit+rs*ty+rb*rx;
					for(tx=rx-1;tx<rx+1;tx++){
						if(tx>=0 && tx<width){
							cv=0;
							for(ci=0;ci<cc;ci++){
								cv+=*srcPointer;
								srcPointer++;
							}
							if(rb==4)
								srcPointer++;
							cv/=cc;
							pv+=cv;
							pi++;
						}
					}
				}
			}
			pv/=pi;
			*dstPointer=(unsigned char)pv;
			dstPointer++;
		}
	}
	return ret;
}

EssBitmap * getGrayEssBitmap(EssBitmap * src){
	if(isEssBitmap(src)==NULL)return NULL;
	EssBitmap * ret=NULL;
	if(src->bpp==8){
		ret=createEssBitmap(8,src->width,src->height,src->bits);
		return ret;
	}
	if(src->bpp==24 || src->bpp==32){
		ret=createEssBitmap(8,src->width,src->height,NULL);
		if(ret==NULL)
			return NULL;
		int rs[3];
		rs[0]=114;
		rs[1]=587;
		rs[2]=299;
		int x,y,i,px;
		unsigned char * srcPointer;
		unsigned char * dstPointer;
		for(y=0;y<src->height;y++){
			srcPointer=src->bits + src->stride * y;
			dstPointer=ret->bits + ret->stride * y;
			for(x=0;x<src->width;x++){
				px=0;
				for(i=0;i<3;i++){
					px+=rs[i]* *srcPointer;
					srcPointer++;
				}
				px/=1000;
				*dstPointer=(unsigned char)px;
				dstPointer++;
				if(src->bpp==32)
					srcPointer++;
			}
		}
		return ret;
	}
	return NULL;
}
EssBitmap * getMeanFilterEssBitmap(EssBitmap * src){
	if(isEssBitmap(src)==NULL)
		return NULL;
	EssBitmap * ret=createEssBitmap(8,src->width,src->height,NULL);
	if(ret==NULL)
		return NULL;
	int x,y,ty,tx,tw,ti,px;
	unsigned char * srcPointer;
	unsigned char * dstPointer;
	tw=(int)((src->width > src->height ? src->width : src->height)*0.0011);
	if(tw<1)tw=1;
	for(y=tw;y<src->height-tw;y++){
		dstPointer=ret->bits + ret->stride * y;
		for(x=tw;x<src->width-tw;x++){
			px=0;
			ti=0;
			for(ty=y-tw;ty<=y+tw;ty++){
				srcPointer=src->bits+src->stride*ty+x-tw;
				for(tx=x-tw;tx<=x+tw;tx++){
					px+=*srcPointer;
					ti++;
					srcPointer++;
				}
			}
			px/=ti;
			*dstPointer=(unsigned char)px;
			dstPointer++;
		}
	}
	return ret;
}
EssBitmap * getBinaryEssBitmapOtsu(EssBitmap * bitmap){
	//用OTSU分割方法分割图像
	if(isEssBitmap(bitmap)==NULL)
		return NULL;
	EssBitmap * src;
	EssBitmap * dst;
	src=bitmap;
	if(src==NULL)
		return NULL;
	dst=createEssBitmap(8,src->width,src->height,NULL);
	if(dst==NULL)
		return NULL;
	int x,y,i,j,l;
	int sx,sy,ex,ey,stpx,stpy,stpxc,stpyc,stpxi,stpyi;
	double hgm[256];
	double u,w0,w1,u0,u1;
	double delt_out=0;	//类间方差值
	double delt_in=0;//类内方差值
	//计算灰度值为i的概率及均值
	double uT = 0;
	double delt_max = 0;
	int threshold=0;
	int srcPointer=2;
	stpx=src->width/srcPointer;
	stpy=src->height/srcPointer;
	stpxc=src->width/stpx;
	stpyc=src->height/stpy;
	for(stpyi=0;stpyi<stpyc;stpyi++){
		for(stpxi=0;stpxi<stpxc;stpxi++){
			sx=stpxi*stpx;
			ex=(stpxi+1)*stpx;
			sy=stpyi*stpy;
			ey=(stpyi+1)*stpy;
			l=(ex-sx)*(ey-sy);
			delt_out=0;delt_in=0;uT = 0;delt_max = 0;
			for(i=0;i<256;i++)
				hgm[i]=0.0;
			//l=src->height*src->width;
			for(y=sy;y<ey;y++){
				for(x=sx;x<ex;x++){
					i=y*src->stride+x;
					if(y<src->height && x<src->width )hgm[src->bits[i]]++;
				}
			}
			for(i = 0; i < 256; i ++){
				//统计灰度值为i的概率	iHeight = row;	iWidth = col;
				hgm[i] /= l;
				//计算均值
				uT += i * hgm[i];
			}
			for(j = 1; j < 256; j ++){
				u = w0 = w1 = u0 = u1 = delt_out =0;
				for(i = 0; i < j; i ++){
					w0 += hgm[i];
					u += i * hgm[i];
				}
				w1 = 1 - w0;
				if( ( w0 < 0.01 ) || ( w1 < 0.01 ) )
					continue;
				u0 = u / w0;
				u1 = (uT - u) / w1;
				////求类间方差值
				delt_out = w0 * w1 * (u1 - u0) * (u1 - u0);
				if(delt_max < delt_out){
					delt_max = delt_out;
					threshold = i;
				}
			}

			for(y = sy; y < ey ; y ++){
				for(x = sx; x < ex ; x ++){
					i=y*dst->stride+x;
					if(y<dst->height && x<dst->width ){
						if(src->bits[i]>threshold)
							dst->bits[i]=0xff;
						else
							dst->bits[i]=0;
					}
				}
			}
		}
	}
	return dst;
}
EssBitmap * getBinaryEssBitmapBkg(EssBitmap * src,EssBitmap * bkg,int dv){
	if(isEssBitmap(src)==NULL || isEssBitmap(bkg)==NULL)
		return NULL;
	if(src->bpp!=8 || bkg->bpp!=8)
		return NULL;
	if(src->stride*src->height!=bkg->stride*bkg->height)
		return NULL;
	int x,y,w,h,s,srcStride,v;
	w=src->width;
	h=src->height;
	s=src->stride;
	EssBitmap * dst=createEssBitmap(8,w,h,NULL);
	if(dst==NULL)
		return NULL;
	unsigned char *srcPointer,*dstPointer,*bp,*srcBit,*db,*bb;
	srcBit=src->bits;
	db=dst->bits;
	bb=bkg->bits;
	for(y=0;y<h;y++){
		srcStride=s*y;
		srcPointer=srcBit+ srcStride ;
		bp=bb+ srcStride ;
		dstPointer=db+ srcStride ;
		for(x=0;x<w;x++){
			v=*bp;
			v-=*srcPointer;
			if(v>dv)
				*dstPointer=0x0;
			else
				*dstPointer=0xff;
			srcPointer++;
			bp++;
			dstPointer++;
		}
	}
	return dst;
}
EssBitmap * getBilateralFilterEssBitmap(EssBitmap * src){
	if(isEssBitmap(src)==NULL)return NULL;
	if(src->bpp!=8)
		return NULL;
	EssBitmap * ret=createEssBitmap(src->bpp,src->width,src->height,src->bits);
	if(ret==NULL)return NULL;
	int x,y,sx,sy,cx,cy,cv,p;
	int w,h;
	double v,sv,rv;

	v=0.0;
	if(inited==0){
		for(y=0;y<3;y++)
			for(x=0;x<3;x++)
				for(cv=0;cv<256;cv++)
					exps[y][x][cv]=exp(-0.001*(x*x+y*y)-0.0001*cv*cv);
		inited=1;
	}
	w=src->width;
	h=src->height;
	p=(int)(0.001*(w>h?w:h));
	if(p<1)p=1;
	if(p>2)p=2;
	unsigned char * srcPointer;
	unsigned char * tp;
	unsigned char * rp;
	for(y=p;y<h-p-1;y++){
		srcPointer=src->bits + src->stride * y;
		rp=ret->bits + ret->stride * y;
		for(x=p;x<w-p-1;x++){
			sv=v=0.0;
			for(sy=y-p;sy<=y+p;sy++){
				tp=src->bits + src->stride * sy + x - p;
				cy=sy-y;
				if(cy<0)cy*=-1;
				for(sx=x-p;sx<=x+p;sx++){
					cx=sx-x;
					cv=*srcPointer;
					cv-=*tp;
					if(cx<0)cx*=-1;
					if(cv<0)cv*=-1;
					rv=exps[cy][cx][cv];
					v+=rv;
					cv=*tp;
					sv+=cv*rv;
					tp++;
				}
			}
			sv/=v;
			if(sv<0)sv=0;
			if(sv>255.0)sv=255.0;
			*rp=(unsigned char)sv;
			srcPointer++;
			rp++;
		}
	}
	return ret;
}
int enhanceEdge(EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	if(bitmap->bpp!=8)
		return 0;
	int x,y,w,h,tx,ty,s, srcStride ;
	unsigned char *srcPointer,*tp,*dstPointer,*srcBit,*db,bv;
	w=bitmap->width;
	h=bitmap->height;
	s=bitmap->stride;
	srcBit=bitmap->bits;
	EssBitmap * tmp=createEssBitmap(8,w,h,NULL);
	if(tmp==NULL)
		return 0;
	db=tmp->bits;
	for(y=1;y<h-1;y++){
		srcStride=y*s+1;
		srcPointer=srcBit+ srcStride ;
		dstPointer=db+ srcStride ;
		for(x=1;x<w-1;x++){
			bv=255;
			for(ty=y-1;ty<=y+1;ty++){
				tp=srcBit+s*ty+x-1;
				for(tx=x-1;tx<=x+1;tx++){
					if(*tp<bv){
						bv=*tp;
						*dstPointer=bv;
					}
					tx++;
				}
			}
			srcPointer++;
			dstPointer++;
		}
	}
	for(y=1;y<h-1;y++){
		srcStride=y*s+1;
		srcPointer= srcBit + srcStride ;
		dstPointer= db + srcStride ;
		for(x=1;x<w-1;x++){
			bv=0;
			for(ty=y-1;ty<=y+1;ty++){
				tp=srcBit+s*ty+x-1;
				for(tx=x-1;tx<=x+1;tx++){
					if(*tp>bv){
						bv=*tp;
						*dstPointer=bv;
					}
					tx++;
				}
			}
			srcPointer++;
			dstPointer++;
		}
	}
	//saveEssBitmap(bitmap);
	deleteEssBitmap(&tmp);
	return 1;
}
EssBitmap * getDocContrastEctEssBitmap(EssBitmap * src,EssBitmap * ebs,EssBitmap * bkg){
	if(isEssBitmap(src)==NULL || isEssBitmap(ebs)==NULL || isEssBitmap(bkg)==NULL)
		return NULL;
	if(src->bpp!=8 || ebs->bpp!=8 || bkg->bpp!=8)
		return NULL;
	int srcStride=src->stride*src->height;
	if( srcStride !=ebs->stride*ebs->height ||  srcStride !=bkg->stride*bkg->height)
		return NULL;
	srcStride=src->stride;
	unsigned char *srcBit,*eb,*bb,*db,*srcPointer,*ep,*bp,*dstPointer;
	int w=src->width;
	int h=src->height;
	int x,y,ev,bv,s;
	EssBitmap * dst=createEssBitmap(8,w,h,NULL);
	if(dst==NULL)
		return NULL;
	srcBit=src->bits;
	eb=ebs->bits;
	bb=bkg->bits;
	db=dst->bits;
	for(y=0;y<h;y++){
		s= srcStride *y;
		srcPointer= srcBit +s;
		ep=eb+s;
		bp=bb+s;
		dstPointer=db+s;
		for(x=0;x<w;x++){
			ev=(int)*ep;
			if(*srcPointer>*bp)
				bv=*srcPointer-*bp;
			else
				bv=*bp-*srcPointer;
			bv=255-bv;
			ev*=bv;
			ev>>=8;
			ev*=ev;
			ev>>=8;
			if(ev>255)ev=255;
			*dstPointer=(unsigned char)ev;
			srcPointer++;
			ep++;
			bp++;
			dstPointer++;
		}
	}
	return dst;

}
EssBitmap * getDocBackgroundEssBitmap(EssBitmap * src,int radius){
	if(isEssBitmap(src)==NULL)
		return NULL;
	if(src->bpp!=8)
		return NULL;
	if(radius<1)
		radius=1;
	EssBitmap * bm1,*bm2;
	int srcWidth,srcHeight,bw,bh;
	int x,y,tx,ty,srcStride,bms;
	unsigned char *srcBit,*tp,*bmp,*bm1b,*bm2b,bv;
	srcStride=src->stride;
	srcBit=src->bits;
	srcWidth=src->width;
	srcHeight=src->height;
	bw=srcWidth/radius;
	bh=srcHeight/radius;
	if(bw<1 || bh<1)
		return NULL;
	bm1=getStretchGrayEssBitmap(src,bw,bh);
	if(bm1==NULL)
		return NULL;
	bm2=createEssBitmap(8,bw,bh,NULL);
	if(bm2==NULL){
		deleteEssBitmap(&bm1);
		return NULL;
	}
	EssBitmap * bwp=NULL;
	bms=bm1->stride;
	bm1b=bm1->bits;
	bm2b=bm2->bits;
	int srcPointerc,srcPointeri;
	srcPointerc=(int)(0.00004*bw*bh);
	if(srcPointerc<1)srcPointerc=1;
	//saveEssBitmap(bm1);
	for(srcPointeri=0;srcPointeri<srcPointerc;srcPointeri++){
		for(y=1;y<bh-1;y++){
			bmp=bm2b + bms * y + 1;
			for(x=1;x<bw-1;x++){
				bv=0;
				for(ty=y-1;ty<=y+1;ty++){
					tp=bm1b + bms * ty + x - 1;
					for(tx=x-1;tx<=x+1;tx++){
						if(*tp>bv){
							*bmp=*tp;
							bv=*tp;
						}
						tp++;
					}
				}
				bmp++;
			}
		}
		bwp=bm2;
		bm2=bm1;
		bm1=bwp;
		bm1b=bm1->bits;
		bm2b=bm2->bits;
	}
	//saveEssBitmap(bm1);
	for(srcPointeri=0;srcPointeri<srcPointerc;srcPointeri++){
		for(y=1;y<bh-1;y++){
			bmp=bm2b + bms * y + 1;
			for(x=1;x<bw-1;x++){
				bv=255;
				for(ty=y-1;ty<=y+1;ty++){
					tp=bm1b + bms * ty + x - 1;
					for(tx=x-1;tx<=x+1;tx++){
						if(*tp<bv){
							*bmp=*tp;
							bv=*tp;
						}
						tp++;
					}
				}
				bmp++;
			}
		}
		bwp=bm2;
		bm2=bm1;
		bm1=bwp;
		bm1b=bm1->bits;
		bm2b=bm2->bits;
	}
	//saveEssBitmap(bm1);
	//bm1
	deleteEssBitmap(&bm2);
	bm2=getStretchGrayEssBitmap(bm1,srcWidth,srcHeight);
	deleteEssBitmap(&bm1);
	//saveEssBitmap(bm2);
	return bm2;
}
EssBitmap * getEqualBrightnessEssBitmap(EssBitmap * src,EssBitmap * docBackground){
	if(isEssBitmap(src)==NULL || isEssBitmap(docBackground)==NULL )
		return NULL;
	if(src->bpp!=8)
		return NULL;
	EssBitmap * bm=docBackground;
	if(bm==NULL)
		return NULL;
	if(src->stride*src->height != bm->stride*bm->height)
		return NULL;
	int x,y,srcStride,srcWidth,srcHeight;
	unsigned char *srcPointer,*srcBit,*bp,*bb,*dstPointer,*db;
	srcStride=src->stride;
	srcBit=src->bits;
	srcWidth=src->width;
	srcHeight=src->height;
	EssBitmap * dst=createEssBitmap(8,srcWidth,srcHeight,NULL);
	if(dst==NULL){
		return NULL;
	}
	bb=bm->bits;
	db=dst->bits;
	int px;
	for(y=0;y<srcHeight;y++){
		srcPointer=srcBit + srcStride * y;
		bp=bb + srcStride * y;
		dstPointer=db + srcStride * y;
		for(x=0;x<srcWidth;x++){
			if(*bp>0){
				px= *srcPointer;
				px<<=8;
				if(*bp>0)
					px/=*bp;
				else
					px=*srcPointer;
				px*=px;
				px>>=8;
				if(px>255)px=255;
				*dstPointer=(unsigned char)px;
			}
			else
				*dstPointer=*srcPointer;
			srcPointer++;
			bp++;
			dstPointer++;
		}
	}
	//saveEssBitmap(dst);
	return dst;
}
EssBitmap * getExpansionEssBitmap(EssBitmap * src,int radius,int contration){
	if(isEssBitmap(src)==NULL)
		return 0;
	if(radius<1)
		radius=(int)((src->width>src->height?src->width:src->height)*0.001);
	if(radius<1)
		radius=1;
	EssBitmap *s,*d,*r;
	r=createEssBitmap(8,src->width,src->height,src->bits);
	if(r==NULL)
		return NULL;
	s=createEssBitmap(8,src->width,src->height,src->bits);
	if(s==NULL){
		deleteEssBitmap(&r);
		return NULL;
	}
	int x,y,st,ri,tx,ty,ef;
	unsigned char *rp,*tp;
	st=s->stride;
	for(ri=0;ri<radius;ri++){
		for(y=1;y<s->height-1;y++){
			rp=s->bits + st * y+1;
			for(x=1;x<s->width-1;x++){
				ef=0;
				for(ty=y-1;ty<=y+1;ty++){
					tp=r->bits + st * ty + x - 1;
					for(tx=x-1;tx<=x+1;tx++){
						if(contration){
							if(*tp>128){
								*rp=255;
								ef=1;
								break;
							}
							else
								*rp=0;
						}else{
							if(*tp<128){
								*rp=0;
								ef=1;
								break;
							}
							else
								*rp=255;
						}
						tp++;
					}
					if(ef)break;
				}
				rp++;
			}
		}
		d=s;
		s=r;
		r=d;
	}
	deleteEssBitmap(&s);
	return r;
}
EssBitmap * getFillCircleEssBitmap(int radius,unsigned char pv,unsigned char bv){
	if(radius<1)
		return NULL;
	int x,y,dx,dy;
	int deltax,deltay;
	int d;
	int xi;
	int w,h,r;
	w=radius*2+1;
	r=radius-1;
	h=w;
	EssBitmap *ret=createEssBitmap(8,w,w,NULL);
	if(ret==NULL)
		return NULL;
	unsigned char * rp, * rb;
	int rs;
	rb=ret->bits;
	rs=ret->stride;
	memset(rb,bv,rs*h);
	x=0;
	y=r;
	deltax=3;
	deltay=2-r-r;
	d=1-r;
	int x0=r;
	int y0=r;
	dx=x+x0;
	dy=y+y0;
	rp=rb + rs * dy +dx;
	if(dx<w && dy<h && dx>=0 && dy>=0)
		*rp=pv;
	dx=x+x0;
	dy=-y+y0;
	rp=rb + rs * dy +dx;
	if(dx<w && dy<h && dx>=0 && dy>=0)
		*rp=pv;
	for(xi=-r+x0;xi<=r+x0;xi++){
		rp=rb + rs * y0 + xi;
		if(xi<w && y0<h && xi>=0 && y0>=0)
			*rp=pv;
	}
	while(x<y){
		if(d<0){
			d+=deltax;
			deltax+=2;
			x++;
		}
		else{
			d+=(deltax+deltay);
			deltax+=2;
			deltay+=2;
			x++;
			y--;
		}
		for(xi=-x+x0;xi<=x+x0;xi++){
			dx=xi;
			dy=-y+y0;
			rp=rb + rs * dy +dx;
			if(dx<w && dy<h && dx>=0 && dy>=0)
				*rp=pv;
			dx=xi;
			dy=y+y0;
			rp=rb + rs * dy +dx;
			if(dx<w && dy<h && dx>=0 && dy>=0)
				*rp=pv;
		}
		for(xi=-y+x0;xi<=y+x0;xi++){
			dx=xi;
			dy=-x+y0;
			rp=rb + rs * dy +dx;
			if(dx<w && dy<h && dx>=0 && dy>=0)
				*rp=pv;
			dx=xi;
			dy=x+y0;
			rp=rb + rs * dy +dx;
			if(dx<w && dy<h && dx>=0 && dy>=0)
				*rp=pv;
		}
	}
	return ret;
}

EssBitmap * convertEssBitmapWithBpp(EssBitmap * bitmap,int bpp){
	if (isEssBitmap(bitmap)==NULL)
		return NULL;
	if (bpp==8 || bpp==24 || bpp==32)
	{
		if (bpp==bitmap->bpp)
			return createEssBitmap(bpp,bitmap->width,bitmap->height,bitmap->bits);
		if(bpp==8)
			return getGrayEssBitmap(bitmap);
		int srcBpp=bitmap->bpp;
		int width=bitmap->width;
		int height=bitmap->height;
		EssBitmap * ret=createEssBitmap(bpp,width,height,NULL);
		if(ret==NULL)
			return 0;
		if(bpp==24 || bpp==32){
			int srcPad=bitmap->stride-srcBpp*width/8;
			int srcColorCount=srcBpp/4;
			int dstPad=ret->stride-bpp*width/8;
			int dstColorCount=bpp/4;
			if(srcColorCount>3)srcColorCount=3;
			if(dstColorCount>3)dstColorCount=3;
			int srcColorStep,dstColorStep=1;
			if(srcBpp==8)
				srcColorStep=0;
			else
				srcColorStep=1;
			int x,y,i;
			unsigned char *sp,*dp;
			sp=bitmap->bits;
			dp=ret->bits;
			for(y=0;y<height;y++){
				for(x=0;x<width;x++){
					for(i=0;i<dstColorCount;i++){
						*dp=*sp;
						sp+=srcColorStep;
						dp+=dstColorStep;
					}
					if(bpp==32){
						*dp=0xff;
						dp++;
					}
					if(srcColorStep==0)
						sp++;
					if(srcBpp==32)
						sp++;
				}
				dp+=dstPad;
				sp+=srcPad;
			}
		}
		return ret;
	}
	else
		return NULL;
}
/*
int saveEssBitmap(EssBitmap * bitmap){
	if(isEssBitmap(bitmap)==NULL)
		return 0;
	ofstream f("E:\\1.pxs",ios::out|ios::binary);
	f.write((char*)&bitmap->bpp,4);
	f.write((char*)&bitmap->stride,4);
	f.write((char*)&bitmap->width,4);
	f.write((char*)&bitmap->height,4);
	f.write((char*)bitmap->bits,bitmap->stride*bitmap->height);
	f.close();
	return 1;
}*/
