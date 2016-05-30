#define _CRT_SECURE_NO_WARNINGS 1
#include "ExamSheetScannerUtil.h"
#include "ExamSheetScannerCore.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************************************/
/**
 * �ⲿ���Ľӿ�C�ļ�
 *
 * �ӿڷ���0��ʾʧ��
 *
 * readEssBitmapBuffer��readEssBitmapΪ����ʶ��ӿڣ��ṩ����ռ���ͼ�����ݼ���ʶ���json
 *
 * ����ӿ�Ϊ�߼��ӿڣ����÷�����Ը��ӣ�����Ϊ���ù���
 *
 * 1.����һ��ʶ��������
 *	 createEssContext ��������ʶ��ͼ��ʶ������ʹ��ʶ����
 * 2.����Ҫʶ���ͼ��������ѡ���������ѡ����һ
 *   setEssBitmapBuffer
 *   setEssBitmap
 * 3.����ʶ��������ڴ���������ʱ���²����Ѿ�ָ����Ĭ��ֵ�����½ӿڸ������ѡ�ã�Ҳ�ɲ���
 *   createEssBitmapForOutput
 *   setDifferRange
 *   setLuminanceSansitivity
 *   setDenstyThresholdVersion1
 *   setDenstyThresholdVersion2
 *   setReadWithVersion
 * 4.��ȡ�����ģ���һ��֮��ʶ��������������������
 *   readContext
 * 5.���������ĵ�ʶ������������Ҫ������Ӧ�ӿ�
 *   ��ȡ���ͼ��
 *   getSourceEssBitmap
 *   getBinaryEssBitmap
 *   getBalanceBinaryEssBitmap
 *   getOutputEssBitmap
 *   ��ȡ�汾����ʾ�ŵ�
 *   getVersion
 *   getLastErrorCode
 *   getCertainty
 *   ��ȡ֤�š��𰸵�
 *   ������Ϣ
 *   getNumberDigit
 *   getSubjectDigit
 *   getAnswerItems
 *   getAnswerCount
 *   ����ָ��
 *   getNumberBufferPtr
 *   getSubjectBufferPtr
 *   getAnswerBufferPtr
 *   getNumberCertaintiesBufferPtr
 *   getSubjectCertaintiesBufferPtr
 *   getAnswerCertaintiesBufferPtr
 *   ��������
 *   copyNumberBuffer
 *   copySubjectBuffer
 *   copyAnswerBuffer
 *   copyNumberCertaintiesBuffer
 *   copySubjectCertaintiesBuffer
 *   copyAnswerCertaintiesBuffer
 */
/*******************************************************************************************************/

/**
 * ʶ��ͼ�񣬳ɹ�������һ��json
 * jsonBuffer���ڴ�����json��buffer
 * bufferSize jsonBuffer�Ĵ�С
 * version ָ��ʶ����Щ�汾
 * bpp,width,height,bitsBuffer ͼ������
 */
EssContext * readEssBitmapBuffer(char * jsonBuffer, int bufferSize, int version, int bpp, int width, int height, unsigned char * bitsBuffer){
	if(jsonBuffer==NULL || bufferSize<1 || bitsBuffer==NULL)
		return 0;
	EssBitmap bitmap;
	bitmap.bpp=bpp;
	bitmap.stride=bpp*width/8;
	if(bitmap.stride%4>0)bitmap.stride+=4-bitmap.stride%4;
	bitmap.width=width;
	bitmap.height=height;
	bitmap.bits=bitsBuffer;
	return readEssBitmap(jsonBuffer,bufferSize,version,&bitmap);
}
/**
 * ʶ��ͼ�񣬳ɹ�������һ��json
 * jsonBuffer���ڴ�����json��buffer
 * bufferSize jsonBuffer�Ĵ�С
 * version ָ��ʶ����Щ�汾
 * bitmap ͼ������
 */
