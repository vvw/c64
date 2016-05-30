#pragma once
#define  NULL 0
typedef struct EssBitmap{
	int bpp;
	int stride;
	int width;
	int height;
	unsigned char * bits;
}EssBitmap;


EssBitmap * createEssBitmap(int bpp,int width,int height,unsigned char * bits);
void deleteEssBitmap(EssBitmap ** bitmap);

EssBitmap * isEssBitmap(EssBitmap * bitmap);

int getEssBitmapBpp(EssBitmap * bitmap);
int getEssBitmapStride(EssBitmap * bitmap);
int getEssBitmapWidth(EssBitmap * bitmap);
int getEssBitmapHeight(EssBitmap *bitmap);
unsigned char * getEssBitmapBitsPtr(EssBitmap * bitmap);
int copyEssBitmapBits(unsigned char * outputBufferPtr,int outputBufferSize,EssBitmap * bitmap);

EssBitmap * copyEssBitmapNearPoint(EssBitmap * src,int x,int y,int width,int height);
EssBitmap * getStretchGrayEssBitmap(EssBitmap * src,int dstBitmapWidth,int dstBitmapHeight);
int stretchGrayEssBitmap(EssBitmap * src,EssBitmap * dst);
EssBitmap * getGrayEssBitmapFromeBuffer(int bpp,int width,int height,unsigned char * buf,double rate);
EssBitmap * getGrayEssBitmap(EssBitmap * src);
EssBitmap * getBinaryEssBitmapOtsu(EssBitmap * bitmap);
EssBitmap * getMeanFilterEssBitmap(EssBitmap * src);
EssBitmap * getBilateralFilterEssBitmap(EssBitmap * bitmap);
EssBitmap * getBinaryEssBitmapBkg(EssBitmap * src,EssBitmap * bkg,int dv);
int enhanceEdge(EssBitmap * bitmap);
EssBitmap * getDocContrastEctEssBitmap(EssBitmap * src,EssBitmap * ebs,EssBitmap * docBackground);
EssBitmap * getDocBackgroundEssBitmap(EssBitmap * src,int radius);
EssBitmap * getEqualBrightnessEssBitmap(EssBitmap * src,EssBitmap * docBackground);
EssBitmap * getExpansionEssBitmap(EssBitmap * src,int radius,int contration);
EssBitmap * getFillCircleEssBitmap(int r,unsigned char pv,unsigned char bv);
EssBitmap * convertEssBitmapWithBpp(EssBitmap * bitmap,int bpp);
int saveEssBitmap(EssBitmap * bitmap);