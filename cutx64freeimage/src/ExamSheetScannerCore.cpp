#define _CRT_SECURE_NO_WARNINGS 1
#include "ExamSheetScannerUtil.h"
#include "ExamSheetScannerCore.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************************************/
/**
 * 外部核心接口C文件
 *
 * 接口返回0表示失败
 *
 * readEssBitmapBuffer、readEssBitmap为简易识别接口，提供输出空间与图像数据即可识别成json
 *
 * 其余接口为高级接口，调用方法相对复杂，以下为调用过程
 *
 * 1.创建一个识别上下文
 *	 createEssContext 用于设置识别图像、识别参数和存放识别结果
 * 2.设置要识别的图像，两个可选，根据情况选用其一
 *   setEssBitmapBuffer
 *   setEssBitmap
 * 3.设置识别参数，在创建上下文时以下参数已经指定了默认值，以下接口根据情况选用，也可不用
 *   createEssBitmapForOutput
 *   setDifferRange
 *   setLuminanceSansitivity
 *   setDenstyThresholdVersion1
 *   setDenstyThresholdVersion2
 *   setReadWithVersion
 * 4.读取上下文，这一步之后，识别结果就在上下文里面了
 *   readContext
 * 5.访问上下文的识别结果，根据需要调用相应接口
 *   获取相关图像
 *   getSourceEssBitmap
 *   getBinaryEssBitmap
 *   getBalanceBinaryEssBitmap
 *   getOutputEssBitmap
 *   获取版本、提示号等
 *   getVersion
 *   getLastErrorCode
 *   getCertainty
 *   获取证号、答案等
 *   长度信息
 *   getNumberDigit
 *   getSubjectDigit
 *   getAnswerItems
 *   getAnswerCount
 *   内容指针
 *   getNumberBufferPtr
 *   getSubjectBufferPtr
 *   getAnswerBufferPtr
 *   getNumberCertaintiesBufferPtr
 *   getSubjectCertaintiesBufferPtr
 *   getAnswerCertaintiesBufferPtr
 *   复制内容
 *   copyNumberBuffer
 *   copySubjectBuffer
 *   copyAnswerBuffer
 *   copyNumberCertaintiesBuffer
 *   copySubjectCertaintiesBuffer
 *   copyAnswerCertaintiesBuffer
 */
/*******************************************************************************************************/

