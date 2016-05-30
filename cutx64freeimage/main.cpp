
#include <assert.h>
#include "stdio.h"

#include "FreeImage.h"
#pragma comment(lib, "FreeImage.lib")

#include "./src/ExamSheetScannerCore.h"
#include "./src/ExamSheetScannerUtil.h"



FIBITMAP* GenericLoader(const char* lpszPathName, int flag) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(lpszPathName, 0);
	if (fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	// check that the plugin has reading capabilities ...
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// ok, let's load the file
		FIBITMAP *dib = FreeImage_Load(fif, lpszPathName, flag);
		// unless a bad file format, we are done !
		return dib;
	}
	return NULL;
}

// 保存图片，第rowStart 到第rowEnd
int saveCut(EssBitmap *bitm, int rowStart, int rowEnd, char *fname) {

	if (rowStart > rowEnd || rowStart < 0 || rowEnd < 0) {
		return -1;
	}

	// 创建一个空的图片
	FIBITMAP *bit = FreeImage_Allocate(bitm->width, rowEnd - rowStart, bitm->bpp);

	EssBitmap bitmapp;
	bitmapp.bpp = FreeImage_GetBPP(bit);
	bitmapp.stride = FreeImage_GetLine(bit);
	unsigned pitch = FreeImage_GetPitch(bit);  // 四字节对齐后的结果？
	if (bitmapp.stride % 4>0)bitmapp.stride += 4 - bitmapp.stride % 4;
	bitmapp.width = FreeImage_GetWidth(bit);
	bitmapp.height = FreeImage_GetHeight(bit);
	bitmapp.bits = (unsigned char *)(void*)FreeImage_GetBits(bit);

	// 一个个像素赋值
	unsigned char * start = bitmapp.bits;
	unsigned char * start2 = bitm->bits + rowStart * bitm->stride;
	for (int i = 0; i < bitmapp.height; i++) {
		for (int j = 0; j < bitmapp.width; j++) {
			start[j] = start2[j]; // 115;
		}
		start = start + bitmapp.stride;
		start2 = start2 + bitm->stride;
	}

	//FIBITMAP *rotated = FreeImage_Rotate(bit, 180);
	//int okQ = FreeImage_FlipHorizontal(rotated);
	//FreeImage_Save(FIF_JPEG, rotated, "E:\\00000.jpg", JPEG_DEFAULT);
	FreeImage_Save(FIF_JPEG, bit, fname, JPEG_DEFAULT);
	FreeImage_Unload(bit);

	return 1;
}