EssContext * readEssBitmap(char * jsonBuffer, int bufferSize, int version, EssBitmap * bitmap){
	if(jsonBuffer==NULL || bufferSize<1 || bitmap==NULL)//�ж�����ͼ������������Ч��
		return 0;
	EssContext * context=createEssContext();//��һ��ʶ�������ģ���������ʶ��ͼ��ʶ������ʹ��ʶ����
	if(context==NULL)
		return 0;
	setReadWithVersion(context,version);//����ֻ���汾
	if(setEssBitmap(context,bitmap)==0){//���ô�ʶ��ͼ��
		deleteEssContext(&context);//���ʧ�ܣ�ɾ�������ģ�������
		return 0;
	}
	int ret=readContext(context);//��ȡ�����ģ���һ��֮��ʶ��������������������
	int i,j,k,l;
	char * json=NULL;
	char schar[100]={0};
	char abcd[16]={0};
	int slen;
	//��֯json
	slen=strjn(&json,"{\r\n");
	slen=strjn(&json,"    \"testCard\":[\r\n");
	slen=strjn(&json,"        {\r\n");
	slen=strjn(&json,"            \"Status\":\"\",\r\n");
	sprintf(schar,"            \"lastErrorCode\":\"%d\",\r\n",context->lastErrorCode);
	int retv=getLastErrorCode(context);//��ȡʶ��������
	slen=strjn(&json,schar);
	//���ݽ������������ʾ
	if(retv==unknowError)
		slen=strjn(&json,"            \"Msg\":\"Recognition failure!\",\r\n");
	if(retv==outMemory)
		slen=strjn(&json,"            \"Msg\":\"Out of memory!\",\r\n");
	if(retv==lostCorner)
		slen=strjn(&json,"            \"Msg\":\"Some of the corners were not detected!\",\r\n");
	if(retv==lostLocation)
		slen=strjn(&json,"            \"Msg\":\"Some of the locations were not detected!\",\r\n");
	if(retv==lostString)
		slen=strjn(&json,"            \"Msg\":\"Some of the strings were not detected!\",\r\n");
	if(retv==tooFarAway)
		slen=strjn(&json,"            \"Msg\":\"The camera is too far away from the Exam Sheet!\",\r\n");
	if(retv==success)
		slen=strjn(&json,"            \"Msg\":\"Recognition success!\",\r\n");
	slen=strjn(&json,"            \"certainty\":\"");
	sprintf(schar,"%d",getCertaintyRange(context->certainty) );//��ȡȫ�����Ŷ�
	slen=strjn(&json,&schar[0]);
	slen=strjn(&json,"\",\r\n");
	slen=strjn(&json,"            \"cardId\":\"");
	if(ret!=0 && context->numberBuffer!=NULL){//����׼��֤��
		k=0;
		for(i=0;i<context->numberDigit;i++){//��λ���н���
			l=0;
			for(j=0;j<10;j++){//ÿλ����10���㣬��Ӧ0-9ʮ����
				if(context->numberBuffer[k] !=0 && l==0 ){//�����Ӧ�ĵ㲻Ϊ0����λ���ǵ�ǰ�����
					sprintf(schar,"%d",j);
					slen=strjn(&json,&schar[0]);
					l=1;
				}
				k++;
			}
			if(l==0)
				slen=strjn(&json,"_");
		}
	}
	slen=strjn(&json,"\",\r\n");
	slen=strjn(&json,"            \"subject\":\"");
	if(ret!=0 && context->subjectBuffer!=NULL){//����������
		k=0;
		for(i=0;i<context->subjectDigit;i++){//��λ���н���
			l=0;
			for(j=0;j<10;j++){//ÿλ����10���㣬��Ӧ0-9ʮ����
				if(context->subjectBuffer[k] !=0 && l==0 ){//�����Ӧ�ĵ㲻Ϊ0����λ���ǵ�ǰ�����
					sprintf(schar,"%d",j);
					slen=strjn(&json,&schar[0]);
					l=1;
				}
				k++;
			}
			if(l==0)
				slen=strjn(&json,"_");
		}
	}
	slen=strjn(&json,"\",\r\n");
	slen=strjn(&json,"            \"userName\":\"Name\",\r\n");
	if(ret!=0 && context->answerBuffer!=NULL)
		slen=strjn(&json,"            \"className\":\"Class\",\r\n");
	else{
		slen=strjn(&json,"            \"className\":\"Class\"\r\n");
		slen=strjn(&json,"        }\r\n");
	}
	int m;
	unsigned char cc,cen;
	if(ret!=0 && context->answerBuffer!=NULL){//������
		cc=65;
		for(i=0;i<16;i++){//��ʼ��һ����ABCDEF.....�����ַ��������ڸ�������ȡ�ַ�
			abcd[i]=cc;
			cc++;
		}
		slen=strjn(&json,"            \"answer\":[\r\n");
		slen=strjn(&json,"                {\r\n");
		k=0;
		m=0;
		for(i=0;i<context->answerCount;i++){//������н���
			slen=strjn(&json,"                    \"testindex\":\"");
			sprintf(schar,"%d",i+1);
			slen=strjn(&json,&schar[0]);
			slen=strjn(&json,"\",\r\n");
			l=0;
			cen=100;
			for(j=0;j<context->answerItems;j++){//ѭ���жϵ����ÿ��ѡ��
				if(context->answerCertainties[m]<cen)//�ҳ�ÿ����С�����Ŷ���Ϊ��������Ŷ�
					cen=context->answerCertainties[m];
				if(context->answerBuffer[k] !=0 ){//��Ϊ0��ʾ��������
					schar[l]=abcd[j];//�õ�ǰ�����Ż�ȡABCD...
					l++;
				}
				m++;
				k++;
			}
			schar[l]=0;
			slen=strjn(&json,"                    \"value\":\"");
			slen=strjn(&json,&schar[0]);
			slen=strjn(&json,"\",\r\n");
			slen=strjn(&json,"                    \"certainty\":\"");
			sprintf(schar,"%d",getCertaintyRange( cen));
			slen=strjn(&json,&schar[0]);
			slen=strjn(&json,"\"\r\n");
			if(i<context->answerCount-1){
				slen=strjn(&json,"                },\r\n");
				slen=strjn(&json,"                {\r\n");
			}
			else
				slen=strjn(&json,"                }\r\n");
		}
		slen=strjn(&json,"            ]\r\n");
		slen=strjn(&json,"        }\r\n");
	}
	slen=strjn(&json,"    ]\r\n");
	slen=strjn(&json,"}\r\n");
	//deleteEssContext(&context);//ɾ��������
	i=bufferSize<slen?bufferSize:slen;//����json���������
	memcpy(jsonBuffer,json,i);
	jsonBuffer[i-1]=0;
	return context;//����json����
}



