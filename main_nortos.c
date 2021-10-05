/* must notice
 * this project is linked to the HardLink project in github
 * */

/*
 *  ======== main_nortos.c ========
 */
#include <stdint.h>
#include <stddef.h>

#include <NoRTOS.h>
#include <HardLink.h>

/* Example/Board Header files */
#include "Board.h"

#include <stdio.h>
extern void *mainThread(void *arg0);

/*
 *  ======== main ========
 */
int main(void)
{
    char a[]="Test";
    /* Call driver init functions */
    Board_initGeneral();

    /* Start NoRTOS */
    NoRTOS_start();

    /* Call mainThread function */
    //mainThread(NULL);
    HardLink_init();
    HardLink_setFrequency(915000000);
    while(1){
        HardLink_send(a,1);
        sleep(1);
    }

    while (1);
}
