#if defined(__cplusplus)
extern "C" {
#endif

void Q5D_Init(void);

void Q5D_Torus_coords(int32_t coords[]);
void Q5D_Partition_size(int32_t coords[]);
void Q5D_Partition_isTorus(int32_t coords[]);
void Q5D_Job_size(int32_t coords[]);
void Q5D_Job_isTorus(int32_t coords[]);

int32_t Q5D_Total_nodes(void);
int32_t Q5D_Total_procs(void);
int32_t Q5D_Node_rank(void);
int32_t Q5D_Proc_rank(void);

int32_t Q5D_Core_id(void);
int32_t Q5D_Thread_id(void);

#if defined(__cplusplus)
}
#endif