/**����������
 *
 */
EssContext * createEssContext(){
	EssContext * ret=(EssContext*)malloc(sizeof(EssContext));
	if(ret==NULL)
		return NULL;
	ret->differRange=40;
	ret->luminanceSansitivity=15;
	ret->denstyThresholdVersion1=60;
	ret->denstyThresholdVersion2=80;
	ret->withVersion=0;
	ret->minSize=480;
	ret->sourceEssBitmap=NULL;
	ret->binaryEssBitmap=NULL;
	ret->balanceBinaryEssBitmap=NULL;
	ret->outputEssBitmap=NULL;
	ret->version=0;
	ret->lastErrorCode=unknowError;
	ret->information=NULL;
	ret->numberDigit=0;
	ret->subjectDigit=0;
	ret->answerItems=0;
	ret->answerCount=0;
	ret->certainty=0;
	ret->numberBuffer=NULL;
	ret->numberCertainties=NULL;
	ret->subjectBuffer=NULL;
	ret->subjectCertainties=NULL;
	ret->answerBuffer=NULL;
	ret->answerCertainties=NULL;
	return ret;
}
/**ɾ��������
 * context �����ĵ�ָ��
 *
 */
void deleteEssContext(EssContext ** context){
	if(context==NULL)
		return;
	if(*context==NULL)
		return;
	if((*context)->sourceEssBitmap!=NULL)
		deleteEssBitmap(&(*context)->sourceEssBitmap);
	if((*context)->binaryEssBitmap!=NULL)
		deleteEssBitmap(&(*context)->binaryEssBitmap);
	if((*context)->balanceBinaryEssBitmap!=NULL)
		deleteEssBitmap(&(*context)->balanceBinaryEssBitmap);
	if((*context)->information!=NULL)
		deleteEssInformation((EssInformation**) &(*context)->information);
	if((*context)->numberBuffer)
		free((*context)->numberBuffer);
	if((*context)->numberCertainties)
		free((*context)->numberCertainties);
	if((*context)->subjectBuffer)
		free((*context)->subjectBuffer);
	if((*context)->subjectCertainties)
		free((*context)->subjectCertainties);
	if((*context)->answerBuffer)
		free((*context)->answerBuffer);
	if((*context)->answerCertainties)
		free((*context)->answerCertainties);
	free(*context);
	*context=NULL;
}
/**����ʶ���ͼ�����������
 * context������
 * bpp,width,height,bitsBuffer ͼ������
 */