int main() {

	///// start of 16-03-20
	FIBITMAP *dib = GenericLoader("ExamSheet2_20_D_red-0311.jpg", 0);
	if (!dib) {
		FreeImage_DeInitialise();
		assert(0);
	}

	//转成灰度图
	FIBITMAP * gray = FreeImage_ConvertTo8Bits(dib);
	if (!gray){
		FreeImage_Unload(dib);
		FreeImage_DeInitialise();
		assert(0);
	}
	if (FreeImage_GetBPP(gray) != 8){
		FreeImage_Unload(dib);
		FreeImage_Unload(gray);
		FreeImage_DeInitialise();
		assert(0);
	}


	int grayWidth = FreeImage_GetWidth(gray);
	int grayHeight = FreeImage_GetHeight(gray);
	double rr = (double)grayWidth / grayHeight; // 原图宽高比
	int newHight = 1280;  // 固定高度，重新计算宽度
	int newWidth = (int)(rr * newHight);



	// 缩放 // 降采样
	FIBITMAP * grayRescale = FreeImage_Rescale(gray, newWidth, newHight, FILTER_BILINEAR);

	FreeImage_Save(FIF_JPEG, grayRescale, "grayRescale.jpg", JPEG_DEFAULT);


	EssBitmap bitmap2;
	bitmap2.bpp = FreeImage_GetBPP(grayRescale);
	bitmap2.stride = FreeImage_GetLine(grayRescale);
	if (bitmap2.stride % 4>0)bitmap2.stride += 4 - bitmap2.stride % 4;
	bitmap2.width = FreeImage_GetWidth(grayRescale);
	bitmap2.height = FreeImage_GetHeight(grayRescale);
	bitmap2.bits = (unsigned char *)(void*)FreeImage_GetBits(grayRescale);
	///// end   of 16-03-20

	EssContext * context = createEssContext();//建一个识别上下文，用于设置识别图像、识别参数和存放识别结果
	if (context == NULL)
		assert(0);
	setReadWithVersion(context, 2);//设置只读版本
	if (setEssBitmap(context, &bitmap2) == 0){//设置待识别图像
		//if (setEssBitmap(context, &bitmap) == 0){//设置待识别图像
		deleteEssContext(&context);//如果失败，删除上下文，并返回
		assert(0);
	}

	EssBitmap *bitm = context->binaryEssBitmap;

	EssCornerArray * eca = getCornerArray(bitm, (double)context->differRange / 100.0);//获取所有候选角点
	if (eca == NULL)
		assert(0);

	if (eca->count<4){//如果候选角点小于4个，极可能是没对正答题卡
		assert(0);
	}

	int i; int j;
	for (i = 0; i<eca->count - 1; i++){//取四个最大的角点
		for (j = i; j<eca->count; j++){
			if (eca->corners[j].rect.right - eca->corners[j].rect.left  >  eca->corners[i].rect.right - eca->corners[i].rect.left){
				swapCorner(&eca->corners[j], &eca->corners[i]);
			}
		}
	}

	j = 0;
	for (i = 1; i<eca->count; i++){//从四个最大的角点获取相似的角点作为候选角点
		if (likeCorner(&eca->corners[0], &eca->corners[i], (double)context->differRange / 100.0))
			j++;
	}
	if (j != 3){//如果候选角点不等于4个，极可能是没对正答题卡或出现多张答题卡
		assert(0);
	}

	// 主观题有若干个小角点，对它们排序，bottom 坐标小的排前面
	if (eca->count % 2 != 0) {
		assert(0);
	}
	else if (eca->count > 4) {
		j = 0;
		for (i = 5; i < eca->count; i++) {
			if (likeCorner(&eca->corners[4], &eca->corners[i], (double)context->differRange / 100.0))
				j++;
		}
		// j 应该等于eca->count - 5
		i = 0;
		for (i = 4; i < eca->count - 1; i++) {  // 冒泡排序
			for (j = i + 1; j < eca->count; j++) {
				if (eca->corners[i].rect.bottom > eca->corners[j].rect.bottom) {
					swapCorner(&eca->corners[i], &eca->corners[j]);
				}
			}
		}

	}

	//saveBmp(bitm->width, eca->corners[5].rect.top - eca->corners[4].rect.bottom, bitm->stride, bitm->bits + eca->corners[4].rect.bottom * bitm->stride, "E:\\13corner.bmp");  //
	//saveBmp(bitm->width, eca->corners[7].rect.top - eca->corners[6].rect.bottom, bitm->stride, bitm->bits + eca->corners[6].rect.bottom * bitm->stride, "E:\\14corner.bmp");  //

	int saveCut(EssBitmap *bitm, int rowStart, int rowEnd, char *fname);
	saveCut(bitm, eca->corners[4].rect.bottom, eca->corners[5].rect.top, "222.jpg");
	saveCut(bitm, eca->corners[6].rect.bottom, eca->corners[7].rect.top, "333.jpg");

	// 对四个大角点排序，截大矩形
	i = 0;
	for (i = 0; i < 4 - 1; i++) {  // 冒泡排序
		for (j = i + 1; j < 4; j++) {
			if (eca->corners[i].rect.bottom > eca->corners[j].rect.bottom) {
				swapCorner(&eca->corners[i], &eca->corners[j]);
			}
		}
	}

	//int saveCut(EssBitmap *bitm, int rowStart, int rowEnd, char *fname);
	saveCut(bitm, eca->corners[0].rect.bottom, eca->corners[3].rect.top, "111.jpg");


	FIBITMAP *bit = FreeImage_Allocate(bitm->width, eca->corners[3].rect.top - eca->corners[0].rect.bottom, bitm->bpp);

	EssBitmap bitmapp;
	bitmapp.bpp = FreeImage_GetBPP(bit);
	bitmapp.stride = FreeImage_GetLine(bit);
	unsigned pitch = FreeImage_GetPitch(bit);  // 四字节对齐后的结果？
	if (bitmapp.stride % 4>0)bitmapp.stride += 4 - bitmapp.stride % 4;
	bitmapp.width = FreeImage_GetWidth(bit);
	bitmapp.height = FreeImage_GetHeight(bit);
	bitmapp.bits = (unsigned char *)(void*)FreeImage_GetBits(bit);

	unsigned char * start = bitmapp.bits;
	unsigned char * start2 = bitm->bits + eca->corners[0].rect.bottom * bitm->stride;
	for (int i = 0; i < bitmapp.height; i++) {
		for (int j = 0; j < bitmapp.width; j++) {
			start[j] = start2[j]; // 115;
		}
		start = start + bitmapp.stride;
		start2 = start2 + bitm->stride;
	}

	//FIBITMAP *rotated = FreeImage_Rotate(bit, 180);
	//int okQ = FreeImage_FlipHorizontal(rotated);
	//FreeImage_Save(FIF_JPEG, rotated, "E:\\00000.jpg", JPEG_DEFAULT);
	FreeImage_Save(FIF_JPEG, bit, "00000.jpg", JPEG_DEFAULT);
	FreeImage_Unload(bit);



	printf("hi,,%s", FreeImage_GetVersion());
}


