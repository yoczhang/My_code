/* Parallel Hierarchical Grid -- an adaptive finite element library.
 *
 * Copyright (C) 2005-2010 State Key Laboratory of Scientific and
 * Engineering Computing, Chinese Academy of Sciences. */

/* This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA */

//写了一个给向量类型 VEC *myvector 赋值，最终是可以赋值成功了，并在最后的打印部分测试了申请的向量的大小
//好吧，在本本上测试的向量 v 大小为 v[2398]，在台式机上不行了，改为 v[2300] 没发生错误，哎～还是不要纠结这个了
#include "phg.h"

#if (PHG_VERSION_MAJOR <= 0 && PHG_VERSION_MINOR < 9)
# undef ELEMENT
typedef SIMPLEX ELEMENT;
typedef USE_MPI 1;
#endif
 
#include <string.h>
#include <math.h>
//#include <iostream>
//#include "other_functions.h"
//#include "Eigen/Dense"
//using namespace Eigen;
//using namespace phg;


int
main(int argc, char *argv[])
{
    INT periodicity = 0 /* X_MASK | Y_MASK | Z_MASK */;

    char *fn = "../mytest.mesh";
    GRID *g;

    //MPI_Init(&argc,&argv);
    //MPI_Comm comm;
    //phgInitSetComm(comm);
    phgInit(&argc, &argv);

    g = phgNewGrid(-1);
    phgSetPeriodicity(g, periodicity);

    if (!phgImport(g, fn, TRUE))
	phgError(1, "can't read file \"%s\".\n", fn);

    /*-----------------------------------------------------------------*/
    /*--------------- the following is my test ------------------------*/
//    int n=6;
    DOF *u;
    ELEMENT *e;
    MAP *map_u;
    DOF *u1, *u2;
    
    u=phgDofNew(g,DOF_P0,1,"u",DofNoAction);
    phgDofSetDataByValue(u,0.);
    
    map_u=phgMapCreate(u,NULL);

    /*----------------------test_0-----------------test_0-----------------------*/
    /*
     * this test is to printf the total dofs and elements
     */

    phgPrintf("total DOFs:%"dFMT"\n",DofGetDataCountGlobal(u));

    int n=0;
    ForAllElements(g,e){
        n++;
    }
    printf("total elements:n=%d \n",n);
    printf("\n");
    /*----------------------test_0----end of test----test_0---------------------*/


    MAT *U;
    U=phgMapCreateMat(map_u,map_u);
    

    /*----------------------test_1-----------------test_1-----------------------*/
    FLOAT mat_U[n][n];
    int i=0,j=0;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            mat_U[i][j]=1.0*i+6.0*j;
        }
    }
    mat_U[0][0]=1.0; 
    printf("\n/************ the value of mat_U  *************/\n");
    for(i=0;i<n;i++){
        printf(" | ");
        for(j=0;j<n;j++){
            printf("%f  ",mat_U[i][j]);
        }
        printf("|\n");
    }
    printf("/*********************************************/\n\n");
    /*----------------------test_1----end of test----test_1---------------------*/


    /*----------------------test_2-----------------test_2-----------------------*/
    /*
     * Assigned to the matrix MAT *U
     * Note that there we just only want to assigned to the matrix U sizing of 6*6,
     * so we take the DOF_P0 which has only 1 DOF in the ELEMENT *e, more the grid 
     * mytest.mesh just has 6 elements. Thus we has the following codes, this is not 
     * a universial code.
     */
    int I[n];
    i=0;
    ForAllElements(g,e){
        I[i]=phgMapE2L(map_u,0,e,0);//because there is only 1 DOF in the element, so the lastest parameter is 0
        i++;
    }// we get the map_u's local number(局部编码) by traversing(遍历) all of the elements, noticing map_u is 
     // associating tightly with MAT *U.

    printf("/*------------ printf I[i] ------------*/\n");
    for(i=0;i<n;i++){
        printf("I[%d]=%d, ",i,I[i]);
    }
    printf("\n/*--------- end of printf I[i] ---------*/\n");
    for(i=0;i<n;i++){
        phgMatAddEntries(U,1,I+i,n,I,mat_U[i]);
    }
    phgMatAssemble(U);
    /*----------------------test_2----end of test----test_2---------------------*/


    /*----------------------test_1-----------------test_1-----------------------*/
    printf("\n------test_1----------test_1-------\n");
    /* This will printf u_data=4.000000, via the printf value of mat_U, I think 
     * u_data is the first nonzero element of the first row in mat_U.
     */

    FLOAT *u_data=U->rows->data;
    int *u_cols=U->rows->cols;
    int u_ncols=U->rows->ncols; 

    for(i=0;i<n;i++){
        printf("u_data[%d]=%f, ",i,*(u_data+i));
    }
    printf("\n");

    for(i=0;i<n;i++){
        printf("u_cols[%d]=%d, ",i,*(u_cols+i));
    }
    printf("\n");

    printf("u_ncols=%d, \n",u_ncols);

    printf("-----------end of test_1------------\n ");

    /*----------------------test_1----end of test----test_1---------------------*/





    
    FLOAT rhs_f[n];
    for(i=0;i<n;i++){
        rhs_f[i]=90+6.0*i+i;
    }
    rhs_f[0]=91+1;

    //rhs_f[1]=288;
    VEC *x,*vec_f;
    x=phgMapCreateVec(map_u,1);
    vec_f=phgMapCreateVec(map_u,1);
    phgVecDisassemble(x);
    phgVecDisassemble(vec_f);
    phgVecAddEntries(vec_f,0,n,I,rhs_f);
    phgVecAssemble(vec_f);
   
    printf("test_2------------\n\n");

    SOLVER *sol;
    sol=phgSolverMat2Solver(SOLVER_GMRES,U);
    //sol->mat->handle_bdry_eqns=FALSE;

    printf("test_3------------\n\n");

    phgVecCopy(vec_f,&(sol->rhs));//copy vec_f to sol->rhs

    FLOAT *rhs_value=sol->rhs->data;// the follwing for(){...} is just a test code
    for(i=0;i<n;i++){
        printf("rhs[%d]=%f,  ",i,rhs_value[i]);
    }

    printf("\ntest_4---------------\n\n");

    phgSolverVecSolve(sol,FALSE,x);
    printf("test_5---------------\n");

    
    FLOAT *value=x->data;
    for(i=0;i<n;i++){
        printf("x[%d]=%f,  ",i,value[i]);
    }

    

    phgVecDestroy(&vec_f);
    phgVecDestroy(&x);
    phgMapDestroy(&map_u);
    phgDofFree(&u);
    phgSolverDestroy(&sol);



    /*--------------- end of my test ----------------------------------*/
    /*-----------------------------------------------------------------*/
    
    phgFreeGrid(&g);

    phgFinalize();

    return 0;
}