int setEssBitmapBuffer(EssContext * context,int bpp,int width,int height,unsigned char * bitsBuffer){
	if(context==NULL || bitsBuffer==NULL)
		return 0;
	EssBitmap bitmap;
	bitmap.bpp=bpp;
	bitmap.stride=width*bpp/8;
	if(bitmap.stride%4)bitmap.stride+=4-bitmap.stride%4;
	bitmap.width=width;
	bitmap.height=height;
	bitmap.bits=bitsBuffer;
	return setEssBitmap(context,&bitmap);
}
/**����ʶ���ͼ�����������
 * context������
 * bitmap ͼ������
 */
int setEssBitmap(EssContext * contex,EssBitmap * bitmap){
	if(contex==NULL || isEssBitmap(bitmap)==NULL)
		return 0;
	EssBitmap * filteringBitmap=NULL;
	if(bitmap->bpp==8)
		filteringBitmap=getBilateralFilterEssBitmap(bitmap);
	else{
		EssBitmap * tmpBitmap=getGrayEssBitmap(bitmap);
		if(tmpBitmap==NULL){
			contex->lastErrorCode=outMemory;
			return 0;
		}
		filteringBitmap=getBilateralFilterEssBitmap(tmpBitmap);
		deleteEssBitmap(&tmpBitmap);
	}
	if(filteringBitmap==NULL){
		contex->lastErrorCode=outMemory;
		return 0;
	}
	enhanceEdge(filteringBitmap);
	EssBitmap * backgroundBitmap=getDocBackgroundEssBitmap(filteringBitmap,3);
	if(backgroundBitmap==NULL){
		contex->lastErrorCode=outMemory;
		deleteEssBitmap(&filteringBitmap);
		return 0;
	}
	EssBitmap * balanceBitmap=getEqualBrightnessEssBitmap(filteringBitmap,backgroundBitmap);
	if(balanceBitmap==NULL){
		contex->lastErrorCode=outMemory;
		deleteEssBitmap(&filteringBitmap);
		deleteEssBitmap(&backgroundBitmap);
		return 0;
	}
	EssBitmap * binaryBitmap=getBinaryEssBitmapOtsu(balanceBitmap);
	if(binaryBitmap==NULL){
		contex->lastErrorCode=outMemory;
		deleteEssBitmap(&filteringBitmap);
		deleteEssBitmap(&backgroundBitmap);
		deleteEssBitmap(&balanceBitmap);
		return 0;
	}
	EssBitmap * balanceBinaryBitmap=getBinaryEssBitmapBkg(filteringBitmap,backgroundBitmap,contex->luminanceSansitivity);
	if(balanceBinaryBitmap==NULL){
		contex->lastErrorCode=outMemory;
		deleteEssBitmap(&filteringBitmap);
		deleteEssBitmap(&backgroundBitmap);
		deleteEssBitmap(&balanceBitmap);
		deleteEssBitmap(&binaryBitmap);
		return 0;
	}
	EssBitmap * sourceBitmap=createEssBitmap(bitmap->bpp,bitmap->width,bitmap->height,bitmap->bits);
	if(sourceBitmap==NULL){
		contex->lastErrorCode=outMemory;
		deleteEssBitmap(&filteringBitmap);
		deleteEssBitmap(&backgroundBitmap);
		deleteEssBitmap(&balanceBitmap);
		deleteEssBitmap(&binaryBitmap);
		deleteEssBitmap(&balanceBinaryBitmap);
		return 0;
	}
	contex->sourceEssBitmap=sourceBitmap;
	contex->binaryEssBitmap=binaryBitmap;
	contex->balanceBinaryEssBitmap=balanceBinaryBitmap;
	deleteEssBitmap(&filteringBitmap);
	deleteEssBitmap(&backgroundBitmap);
	deleteEssBitmap(&balanceBitmap);
	return 1;
}
/**
 * ����һ���������ʶ����̵�ͼ��
 * context ������
 * bitmapIndex ����ʲôͼ����������0ԭͼ��1���ͼ��2�ܶ�ͼ��3��ɫ����ͼ
 * bpp ���ͼ�����ɫ��ȣ���֧��8��24��32
 */
