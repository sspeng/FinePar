#include "spmv_util.h"
#define TOTALNUMber 5
int cpuoffset;

#include <pthread.h>
int rowInfo[11];
#define MAX 10

pthread_t thread[2];
pthread_mutex_t mut;
int number1=0;
int number2=0;
cl_int errorCode ;
    cl_mem devaold;
    cl_mem devanew;
    cl_mem devhold;
    cl_mem devhnew;
cl_kernel csrKernela;
cl_kernel csrKernelh;
cl_kernel csrKernela_cpu;
 cl_kernel csrKernelh_cpu;
    cl_command_queue cmdQueue ;
    cl_command_queue cmdQueue_cpu ;
cl_uint work_dim;
size_t globalsize[2] ;
size_t blocksize[2] ;
cl_uint work_dimcpu;
        size_t globalsizecpu;
        size_t localsizecpu;


void *thread1(void *)
{
  int i;
  //printf ("thread1 : I'm thread 1\n");

  for (i = 0; i < MAX; i++)
  {
    //printf("thread1 : number1 = %d\n",number1);
    pthread_mutex_lock(&mut);
    if(number2>number1+1){
     number1++;
              errorCode = clSetKernelArg(csrKernela, 3, sizeof(cl_mem), &devhold); CHECKERROR;
              errorCode = clSetKernelArg(csrKernela, 4, sizeof(cl_mem), &devanew); CHECKERROR;
        errorCode = clSetKernelArg(csrKernela, 8, sizeof(int), &number1); CHECKERROR;

              errorCode = clSetKernelArg(csrKernelh, 3, sizeof(cl_mem), &devaold); CHECKERROR;
              errorCode = clSetKernelArg(csrKernelh, 4, sizeof(cl_mem), &devhnew); CHECKERROR;
        errorCode = clSetKernelArg(csrKernelh, 8, sizeof(int), &number1); CHECKERROR;

              errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernela, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;

              errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernelh, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;







      //errorCode = clSetKernelArg(csrKernel, 9, sizeof(int), &number1); CHECKERROR;
      //errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernel, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
    pthread_mutex_unlock(&mut);
      clFinish(cmdQueue);
    }
    else{
      pthread_mutex_unlock(&mut);
      pthread_exit(NULL);
    }


 //   sleep(1);
  }


  //printf("thread1 :主函数在等我完成任务吗？\n");
  pthread_exit(NULL);
//return NULL;
}
void *thread2(void *)
{
  int i;
//  printf("thread2 : I'm thread 2\n");

  for (i = 0; i < MAX; i++)
  {
 //   printf("thread2 : number2 = %d\n",number2);
    pthread_mutex_lock(&mut);
    if(number2>number1+1){
      number2--;

      errorCode = clSetKernelArg(csrKernela_cpu, 3, sizeof(cl_mem), &devhold); CHECKERROR;
      errorCode = clSetKernelArg(csrKernela_cpu, 4, sizeof(cl_mem), &devanew); CHECKERROR;
      errorCode = clSetKernelArg(csrKernela_cpu, 8, sizeof(int), &number2); CHECKERROR;

      errorCode = clSetKernelArg(csrKernelh_cpu, 3, sizeof(cl_mem), &devaold); CHECKERROR;
      errorCode = clSetKernelArg(csrKernelh_cpu, 4, sizeof(cl_mem), &devhnew); CHECKERROR;
      errorCode = clSetKernelArg(csrKernelh_cpu, 8, sizeof(int), &number2); CHECKERROR;

      errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernela_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;
      errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernelh_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;






 //       errorCode = clSetKernelArg(csrKernelcpu, 11, sizeof(int), &number2); CHECKERROR;
//      errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernelcpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;
    pthread_mutex_unlock(&mut);
      clFinish(cmdQueue_cpu);
    }
    else{
      pthread_mutex_unlock(&mut);
      pthread_exit(NULL);
    }
//    sleep(1);
  }


  //printf("thread2 :主函数在等我完成任务吗？\n");
  pthread_exit(NULL);
//return NULL;
}
void thread_create(void)
{
  int temp;
    number1=0; number2=9;
    number1--;
    number2++;
  memset(&thread, 0, sizeof(thread));          //comment1
  /*创建线程*/
  if((temp = pthread_create(&thread[0], NULL, thread1, NULL)) != 0)  //comment2     
    printf("线程1创建失败!\n");
  //else
   // printf("线程1被创建\n");

  if((temp = pthread_create(&thread[1], NULL, thread2, NULL)) != 0)  //comment3
    printf("线程2创建失败");
//  else
 //   printf("线程2被创建\n");
}
void thread_wait(void)
{
        /*等待线程结束*/
        if(thread[0] !=0)
           {             //comment4    
                pthread_join(thread[0],NULL);
  //              printf("线程1已经结束\n");
          }   
        if(thread[1] !=0) 
           {   
                //comment5
               pthread_join(thread[1],NULL);
   //             printf("线程2已经结束\n");
         }   
}


    //spmv_csr_vector_ocl(&csrmat, &csrmatT, cooresa, cooresh, 0,  opttime1, optmethod1, clfilename, ntimes);//zf method1
