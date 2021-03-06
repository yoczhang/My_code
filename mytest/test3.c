/*
 * This test2.c file is aimed to test 'QUDA *quad' in 1D stituation
 *
 */

#include "phg.h"

#if (PHG_VERSION_MAJOR <= 0 && PHG_VERSION_MINOR < 9)
# undef ELEMENT
typedef SIMPLEX ELEMENT;
typedef USE_MPI 1;
#endif
 
#include <string.h>
#include <math.h>

int 
main(int argc, char *argv[])
{
    
    char *fn = "../mytest.mesh";
    GRID *g;
    
    phgInit(&argc, &argv);

    g = phgNewGrid(-1);

    if (!phgImport(g, fn, TRUE))
	phgError(1, "can't read file \"%s\".\n", fn);

    /*-----------------------------------------------------------------*/
    /*--------------- the following is my test ------------------------*/
    INT i;
    QUAD *quad;
    quad=phgQuadGetQuad1D(1);
    printf("1D npoints=%d \n",quad->npoints);

    quad=phgQuadGetQuad3D(4);
    printf("3D npoints=%d \n",quad->npoints);

    for(i=0;i<5*quad->npoints;i++){
        printf("Gausspoints[%d]= %f \n",i,quad->points[i]);
    }

 return 0;
}//endof_main()