int createEssBitmapForOutput(EssContext * context,int bitmapIndex,int bpp){
	if(context==NULL)
		return 0;
	if (bpp==8 || bpp==24 || bpp==32)
	{
		EssBitmap * bmp=NULL;
		if(bitmapIndex==0)
			bmp=context->sourceEssBitmap;
		else if(bitmapIndex==1)
			bmp=context->binaryEssBitmap;
		else if(bitmapIndex==2)
			bmp=context->balanceBinaryEssBitmap;
		EssBitmap * bitmap= NULL;
		if(bmp!=NULL)
			bitmap=convertEssBitmapWithBpp(bmp,bpp);
		else
			bitmap=createEssBitmap(bpp,context->sourceEssBitmap->width,context->sourceEssBitmap->height,NULL);
		if(bitmap==NULL)
			return 0;
		if(bmp==NULL)
			memset(bitmap->bits,0xff,bitmap->stride*bitmap->height);
		context->outputEssBitmap=bitmap;
		return 1;
	}
	return 0;
}
/**
 * ���ü��������Χ
 * context������
 * value 20��50
 */
int setDifferRange(EssContext * context,int value){
	if(context==NULL)
		return 0;
	int nv=value;
	if(nv<1)nv=1;
	if(nv>50)nv=50;
	context->differRange=nv;
	return 1;
}
/**
 * �������Ȳ����жȣ�������ǰ�������Ȳ���ھ�ȷ��ֵ����
 * context������
 * value 0��255
 */
int setLuminanceSansitivity(EssContext * context,int value){
	if(context==NULL)
		return 0;
	int nv=value;
	if(nv<0)nv=0;
	if(nv>254)nv=254;
	context->luminanceSansitivity=nv;
	return 1;
}
/**
 * ������Ϳ�б��ܶȣ���⵽���ܶȴ��ڸ�ֵʱ�б�Ϊ����Ϳ
 * context ������
 * value 50��95
 */
int setDenstyThresholdVersion1(EssContext * context,int value){
	if(context==NULL)
		return 0;
	int nv=value;
	if(nv<1)nv=1;
	if(nv>100)nv=100;
	context->denstyThresholdVersion1=nv;
	return 1;
}
/**
 * ������Ϳ�б��ܶȣ���⵽���ܶȴ��ڸ�ֵʱ�б�Ϊ����Ϳ
 * context ������
 * value 50��95
 */
int setDenstyThresholdVersion2(EssContext * context,int value){
	if(context==NULL)
		return 0;
	int nv=value;
	if(nv<1)nv=1;
	if(nv>100)nv=100;
	context->denstyThresholdVersion2=nv;
	return 1;
}
/**
 * �趨ֻ�����⿨�汾��
 * context ������
 * version 0��ʾ���а汾��1��ʾ1�棬2��ʾ2��
 */
int setReadWithVersion(EssContext * context,int version){
	if(context==NULL)
		return 0;
	if(version==0 || version==1 || version==2){
		context->withVersion=version;
		return 1;
	}
	return 0;
}
/**
 * �����������⿨ͼ�����С�ߴ磬С�ڸ�ֵʱ����Ϊ���⿨��������ͷ��Զ������ʶ��
 * context ������
 * minSize 240������ͷ���ͼ�����ֱ���-100
 */