void spmv_csr_vector_ocl(csr_matrix<int, float>* mat, csr_matrix<int, float>* matT, float* resa, float* resh, float* res, int padNum, double& opttime, int& optmethod, char* oclfilename, int ntimes)
//void spmv_csr_vector_ocl(csr_matrix<int, float>* mat, float* vec, float* result, int padNum, double& opttime, int& optmethod, char* oclfilename, float* coores, int ntimes)
{
    cl_device_id* devices = NULL;
    cl_context context = NULL;
    cmdQueue = NULL;
    cmdQueue_cpu = NULL;
//    cl_command_queue cmdQueue = NULL;
 //   cl_command_queue cmdQueue_cpu = NULL;


    cl_program program = NULL;

    assert(initialization2(devices, &context, &cmdQueue, &program, oclfilename, &cmdQueue_cpu) == 1);

    cl_int errorCode = CL_SUCCESS;

    //Create device memory objects
    cl_mem devRowPtr;
    cl_mem devColId;
    cl_mem devData;
    cl_mem devRowPtrT;
    cl_mem devColIdT;
    cl_mem devDataT;

//    cl_mem devaold;
 //   cl_mem devanew;
  //  cl_mem devhold;
//    cl_mem devhnew;

    /*
    cl_mem devVec;
    cl_mem devprnew;
    cl_mem devprold;
    */

    cl_mem devRowPtr_cpu;
    cl_mem devColId_cpu;
    cl_mem devData_cpu;
    cl_mem devRowPtrT_cpu;
    cl_mem devColIdT_cpu;
    cl_mem devDataT_cpu;

    //cl_mem devVec_cpu;


    //cl_mem devRes;

    //Initialize values
    int nnz = mat->matinfo.nnz;
    int vecsize = mat->matinfo.width;
    int rownum = mat->matinfo.height;
    int rowptrsize = rownum + 1;
    ALLOCATE_GPU_READ(devRowPtr, mat->csr_row_ptr, sizeof(int)*rowptrsize);
    ALLOCATE_GPU_READ(devColId, mat->csr_col_id, sizeof(int)*nnz);
    ALLOCATE_GPU_READ(devData, mat->csr_data, sizeof(float)*nnz);
    ALLOCATE_GPU_READ(devRowPtrT, matT->csr_row_ptr, sizeof(int)*rowptrsize);
    ALLOCATE_GPU_READ(devColIdT, matT->csr_col_id, sizeof(int)*nnz);
    ALLOCATE_GPU_READ(devDataT, matT->csr_data, sizeof(float)*nnz);


    /*
    devprold= clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR, sizeof(float)*vecsize, NULL, &errorCode); CHECKERROR;
    errorCode = clEnqueueWriteBuffer(cmdQueue, devprold, CL_TRUE, 0, sizeof(float)*rownum, vec, 0, NULL, NULL); CHECKERROR;
    devprnew= clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR, sizeof(float)*vecsize, NULL, &errorCode); CHECKERROR;
    errorCode = clEnqueueWriteBuffer(cmdQueue, devprnew, CL_TRUE, 0, sizeof(float)*rownum, vec, 0, NULL, NULL); CHECKERROR;
    */

    ALLOCATE_GPU_READ_cpu(devRowPtr_cpu, mat->csr_row_ptr, sizeof(int)*rowptrsize);
    ALLOCATE_GPU_READ_cpu(devColId_cpu, mat->csr_col_id, sizeof(int)*nnz);
    ALLOCATE_GPU_READ_cpu(devData_cpu, mat->csr_data, sizeof(float)*nnz);
    ALLOCATE_GPU_READ_cpu(devRowPtrT_cpu, matT->csr_row_ptr, sizeof(int)*rowptrsize);
    ALLOCATE_GPU_READ_cpu(devColIdT_cpu, matT->csr_col_id, sizeof(int)*nnz);
    ALLOCATE_GPU_READ_cpu(devDataT_cpu, matT->csr_data, sizeof(float)*nnz);

    ALLOCATE_GPU_READ_cpu(devaold, res, sizeof(float)*vecsize);
    ALLOCATE_GPU_READ_cpu(devanew, res, sizeof(float)*vecsize);
    ALLOCATE_GPU_READ_cpu(devhold, res, sizeof(float)*vecsize);
    ALLOCATE_GPU_READ_cpu(devhnew, res, sizeof(float)*vecsize);

   
    //int paddedres = findPaddedSize(rownum, 16);
    //devRes = clCreateBuffer(context, CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR, sizeof(float)*paddedres, NULL, &errorCode); CHECKERROR;
    errorCode = clEnqueueWriteBuffer(cmdQueue, devaold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
    errorCode = clEnqueueWriteBuffer(cmdQueue, devanew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
    errorCode = clEnqueueWriteBuffer(cmdQueue, devhold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
    errorCode = clEnqueueWriteBuffer(cmdQueue, devhnew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;




    opttime = 10000.0f;
    optmethod = 0;
    int dim2 = 1;


    {
	int methodid = 7;


       
//////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////



	//printf("\nRow Num %d padded size %d\n", rownum, padrowsize);
	work_dim = 1;
	//cl_uint work_dim = 1;
	blocksize[0] = CSR_VEC_GROUP_SIZE;
	blocksize[1] =  1;
	//size_t blocksize[] = {CSR_VEC_GROUP_SIZE, 1};
        //int threadsinrow=16;

        int rowsetzf=(double)cpuoffset/100*rownum;//zf
        printf("rowsetzf=%d\n",rowsetzf);


	csrKernela = NULL;
	//cl_kernel csrKernela = NULL;
	csrKernela = clCreateKernel(program, "gpu_csr_ve_slm_pm_fs", &errorCode); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 0, sizeof(cl_mem), &devRowPtrT); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 1, sizeof(cl_mem), &devColIdT); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 2, sizeof(cl_mem), &devDataT); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 3, sizeof(cl_mem), &devhold); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 4, sizeof(cl_mem), &devanew); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 5, sizeof(int), &rownum); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela, 6, sizeof(int), &rowsetzf); CHECKERROR;



        for(int i=0; i<11; i++){
          rowInfo[i]=(double)i/10*rownum;
        }
	cl_mem devrowInfo;
	ALLOCATE_GPU_READ(devrowInfo, rowInfo, sizeof(int)*(11));
        errorCode = clSetKernelArg(csrKernela, 7, sizeof(cl_mem), &devrowInfo); CHECKERROR;
        errorCode = clSetKernelArg(csrKernela, 8, sizeof(int), &number1); CHECKERROR;




	csrKernelh = NULL;
	//cl_kernel csrKernelh = NULL;
	csrKernelh = clCreateKernel(program, "gpu_csr_ve_slm_pm_fs", &errorCode); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 0, sizeof(cl_mem), &devRowPtr); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 1, sizeof(cl_mem), &devColId); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 2, sizeof(cl_mem), &devData); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 3, sizeof(cl_mem), &devaold); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 4, sizeof(cl_mem), &devhnew); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 5, sizeof(int), &rownum); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh, 6, sizeof(int), &rowsetzf); CHECKERROR;

        errorCode = clSetKernelArg(csrKernelh, 7, sizeof(cl_mem), &devrowInfo); CHECKERROR;
        errorCode = clSetKernelArg(csrKernelh, 8, sizeof(int), &number1); CHECKERROR;



	csrKernela_cpu = NULL;
	//cl_kernel csrKernela_cpu = NULL;
	csrKernela_cpu = clCreateKernel(program, "cpu_csr_ve_slm_pm_fs", &errorCode); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 0, sizeof(cl_mem), &devRowPtrT_cpu); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 1, sizeof(cl_mem), &devColIdT_cpu); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 2, sizeof(cl_mem), &devDataT_cpu); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 3, sizeof(cl_mem), &devhold); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 4, sizeof(cl_mem), &devanew); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 5, sizeof(int), &rownum); CHECKERROR;
	errorCode = clSetKernelArg(csrKernela_cpu, 6, sizeof(int), &rowsetzf); CHECKERROR;

        errorCode = clSetKernelArg(csrKernela_cpu, 7, sizeof(cl_mem), &devrowInfo); CHECKERROR;
        errorCode = clSetKernelArg(csrKernela_cpu, 8, sizeof(int), &number1); CHECKERROR;


	csrKernelh_cpu = NULL;
	//cl_kernel csrKernelh_cpu = NULL;
	csrKernelh_cpu = clCreateKernel(program, "cpu_csr_ve_slm_pm_fs", &errorCode); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 0, sizeof(cl_mem), &devRowPtr_cpu); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 1, sizeof(cl_mem), &devColId_cpu); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 2, sizeof(cl_mem), &devData_cpu); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 3, sizeof(cl_mem), &devaold); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 4, sizeof(cl_mem), &devhnew); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 5, sizeof(int), &rownum); CHECKERROR;
	errorCode = clSetKernelArg(csrKernelh_cpu, 6, sizeof(int), &rowsetzf); CHECKERROR;

        errorCode = clSetKernelArg(csrKernelh_cpu, 7, sizeof(cl_mem), &devrowInfo); CHECKERROR;
        errorCode = clSetKernelArg(csrKernelh_cpu, 8, sizeof(int), &number1); CHECKERROR;



	cl_kernel csrKernelcpu = NULL;
        globalsizecpu=3;
        localsizecpu=1;
        //size_t globalsizecpu=3;
        //size_t localsizecpu=1;

	int maxloopsize = CSR_VEC_MIN_TH_NUM;
	double minlooptime = 1000.0f;
        work_dimcpu=1;
        //cl_uint work_dimcpu=1;
	
	for (int groupnum = 24; groupnum <= 288; groupnum+= 24)
	{
	    globalsize[0] = groupnum*CSR_VEC_GROUP_SIZE;
	    globalsize[1] =  dim2;
	    //size_t globalsize[] = {groupnum*CSR_VEC_GROUP_SIZE, dim2};

            errorCode = clEnqueueWriteBuffer(cmdQueue, devaold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
            errorCode = clEnqueueWriteBuffer(cmdQueue, devanew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
            errorCode = clEnqueueWriteBuffer(cmdQueue, devhold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
            errorCode = clEnqueueWriteBuffer(cmdQueue, devhnew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;


            clFinish(cmdQueue);


            for(int order=0; order<TOTALNUMber; order++){
thread_create();
thread_wait();

/*
              errorCode = clSetKernelArg(csrKernela, 3, sizeof(cl_mem), &devhold); CHECKERROR;
              errorCode = clSetKernelArg(csrKernela, 4, sizeof(cl_mem), &devanew); CHECKERROR;

              errorCode = clSetKernelArg(csrKernelh, 3, sizeof(cl_mem), &devaold); CHECKERROR;
              errorCode = clSetKernelArg(csrKernelh, 4, sizeof(cl_mem), &devhnew); CHECKERROR;


              errorCode = clSetKernelArg(csrKernela_cpu, 3, sizeof(cl_mem), &devhold); CHECKERROR;
              errorCode = clSetKernelArg(csrKernela_cpu, 4, sizeof(cl_mem), &devanew); CHECKERROR;

              errorCode = clSetKernelArg(csrKernelh_cpu, 3, sizeof(cl_mem), &devaold); CHECKERROR;
              errorCode = clSetKernelArg(csrKernelh_cpu, 4, sizeof(cl_mem), &devhnew); CHECKERROR;



              errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernela, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernela_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;

              errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernelh, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernelh_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;
              clFinish(cmdQueue);
              clFinish(cmdQueue_cpu);
              */

              cl_mem ptmpt=devaold;
              devaold=devanew;
              devanew=ptmpt;

              ptmpt=devhold;
              devhold=devhnew;
              devhnew=ptmpt;


            }






	    float* tmpresulta = (float*)malloc(sizeof(float)*rownum);
	    float* tmpresulth = (float*)malloc(sizeof(float)*rownum);
	    errorCode = clEnqueueReadBuffer(cmdQueue, devhold, CL_TRUE, 0, sizeof(float)*rownum, tmpresulth, 0, NULL, NULL); CHECKERROR;
	    errorCode = clEnqueueReadBuffer(cmdQueue, devaold, CL_TRUE, 0, sizeof(float)*rownum, tmpresulta, 0, NULL, NULL); CHECKERROR;
	    //errorCode = clEnqueueReadBuffer(cmdQueue, devRes, CL_TRUE, 0, sizeof(float)*rownum, tmpresult, 0, NULL, NULL); CHECKERROR;
	    clFinish(cmdQueue);

            
            /*
        for(int i=0; i<rowforcpusum; i++){//zfadded
        //  printf("rowforcpu[%d]=%d\n",i,rowforcpu[i]);
          tmpresult[rowforcpu[i]]=coores[rowforcpu[i]];
        }
        */
        
        


	    two_vec_compare(resa, tmpresulta, rownum);
	    two_vec_compare(resh, tmpresulth, rownum);
	    free(tmpresulta);
	    free(tmpresulth);
            //exit(-1);
      /*  }
        }
        }

#if 0
    */
	    for (int k = 0; k < 2; k++)
	    {
              errorCode = clEnqueueWriteBuffer(cmdQueue, devaold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueWriteBuffer(cmdQueue, devanew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueWriteBuffer(cmdQueue, devhold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueWriteBuffer(cmdQueue, devhnew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;


              clFinish(cmdQueue);


              for(int order=0; order<TOTALNUMber; order++){
thread_create();
thread_wait();

/*
                errorCode = clSetKernelArg(csrKernela, 3, sizeof(cl_mem), &devhold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernela, 4, sizeof(cl_mem), &devanew); CHECKERROR;

                errorCode = clSetKernelArg(csrKernelh, 3, sizeof(cl_mem), &devaold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernelh, 4, sizeof(cl_mem), &devhnew); CHECKERROR;


                errorCode = clSetKernelArg(csrKernela_cpu, 3, sizeof(cl_mem), &devhold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernela_cpu, 4, sizeof(cl_mem), &devanew); CHECKERROR;

                errorCode = clSetKernelArg(csrKernelh_cpu, 3, sizeof(cl_mem), &devaold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernelh_cpu, 4, sizeof(cl_mem), &devhnew); CHECKERROR;



                errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernela, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
                errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernela_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;

                errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernelh, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
                errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernelh_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;
                clFinish(cmdQueue);
                clFinish(cmdQueue_cpu);
*/
                cl_mem ptmpt=devaold;
                devaold=devanew;
                devanew=ptmpt;

                ptmpt=devhold;
                devhold=devhnew;
                devhnew=ptmpt;


              }

	    }



            double time_in_sec=0;
	    for (int i = 0; i < ntimes; i++)
	    {
              errorCode = clEnqueueWriteBuffer(cmdQueue, devaold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueWriteBuffer(cmdQueue, devanew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueWriteBuffer(cmdQueue, devhold, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;
              errorCode = clEnqueueWriteBuffer(cmdQueue, devhnew, CL_TRUE, 0, sizeof(float)*vecsize, res, 0, NULL, NULL); CHECKERROR;


              clFinish(cmdQueue);


              double teststart = timestamp();
              for(int order=0; order<TOTALNUMber; order++){
thread_create();
thread_wait();

/*
                errorCode = clSetKernelArg(csrKernela, 3, sizeof(cl_mem), &devhold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernela, 4, sizeof(cl_mem), &devanew); CHECKERROR;

                errorCode = clSetKernelArg(csrKernelh, 3, sizeof(cl_mem), &devaold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernelh, 4, sizeof(cl_mem), &devhnew); CHECKERROR;


                errorCode = clSetKernelArg(csrKernela_cpu, 3, sizeof(cl_mem), &devhold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernela_cpu, 4, sizeof(cl_mem), &devanew); CHECKERROR;

                errorCode = clSetKernelArg(csrKernelh_cpu, 3, sizeof(cl_mem), &devaold); CHECKERROR;
                errorCode = clSetKernelArg(csrKernelh_cpu, 4, sizeof(cl_mem), &devhnew); CHECKERROR;



                errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernela, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
                errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernela_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;

                errorCode = clEnqueueNDRangeKernel(cmdQueue, csrKernelh, work_dim, NULL, globalsize, blocksize, 0, NULL, NULL); CHECKERROR;
                errorCode = clEnqueueNDRangeKernel(cmdQueue_cpu, csrKernelh_cpu, work_dimcpu, NULL, &globalsizecpu, &localsizecpu, 0, NULL, NULL); CHECKERROR;
                clFinish(cmdQueue);
                clFinish(cmdQueue_cpu);
*/
                cl_mem ptmpt=devaold;
                devaold=devanew;
                devanew=ptmpt;

                ptmpt=devhold;
                devhold=devhnew;
                devhnew=ptmpt;

              }

              double testend = timestamp();
              time_in_sec += (testend - teststart)/(double)dim2;

            }
            double gflops = (double)nnz*2*2*TOTALNUMber/(time_in_sec/(double)ntimes)/(double)1e9;
	    printf("\nCSR vector SLM row ptr groupnum:%d cpu time %lf ms GFLOPS %lf code %d \n\n", groupnum,   time_in_sec / (double) ntimes * 1000, gflops, methodid);

	    double onetime = time_in_sec / (double) ntimes;
	    if (onetime < opttime)
	    {
		opttime = onetime;
		optmethod = methodid;
	    }
	    if (onetime < minlooptime)
	    {
		minlooptime = onetime;
		maxloopsize = groupnum;
	    }
	}
	printf("******* Min time %f groupnum %d **********", minlooptime, maxloopsize);

	if (csrKernela)
	    clReleaseKernel(csrKernela);
	if (csrKernela_cpu)
	    clReleaseKernel(csrKernela_cpu);

	if (csrKernelh)
	    clReleaseKernel(csrKernelh);
	if (csrKernelh_cpu)
	    clReleaseKernel(csrKernelh_cpu);




        //free(bitmap);free(rowforcpu);//zf
//	clReleaseMemObject(devbitmap);//zf
    }

        //exit(-1);
    
    
   
    



    //Clean up
 //   if (image2dVec)
//	free(image2dVec);

    if (devRowPtr)
	clReleaseMemObject(devRowPtr);
    if (devColId)
	clReleaseMemObject(devColId);
    if (devData)
	clReleaseMemObject(devData);
    if (devaold)
	clReleaseMemObject(devaold);
    if (devhold)
	clReleaseMemObject(devhold);
    if (devanew)
	clReleaseMemObject(devanew);
    if (devhnew)
	clReleaseMemObject(devhnew);


    freeObjects(devices, &context, &cmdQueue, &program);


        return;
}


int main(int argc, char* argv[])
{
    if (argc < 2){
	printf("\nUsage: ./main *.mtx \n");
        return 1;
    }

    char* filename = argv[1];

    cpuoffset = 10;//atoi(argv[2]);
//    printf("cpuoffset=%d\n",cpuoffset);
    int ntimes = 5;

    coo_matrix<int, float> mat;
    init_coo_matrix(mat);
    ReadMMF(filename, &mat);

    if(mat.matinfo.width != mat.matinfo.height){
      printf("width != height\n");
      exit(-1);
    }
    printMatInfo(&mat);
    //////////////////////////////////////up/////////////////////////////////////////////////
    csr_matrix<int, float> csrmattmp;
    coo2csr<int, float>(&mat, &csrmattmp);
    float* tmp=(float*)malloc(sizeof(float)*mat.matinfo.width);
    for(int i=0; i<mat.matinfo.width; i++){
      tmp[i]=csrmattmp.csr_row_ptr[i+1]-csrmattmp.csr_row_ptr[i];
    }   
    for(int i=0; i<mat.matinfo.nnz; i++){
      mat.coo_data[i]=1.0/tmp[mat.coo_row_id[i]];
    }   
    free_csr_matrix(csrmattmp);
    free(tmp);


    csr_matrix<int, float> csrmat;
    coo2csr<int, float>(&mat, &csrmat);



   
    /*
    int maxr=0, maxc=0;
    for(int i=0; i<mat.matinfo.nnz; i++){
      if((mat.coo_row_id[i]<0)||(mat.coo_col_id[i]<0))
        printf("coo[%d][%d]=%f\n",mat.coo_row_id[i],mat.coo_col_id[i],mat.coo_data[i]);
      if(mat.coo_row_id[i]>maxr)
        maxr=mat.coo_row_id[i];
      if(mat.coo_col_id[i]>maxc)
        maxc=mat.coo_col_id[i];
    }
    printf("maxr=%d, maxc=%d\n",maxr,maxc);
    */
    

    
    for(int i=0; i<mat.matinfo.nnz; i++){
      int tmpt=mat.coo_row_id[i];
      mat.coo_row_id[i]=mat.coo_col_id[i];
      mat.coo_col_id[i]=tmpt;
    }
    

    /*
    if (!if_sorted_coo(&mat))
    {
	assert(sort_coo(&mat) == true);
    }
    if (!if_sorted_coo(&mat)){printf("zf not sorted\n");exit(-1);}//zf added
    for(int i=0; i<40; i++)
      printf("coo[%d][%d]=%f\n",mat.coo_row_id[i],mat.coo_col_id[i],mat.coo_data[i]);
    sort_coo<int, float>(&mat);//zf
    */

    csr_matrix<int, float> csrmatT;
    coo2csr<int, float>(&mat, &csrmatT);

//    printf("haahha\n");


    ////////////////////////////////////////down///////////////////////////////////////////////




    //char* clspmvpath = getenv("CLSPMVPATH");
    char clfilename[1000];
    
   {
	sprintf(clfilename, "%s%s", ".", "/spmv_csr_vector.cl");
	//sprintf(clfilename, "%s%s", "/home/pacman/zf/wubo/apu_corun/hits_dynamic", "/spmv_csr_vector.cl");





////////////////////////////////////////////////////////////////////////////////////////////////////////
    float* anew = (float*)malloc(sizeof(float)*mat.matinfo.width);
    float* aold = (float*)malloc(sizeof(float)*mat.matinfo.width);
    float* hnew = (float*)malloc(sizeof(float)*mat.matinfo.width);
    float* hold = (float*)malloc(sizeof(float)*mat.matinfo.width);



    //float* pr0 = (float*)malloc(sizeof(float)*mat.matinfo.width);
    float tmpt=1.0;
    //float tmpt=1.0/(float)mat.matinfo.width;
    printf("cpu start initialization\n");
    for(int i=0; i<mat.matinfo.width; i++){
      anew[i]=tmpt;
      aold[i]=tmpt;
      hnew[i]=tmpt;
      hold[i]=tmpt;
    }
    int height=mat.matinfo.width;


    //float d=0.85;
    //float e=0.99;
    //float distance=0;
    printf("cpu start computing\n");
    //while(distance < e){
    for(int order=0; order<TOTALNUMber; order++){
      for(int row=0; row<height; row++){
        int start = csrmatT.csr_row_ptr[row];
        int end = csrmatT.csr_row_ptr[row+1];
        float accumulant = 0;
        for(int j=start; j<end; j++){
          int col=csrmatT.csr_col_id[j];
          float data=csrmatT.csr_data[j];
          accumulant += data*hold[col];
        }
        anew[row]=accumulant;
      }

      for(int row=0; row<height; row++){
        int start = csrmat.csr_row_ptr[row];
        int end = csrmat.csr_row_ptr[row+1];
        float accumulant = 0;
        for(int j=start; j<end; j++){
          int col=csrmat.csr_col_id[j];
          float data=csrmat.csr_data[j];
          accumulant += data*aold[col];
        }
        hnew[row]=accumulant;
      }

      float *ptmpt=anew;
      anew=aold;
      aold=ptmpt;

      ptmpt=hnew;
      hnew=hold;
      hold=ptmpt;

    }


//    exit(0);
    float* cooresa = (float*)malloc(sizeof(float)*mat.matinfo.height);
    float* cooresh = (float*)malloc(sizeof(float)*mat.matinfo.height);
    float* res = (float*)malloc(sizeof(float)*mat.matinfo.height);
    for(int i=0; i<mat.matinfo.height; i++){
      cooresa[i]=aold[i];
      cooresh[i]=hold[i];
      res[i]=1;
    }





////////////////////////////////////////////////////////////////////////////////////////////////////////

    double opttime1 = 10000.0f;
    int optmethod1 = 0;

    spmv_csr_vector_ocl(&csrmat, &csrmatT, cooresa, cooresh, res, 0,  opttime1, optmethod1, clfilename, ntimes);//zf method1
    //spmv_csr_vector_ocl(&csrmat, pr0, res, 0,  opttime1, optmethod1, clfilename, coores, ntimes);//zf method1

	double opttime2 = 10000.0f;
	int optmethod2 = 0;

	csr_matrix<int, float> padcsr;
	csr_matrix<int, float> padcsrT;
	pad_csr(&csrmat, &padcsr, WARPSIZE / 2);
	pad_csr(&csrmatT, &padcsrT, WARPSIZE / 2);
	printf("\nNNZ Before %d After %d\n", csrmat.matinfo.nnz, padcsr.matinfo.nnz);
//in-2004    spmv_csr_vector_ocl(&padcsr, &padcsrT, cooresa, cooresh, res, 0,  opttime2, optmethod2, clfilename, ntimes);//zf method1
//	spmv_csr_vector_ocl(&padcsr, pr0, res, 16, opttime2, optmethod2, clfilename, coores, ntimes);
	free_csr_matrix(padcsr);

	int nnz = mat.matinfo.nnz;
	double gflops = (double)nnz*2*TOTALNUMber/opttime1/(double)1e9;
	printf("\n------------------------------------------------------------------------\n");
	printf("CSR VEC without padding best time %f ms best method %d gflops %f", opttime1*1000.0, optmethod1, gflops);
	printf("\n------------------------------------------------------------------------\n");
	gflops = (double)nnz*2*TOTALNUMber/opttime2/(double)1e9;
	printf("CSR VEC with padding best time %f ms best method %d gflops %f", opttime2*1000.0, optmethod2, gflops);
	printf("\n------------------------------------------------------------------------\n");

        double optfinal=opttime1;
        if(opttime2<optfinal)
          optfinal=opttime2;
        printf("CAUTTION: kernel time(ms): %f\n",optfinal*1000.0);
  

    //free(vec);
    //free(res);
    free_csr_matrix(csrmat);
    //free(coores);


    }

    free_coo_matrix(mat);

    return 0;
}

