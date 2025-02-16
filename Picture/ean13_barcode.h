#ifndef _EAN13_BARCODE_H
#define _EAN13_BARCODE_H

#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****      内部调用       *****/


/*****      外部调用       *****/
void BarcodeAjust(vu8 *BarcodeImage); //倾斜校正
void BarcodeDenoise(vu8 *BarcodeImage); //条形码降噪处理，通常需要在倾斜校正前和校正后各调用一次


#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif /* _EAN13_BARCODE_H */
