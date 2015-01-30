/********************************************************************
* The following is a notice of limited availability of the code, and disclaimer
* which must be included in the prologue of the code and in all source listings
* of the code.
*
* Author:
*
* Jeff R. Hammond (jhammond@alcf.anl.gov)
* Argonne Leadership Computing Facility
*
* Permission is hereby granted to use, reproduce, prepare derivative works, and
* to redistribute to others.
*
*                 LICENSE
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
* - Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer listed
*    in this license in the documentation and/or other materials
*    provided with the distribution.
*
*  - Neither the name of the copyright holders nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* The copyright holders provide no reassurances that the source code
* provided does not infringe any patent, copyright, or any other
* intellectual property rights of third parties.  The copyright holders
* disclaim any liability to any recipient for claims brought against
* recipient by any third party for infringement of that parties
* intellectual property rights.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*********************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include <process.h>
#include <location.h>
#include <personality.h>

/**************************************************/

typedef struct
{
    int32_t TotalNodes;
    int32_t TotalProcs;
    int32_t NodeRank;
    int32_t ProcRank;
    int32_t Coords[6];
    int32_t PartitionSize[6];
    int32_t PartitionTorus[6];
    int32_t JobSize[6];
    int32_t JobTorus[6];
}
BGQ_Torus_t;

/**************************************************/

BGQ_Torus_t info;

/**************************************************/

void Q5D_Init(void)
{
    uint32_t rc;

    Personality_t pers;
    BG_JobCoords_t jobcoords;

    rc = Kernel_GetPersonality(&pers, sizeof(pers));
    assert(rc==0);

    rc = Kernel_JobCoords(&jobcoords);
    assert(rc==0);

    info.ProcRank = Kernel_GetRank();

    info.Coords[0] = pers.Network_Config.Acoord;
    info.Coords[1] = pers.Network_Config.Bcoord;
    info.Coords[2] = pers.Network_Config.Ccoord;
    info.Coords[3] = pers.Network_Config.Dcoord;
    info.Coords[4] = pers.Network_Config.Ecoord;
    info.Coords[5] = Kernel_MyTcoord();

    info.PartitionSize[0] = pers.Network_Config.Anodes;
    info.PartitionSize[1] = pers.Network_Config.Bnodes;
    info.PartitionSize[2] = pers.Network_Config.Cnodes;
    info.PartitionSize[3] = pers.Network_Config.Dnodes;
    info.PartitionSize[4] = pers.Network_Config.Enodes;
    info.PartitionSize[5] = Kernel_ProcessCount();

    /* shift rank back to 0 modulo the node then divide by procs per node to index by 1 */
    info.NodeRank = ( info.ProcRank - info.Coords[5] ) / info.PartitionSize[5];

    info.TotalNodes = info.PartitionSize[0] *
                      info.PartitionSize[1] *
                      info.PartitionSize[2] *
                      info.PartitionSize[3] *
                      info.PartitionSize[4];
    info.TotalProcs = info.PartitionSize[5] * info.TotalNodes;

    info.PartitionTorus[0] = ND_GET_TORUS(0,pers.Network_Config.NetFlags);
    info.PartitionTorus[1] = ND_GET_TORUS(1,pers.Network_Config.NetFlags);
    info.PartitionTorus[2] = ND_GET_TORUS(2,pers.Network_Config.NetFlags);
    info.PartitionTorus[3] = ND_GET_TORUS(3,pers.Network_Config.NetFlags);
    info.PartitionTorus[4] = ND_GET_TORUS(4,pers.Network_Config.NetFlags);
    info.PartitionTorus[5] = 0;

    info.JobSize[0] = jobcoords.shape.a;
    info.JobSize[1] = jobcoords.shape.b;
    info.JobSize[2] = jobcoords.shape.c;
    info.JobSize[3] = jobcoords.shape.d;
    info.JobSize[4] = jobcoords.shape.e;
    info.JobSize[5] = jobcoords.shape.core;

    info.JobTorus[0] = ND_GET_TORUS(0,pers.Network_Config.NetFlags) && jobcoords.shape.a==pers.Network_Config.Anodes;
    info.JobTorus[1] = ND_GET_TORUS(1,pers.Network_Config.NetFlags) && jobcoords.shape.b==pers.Network_Config.Bnodes;
    info.JobTorus[2] = ND_GET_TORUS(2,pers.Network_Config.NetFlags) && jobcoords.shape.c==pers.Network_Config.Cnodes;
    info.JobTorus[3] = ND_GET_TORUS(3,pers.Network_Config.NetFlags) && jobcoords.shape.d==pers.Network_Config.Dnodes;
    info.JobTorus[4] = ND_GET_TORUS(4,pers.Network_Config.NetFlags) && jobcoords.shape.e==pers.Network_Config.Enodes;
    info.JobTorus[5] = 0;

    return;
}

/* C implementation */

void Q5D_Torus_coords(int32_t coords[])
{
    coords[0] = info.Coords[0];
    coords[1] = info.Coords[1];
    coords[2] = info.Coords[2];
    coords[3] = info.Coords[3];
    coords[4] = info.Coords[4];
    coords[5] = info.Coords[5];
    return;
}

void Q5D_Partition_size(int32_t coords[])
{
    coords[0] = info.PartitionSize[0];
    coords[1] = info.PartitionSize[1];
    coords[2] = info.PartitionSize[2];
    coords[3] = info.PartitionSize[3];
    coords[4] = info.PartitionSize[4];
    coords[5] = info.PartitionSize[5];
    return;
}

void Q5D_Partition_isTorus(int32_t coords[])
{
    coords[0] = info.PartitionTorus[0];
    coords[1] = info.PartitionTorus[1];
    coords[2] = info.PartitionTorus[2];
    coords[3] = info.PartitionTorus[3];
    coords[4] = info.PartitionTorus[4];
    coords[5] = info.PartitionTorus[5];
    return;
}

void Q5D_Job_size(int32_t coords[])
{
    coords[0] = info.JobSize[0];
    coords[1] = info.JobSize[1];
    coords[2] = info.JobSize[2];
    coords[3] = info.JobSize[3];
    coords[4] = info.JobSize[4];
    coords[5] = info.JobSize[5];
    return;
}

void Q5D_Job_isTorus(int32_t coords[])
{
    coords[0] = info.JobTorus[0];
    coords[1] = info.JobTorus[1];
    coords[2] = info.JobTorus[2];
    coords[3] = info.JobTorus[3];
    coords[4] = info.JobTorus[4];
    coords[5] = info.JobTorus[5];
    return;
}

int32_t Q5D_Total_nodes(void)
{
    return info.TotalNodes;
}

int32_t Q5D_Total_procs(void)
{
    return info.TotalProcs;
}

int32_t Q5D_Node_rank(void)
{
    return info.NodeRank;
}

int32_t Q5D_Proc_rank(void)
{
    return info.ProcRank;
}

int32_t Q5D_Core_id(void)
{
    /* routine to return the BGQ core number (0-15) */
    return (int32_t) Kernel_ProcessorCoreID();
}

int32_t Q5D_Thread_id(void)
{
    /* routine to return the BGQ virtual core number (0-67) */
    return (int32_t) Kernel_ProcessorID();
}

#if defined(__cplusplus)
}
#endif
