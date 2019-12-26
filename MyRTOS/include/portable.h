#ifndef __PORTABLE_H_
#define __PORTABLE_H_

#include "portmacro.h"
#include "projdefs.h"

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode,
                                   void *pvParameters);

#endif /*__PORTABLE_H_*/