int setMinSize(EssContext * context,int minSize){
	if(context==NULL)
		return 0;
	if(minSize>240){
		context->minSize=minSize;
		return 1;
	}
	return 0;
}
/**
 * ��ȡ������
 * �����ȴ��������Ĳ�����ͼ��
 * ��Ҫ���ò���ʱ�����ú��ٵ���
 * ���ú�ſ��Լ��������ĵ�����
 * �������ݺ����ɾ��������
 * context ������
 */
int readContext(EssContext * context){
	if(context==NULL)
		return 0;
	if(getCorners(context)==0)
		return 0;
	int ret=0;
	if(context->version==1)
		ret= readPoints(context);
	if(context->version==2)
		ret= readEssStrings(context);
	if(ret!=0)
		context->lastErrorCode=success;
	return ret;
}
/**
 * ��ȡ�������е�ԭͼ
 * context ������
 * bpp ��ȡʱת���ɸ�ɫ��
 */
EssBitmap * getSourceEssBitmap(EssContext * context,int bpp){
	if(context==NULL)
		return 0;
	return convertEssBitmapWithBpp(context->sourceEssBitmap,bpp);
}
/**
 * ��ȡ�������еļ��ͼ
 * context ������
 * bpp ��ȡʱת���ɸ�ɫ��
 */
EssBitmap * getBinaryEssBitmap(EssContext * context,int bpp){
	if(context==NULL)
		return 0;
	return convertEssBitmapWithBpp(context->binaryEssBitmap,bpp);
}
/**
 * ��ȡ�������е��ܶ�ͼ
 * context ������
 * bpp ��ȡʱת���ɸ�ɫ��
 */
EssBitmap * getBalanceBinaryEssBitmap(EssContext * context,int bpp){
	if(context==NULL)
		return 0;
	return convertEssBitmapWithBpp(context->balanceBinaryEssBitmap,bpp);
}
/**
 * ��ȡ�������е����ͼ
 * context ������
 * bpp ��ȡʱת���ɸ�ɫ��
 */
EssBitmap * getOutputEssBitmap(EssContext * context,int bpp){
	if(context==NULL)
		return 0;
	return convertEssBitmapWithBpp(context->outputEssBitmap,bpp);
}
/**
 * ���ʶ��ɹ����ش��⿨�İ汾��
 * context ������
 */
int getVersion(EssContext * context){
	if(context==NULL)
		return 0;
	return context->version;
}
/**
 * ��ȡ�����������
 * context ������
 */
int getLastErrorCode(EssContext * context){
	if(context==NULL)
		return 0;
	return (int)context->lastErrorCode;
}
/**
 * ��ȡ׼��֤λ��
 * context ������
 */
int getNumberDigit(EssContext * context){
	if(context==NULL)
		return 0;
	return context->numberDigit;
}
/**
 * ��ȡ������λ��
 * context ������
 */
int getSubjectDigit(EssContext * context){
	if(context==NULL)
		return 0;
	return context->subjectDigit;
}
/**
 * ��ȡ�𰸵�ѡ����
 * context ������
 */
int getAnswerItems(EssContext * context){
	if(context==NULL)
		return 0;
	return context->answerItems;
}
/**
 * ��ȡ������
 * context ������
 */
int getAnswerCount(EssContext * context){
	if(context==NULL)
		return 0;
	return context->answerCount;
}
/**
 * ��ȡȫ�����Ŷ�
 * context ������
 */
int getCertainty(EssContext * context){
	if(context==NULL)
		return 0;
	return context->certainty;
}
/*
 * ��ȡ׼��֤�����ݻ�����ָ��
 * context ������
 *
 */
unsigned char * getNumberBufferPtr(EssContext * context){
	if(context==NULL)
		return 0;
	return context->numberBuffer;
}
/*
 * ��ȡ���������ݻ�����ָ��
 * context ������
 *
 */
unsigned char * getSubjectBufferPtr(EssContext * context){
	if(context==NULL)
		return 0;
	return context->subjectBuffer;
}
/*
 * ��ȡ�����ݻ�����ָ��
 * context ������
 *
 */
unsigned char * getAnswerBufferPtr(EssContext * context){
	if(context==NULL)
		return 0;
	return context->answerBuffer;
}
/*
 * ����׼��֤�����ݵ�ָ��������
 * context ������
 * outputBufferPtr Ŀ�껺����ָ��
 * outputBufferSize Ŀ�껺������С
 *
 */
int copyNumberBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize){
	if(context==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int size=context->numberDigit*10;
	int i;
	for( i=0;i<outputBufferSize;i++){
		if(i<size && i< outputBufferSize)
			outputBufferPtr[i]=context->numberBuffer[i];
		else
			outputBufferPtr[i]=0;
	}
	return 1;
}
/*
 * �������������ݵ�ָ��������
 * context ������
 * outputBufferPtr Ŀ�껺����ָ��
 * outputBufferSize Ŀ�껺������С
 *
 */
int copySubjectBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize){
	if(context==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int size=context->subjectDigit*10;
	int i;
	for(i=0;i<outputBufferSize;i++){
		if(i<size && i< outputBufferSize)
			outputBufferPtr[i]=context->subjectBuffer[i];
		else
			outputBufferPtr[i]=0;
	}
	return 1;
}
/*
 * ���ƴ����ݵ�ָ��������
 * context ������
 * outputBufferPtr Ŀ�껺����ָ��
 * outputBufferSize Ŀ�껺������С
 *
 */
int copyAnswerBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize){
	if(context==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int size=context->answerCount*context->answerItems;
	int i;
	for(i=0;i<outputBufferSize;i++){
		if(i<size && i< outputBufferSize)
			outputBufferPtr[i]=context->answerBuffer[i];
		else
			outputBufferPtr[i]=0;
	}
	return 1;
}
/*
 * ��ȡ׼��֤�����ŶȻ�����ָ��
 * context ������
 *
 */
unsigned char * getNumberCertaintiesBufferPtr(EssContext * context){
	if(context==NULL)
		return 0;
	return context->numberCertainties;
}
/*
 * ��ȡ���������ŶȻ�����ָ��
 * context ������
 *
 */
unsigned char * getSubjectCertaintiesBufferPtr(EssContext * context){
	if(context==NULL)
		return 0;
	return context->subjectCertainties;
}
/*
 * ��ȡ�����ŶȻ�����ָ��
 * context ������
 *
 */
unsigned char * getAnswerCertaintiesBufferPtr(EssContext * context){
	if(context==NULL)
		return 0;
	return context->answerCertainties;
}
/*
 * ����׼��֤�����Ŷȵ�ָ��������
 * context ������
 * outputBufferPtr Ŀ�껺����ָ��
 * outputBufferSize Ŀ�껺������С
 *
 */
int copyNumberCertaintiesBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize){
	if(context==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int size=context->numberDigit*10;
	int i;
	for(i=0;i<outputBufferSize;i++){
		if(i<size && i< outputBufferSize)
			outputBufferPtr[i]=context->numberCertainties[i];
		else
			outputBufferPtr[i]=0;
	}
	return 1;
}
/*
 * �������������Ŷȵ�ָ��������
 * context ������
 * outputBufferPtr Ŀ�껺����ָ��
 * outputBufferSize Ŀ�껺������С
 *
 */
int copySubjectCertaintiesBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize){
	if(context==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int size=context->subjectDigit*10;
	int i;
	for(i=0;i<outputBufferSize;i++){
		if(i<size && i< outputBufferSize)
			outputBufferPtr[i]=context->subjectCertainties[i];
		else
			outputBufferPtr[i]=0;
	}
	return 1;
}
/*
 * ���ƴ����Ŷȵ�ָ��������
 * context ������
 * outputBufferPtr Ŀ�껺����ָ��
 * outputBufferSize Ŀ�껺������С
 */
int copyAnswerCertaintiesBuffer(EssContext * context,unsigned char * outputBufferPtr,int outputBufferSize){
	if(context==NULL || outputBufferPtr==NULL || outputBufferSize<1)
		return 0;
	int size=context->answerCount*context->answerItems;
	int i;
	for(i=0;i<outputBufferSize;i++){
		if(i<size && i< outputBufferSize)
			outputBufferPtr[i]=context->answerCertainties[i];
		else
			outputBufferPtr[i]=0;
	}
	return 1;
}