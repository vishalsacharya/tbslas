#*************************************************************************
#Copyright (C) 2015 by Arash Bakhtiari
#You may not use this file except in compliance with the License.
#You obtain a copy of the License in the LICENSE file.

#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#*************************************************************************
import os
import subprocess
import math
import sys
from collections import OrderedDict
import json
import utils

def test1():
    ############################################################################
    # TEST 1: WEAK/STRONG SCALING FOR ADVECTION
    ############################################################################
    prog  = 'advection'
    tl_list = [
            # 1e+0,
            1e-2,
            1e-4,
            1e-7,
            ]
    dp_list = [
            # 6,
            # 8,
            # 10,
            15,
        ]
    cq_list = [
            4,
            6,
            14,
            ]
    np_list = [
            1,
            2,
            4,
            8,
            16,
            32,
            ]

    # dt = 7.85398e-03
    # tn = 100

    dt = 0.0628
    tn = 200

    use_cubic     = True
    vtk_save_rate = 0
    merge_type    = 3

    max_np        = max(np_list)
    num_pnts      = 8**(math.floor(math.log(max_np,8)+1))
    uf            = 2

    table_counter = 0
    for cq in cq_list:
        for tl in tl_list:
            if cq is 4 and tl is 1e-7:
                continue
            for dp in dp_list:
                # USE UF 4 FOR Q 14
                if cq is 14:
                    uf = 4
                cmd_args = OrderedDict()
                cmd_id = 0
                for np in np_list:
                    cmd_args[cmd_id] = utils.generate_commands(
                        prog    = prog,
                        pn_list = [num_pnts        ],
                        tl_list = [tl              ],
                        dp_list = [dp              ],
                        cq_list = [cq              ],
                        ci_list = [use_cubic       ],
                        uf_list = [uf              ],
                        np_list = [np              ],
                        nt_list = [omp_num_threads ],
                        dt_list = [dt              ],
                        tn_list = [tn              ],
                        vs_list = [vtk_save_rate   ],
                        mg_list = [merge_type      ]
                        )[1]
                    cmd_id = cmd_id + 1
                utils.execute_commands(cmd_args, prog+'-table-'+str(table_counter))
                table_counter = table_counter + 1

def test2():
    ############################################################################
    # TEST 2: CONVERGENCE TEST FOR ADVECTION
    ############################################################################
    mpi_num_procs, omp_num_threads = utils.parse_args()
    prog          = 'advection'
    dt            = 0.0628
    vtk_save_rate = 10
    mrg_type      = 3
    np            = mpi_num_procs
    num_pnts      = 8**(math.floor(math.log(np,8)+1))
    nt            = omp_num_threads

    dp_list = [5    , 5     , 6    , 6     , 7    , 7    ]
    cq_list = [4    , 4     , 4    , 4     , 4    , 4    ]
    ci_list = [True , False , True , False , True , False]
    uf_list = [2    , 2     , 2    , 2     , 2    , 2    ]
    dt_list = [dt   , dt    , dt/2 , dt/2  , dt/4 , dt/4 ]
    tn_list = [100  , 100   , 200  , 200   , 400  , 400  ]

    num_steps = len(dp_list)
    pn_list = [num_pnts      for cnt in range(0,num_steps)]
    tl_list = [1e-30         for cnt in range(0,num_steps)]
    np_list = [np            for cnt in range(0,num_steps)]
    nt_list = [nt            for cnt in range(0,num_steps)]
    mg_list = [mrg_type      for cnt in range(0,num_steps)]
    vs_list = [vtk_save_rate for cnt in range(0,num_steps)]

    cmd_args = OrderedDict()
    cmd_args = utils.generate_commands(
        prog,
        pn_list,
        tl_list,
        dp_list,
        cq_list,
        ci_list,
        uf_list,
        np_list,
        nt_list,
        dt_list,
        tn_list,
        vs_list,
        mg_list)
    utils.execute_commands(cmd_args, prog+'-table-'+str(0))

def test3():
    ############################################################################
    # TEST 2: CONVERGENCE TEST FOR ADVECTION
    ############################################################################
    mpi_num_procs, omp_num_threads = utils.parse_args()
    prog          = 'advection-inverse'
    tfinal        = 1.0
    dt            = tfinal/100
    vtk_save_rate = 10
    mrg_type      = 3
    np            = mpi_num_procs
    num_pnts      = 8**(math.floor(math.log(np,8)+1))
    nt            = omp_num_threads

    dp_list = [6    , 6     , 7    , 7     , 8    , 8    ]
    cq_list = [4    , 4     , 4    , 4     , 4    , 4    ]
    ci_list = [True , False , True , False , True , False]
    uf_list = [2    , 2     , 2    , 2     , 2    , 2    ]
    dt_list = [dt   , dt    , dt/2 , dt/2  , dt/4 , dt/4 ]
    tn_list = [100  , 100   , 200  , 200   , 400  , 400  ]

    num_steps = len(dp_list)
    pn_list = [num_pnts      for cnt in range(0,num_steps)]
    tl_list = [1e-30         for cnt in range(0,num_steps)]
    np_list = [np            for cnt in range(0,num_steps)]
    nt_list = [nt            for cnt in range(0,num_steps)]
    mg_list = [mrg_type      for cnt in range(0,num_steps)]
    vs_list = [vtk_save_rate for cnt in range(0,num_steps)]

    cmd_args = OrderedDict()
    cmd_args = utils.generate_commands(
        prog,
        pn_list,
        tl_list,
        dp_list,
        cq_list,
        ci_list,
        uf_list,
        np_list,
        nt_list,
        dt_list,
        tn_list,
        vs_list,
        mg_list)
    utils.execute_commands(cmd_args, prog+'-table-'+str(0))

################################################################################
# MAIN
################################################################################
if __name__ == '__main__':
    mpi_num_procs, omp_num_threads = utils.parse_args()
    # test1()
    # test2()
    test3()
