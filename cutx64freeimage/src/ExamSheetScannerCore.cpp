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

