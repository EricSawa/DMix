/*
 * eal_task.h
 *
 *  Created on: 26.01.2019
 *      Author: erics
 */

#ifndef EAL_TASK_H_
#define EAL_TASK_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "msg.h"
/******************************************************************************
* Constants
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct EAL_TASK_TASK{
	bool enable;
	uint16_t id;
	void (*init) (struct EAL_TASK_TASK *self);
	void (*mainLoop) (struct EAL_TASK_TASK *self);
	void (*timer) (struct EAL_TASK_TASK *self, bool stat);
	void (*receiveMsg) (struct EAL_TASK_TASK *self, msg_Message *message);
	void (*sendMsg) (struct EAL_TASK_TASK *self, msg_Message *message);
}eal_task_Task;
/******************************************************************************
* Callback's
*******************************************************************************/
/******************************************************************************
* Function Prototypes
*******************************************************************************/




#endif /* EAL_TASK_H_ */
