#ifndef _EAN13_BARCODE_H
#define _EAN13_BARCODE_H

#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****      �ڲ�����       *****/


/*****      �ⲿ����       *****/
void BarcodeAjust(vu8 *BarcodeImage); //��бУ��
void BarcodeDenoise(vu8 *BarcodeImage); //�����뽵�봦��ͨ����Ҫ����бУ��ǰ��У���������һ��


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _EAN13_BARCODE_H */
