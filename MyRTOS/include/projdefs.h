#ifndef __PROJ_DEFS_H_
#define __PROJ_DEFS_H_

typedef void (*TaskFunction_t)( void * );

#define pdFALSE      ( ( BaseType_t ) 0 ) 
#define pdTRUE      ( ( BaseType_t ) 1 ) 
  
#define pdPASS      ( pdTRUE ) 
#define pdFAIL      ( pdFALSE )


#endif
