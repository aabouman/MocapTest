from re import I
import osqp
import osqp.codegen as cg
import osqp.codegen.utils as cgutils
import numpy as np
import scipy as sp
from scipy import sparse
import os
import json
import datetime


def write_dense_array(f, mat, name, vectype):
    """
    Writes a dense matrix in column-major format
    """
    [m, n] = mat.shape
    f.write("const int {}_nrows = {};\n".format(name, m))
    f.write("const int {}_ncols = {};\n".format(name, n))
    f.write("{} {}[{}] = {{\n".format(vectype, name, mat.size))
    for j in range(0, n):
        for i in range(0, m):
            f.write("{:0.10f}, ".format(mat[i, j]))
        f.write("\n");
    f.write("};\n")

def write_dense_array_extern(f, mat, name, vectype):
  f.write("extern {} {}[{}];\n".format(vectype, name, mat.size))

def render_workspace(variables, hfname, cfname):
    rho_vectors = variables['rho_vectors']
    data = variables['data']
    linsys_solver = variables['linsys_solver']
    scaling = variables['scaling']
    settings = variables['settings']
    embedded_flag = variables['embedded_flag']
    n = data['n']
    m = data['m']

    # Open output file
    incFile = open(hfname, 'w')
    srcFile = open(cfname, 'w')

    # Add an include-guard statement
    fname = os.path.splitext(os.path.basename(hfname))[0]
    incGuard = fname.upper() + "_H"
    incFile.write("#ifndef %s\n" % incGuard)
    incFile.write("#define %s\n\n" % incGuard)

    # Print comment headers containing the generation time into the files
    now = datetime.datetime.now()
    daystr = now.strftime("%B %d, %Y")
    timestr = now.strftime("%H:%M:%S")
    incFile.write("/*\n")
    incFile.write(
        " * This file was autogenerated by EmbeddedMPC on %s at %s.\n" % (daystr, timestr))
    incFile.write(" * \n")
    incFile.write(
        " * This file contains the prototypes for all the workspace variables needed\n")
    incFile.write(
        " * by OSQP. The actual data is contained inside workspace.c.\n")
    incFile.write(
        " * The generating script was adapted from the one in OSQP-Python.")
    incFile.write(" */\n\n")

    srcFile.write("/*\n")
    srcFile.write(
        " * This file was autogenerated by EmbeddedMPC on %s at %s.\n" % (daystr, timestr))
    srcFile.write(" * \n")
    srcFile.write(
        " * This file contains the workspace variables needed by OSQP.\n")
    srcFile.write(
        " * The generating script was adapted from the one in OSQP-Python.")
    srcFile.write(" */\n\n")

    # Include types, constants and linsys_solver header
    incFile.write("#include \"osqp/types.h\"\n")
    incFile.write("#include \"osqp/qdldl_interface.h\"\n")

    srcFile.write("#include \"workspace.h\"\n")

    # Write data structure
    cgutils.write_data_src(srcFile, data)
    cgutils.write_data_inc(incFile, data)

    # Write settings structure
    cgutils.write_settings_src(srcFile, settings, embedded_flag)
    cgutils.write_settings_inc(incFile, settings, embedded_flag)

    # Write scaling structure
    cgutils.write_scaling_src(srcFile, scaling)
    cgutils.write_scaling_inc(incFile, scaling)

    # Write linsys_solver structure
    cgutils.write_linsys_solver_src(srcFile, linsys_solver, embedded_flag)
    cgutils.write_linsys_solver_inc(incFile, linsys_solver, embedded_flag)

    # Define empty solution structure
    cgutils.write_solution_src(srcFile, data)
    cgutils.write_solution_inc(incFile, data)

    # Define info structure
    cgutils.write_info_src(srcFile)
    cgutils.write_info_inc(incFile)

    # Define workspace structure
    cgutils.write_workspace_src(srcFile, n, m, rho_vectors, embedded_flag)
    cgutils.write_workspace_inc(incFile, n, m, rho_vectors, embedded_flag)

    # The endif for the include-guard
    incFile.write("#endif // ifndef %s\n" % incGuard)

    incFile.close()
    srcFile.close()


def render_problem_data(A, B, f, Q,q,R,r,Qf,qf,c, xe,ue,xg, N, target_dir):
    hfname = os.path.join(target_dir, 'problem_data.h')
    cfname = os.path.join(target_dir, 'problem_data.c')

    [nx, nu] = B.shape

    incFile = open(hfname, 'w')
    srcFile = open(cfname, 'w')

    # Header guard
    incFile.write("#pragma once\n")

    # Print comment headers containing the generation time into the files
    now = datetime.datetime.now()
    daystr = now.strftime("%B %d, %Y")
    timestr = now.strftime("%H:%M:%S")
    incFile.write("/*\n")
    incFile.write(
        " * This file was autogenerated by EmbeddedMPC on %s at %s.\n" % (daystr, timestr))
    incFile.write(" * \n")
    incFile.write(
        " * This file contains the data for the MPC problem\n")
    incFile.write(" */\n\n")

    srcFile.write("/*\n")
    srcFile.write(
        " * This file was autogenerated by EmbeddedMPC on %s at %s.\n" % (daystr, timestr))
    srcFile.write(" * \n")
    srcFile.write(
        " * This file contains the data for the MPC problem\n")
    srcFile.write(" */\n\n")

    # Include types, constants and linsys_solver header
    incFile.write("#include \"mpc_types.hpp\"\n\n")
    srcFile.write("#include \"problem_data.h\"\n")

    # Write constants to header file
    incFile.write("const int nstates = {};\n".format(nx))
    incFile.write("const int ninputs = {};\n".format(nu))
    incFile.write("const int nhorizon = {};\n".format(N+1))

    # Write dynamics matrices
    write_dense_array_extern(incFile, A, 'dynamics_Adata', 'c_float')
    write_dense_array(srcFile, A, 'dynamics_Adata', 'c_float')

    write_dense_array_extern(incFile, B, 'dynamics_Bdata', 'c_float')
    write_dense_array(srcFile, B, 'dynamics_Bdata', 'c_float')

    cgutils.write_vec_extern(incFile, f, 'dynamics_fdata', 'c_float')
    cgutils.write_vec(srcFile, f, 'dynamics_fdata', 'c_float')

    # Write cost matrices
    cgutils.write_vec_extern(incFile, Q.diagonal(), 'cost_Qdata', 'c_float')
    cgutils.write_vec(srcFile, Q.diagonal(), 'cost_Qdata', 'c_float')
    cgutils.write_vec_extern(incFile, q, 'cost_qdata', 'c_float')
    cgutils.write_vec(srcFile, q, 'cost_qdata', 'c_float')
    cgutils.write_vec_extern(incFile, R.diagonal(), 'cost_Rdata', 'c_float')
    cgutils.write_vec(srcFile, R.diagonal(), 'cost_Rdata', 'c_float')
    cgutils.write_vec_extern(incFile, r, 'cost_rdata', 'c_float')
    cgutils.write_vec(srcFile, r, 'cost_rdata', 'c_float')
    cgutils.write_vec_extern(incFile, Qf.diagonal(), 'cost_Qfdata', 'c_float')
    cgutils.write_vec(srcFile, Qf.diagonal(), 'cost_Qfdata', 'c_float')
    cgutils.write_vec_extern(incFile, qf, 'cost_qfdata', 'c_float')
    cgutils.write_vec(srcFile, qf, 'cost_qfdata', 'c_float')
    incFile.write("const c_float cost_c = {};\n".format(c))

    # Write vectors
    cgutils.write_vec_extern(incFile, xe, 'dynamics_xe', 'c_float')
    cgutils.write_vec(srcFile, xe, 'dynamics_xe', 'c_float')
    cgutils.write_vec_extern(incFile, ue, 'dynamics_ue', 'c_float')
    cgutils.write_vec(srcFile, ue, 'dynamics_ue', 'c_float')
    cgutils.write_vec_extern(incFile, xg, 'dynamics_xg', 'c_float')
    cgutils.write_vec(srcFile, xg, 'dynamics_xg', 'c_float')

    incFile.close()
    srcFile.close()


def codegen_workspace_files(prob: osqp.OSQP, target_dir):
    work = prob._model._get_workspace()
    embedded = 2
    python_ext_name = 'mpc_osqp'
    template_vars = {'rho_vectors':     work['rho_vectors'],
                     'data':            work['data'],
                     'settings':        work['settings'],
                     'linsys_solver':   work['linsys_solver'],
                     'scaling':         work['scaling'],
                     'embedded_flag':   embedded,
                     'python_ext_name': python_ext_name}

    render_workspace(template_vars,
                     os.path.join(target_dir, 'workspace.h'),
                     os.path.join(target_dir, 'workspace.c'))


# Read data from JSON file 
dirname = os.path.dirname(os.path.realpath(__file__))
f = open(os.path.join(dirname, "mpc_data.json"))
data = json.load(f)

# Dynamics
Ad = sparse.csc_matrix(data['A'])
Bd = sparse.csc_matrix(data['B'])
xe = np.array(data['xe'])
ue = np.array(data['ue'])
xg = np.array(data['xg'])
dx0 = np.array(data['dx0'])
dxg = np.array(data['dxg'])

# Objective function
Qk = sparse.diags(data['Qk'])
qk = np.array(data['qk'])
Rk = sparse.diags(data['Rk'])
rk = np.array(data['rk'])
Qf = sparse.diags(data['Qf'])
qf = np.array(data['qf'])
[nx, nu] = Bd.shape
fd = np.zeros(nx)
f.close() 

# Constraints
u0 = 10.5916
umin = np.array([9.6, 9.6, 9.6, 9.6]) - u0
umax = np.array([13., 13., 13., 13.]) - u0
xmin = np.array([-np.pi/6, -np.pi/6, -np.inf, -np.inf, -np.inf, -1.,
                 -np.inf, -np.inf, -np.inf, -np.inf, -np.inf, -np.inf])
xmax = np.array([np.pi/6, np.pi/6, np.inf, np.inf, np.inf, np.inf,
                 np.inf, np.inf, np.inf, np.inf, np.inf, np.inf])

# Prediction horizon
N = 10

# Cast MPC problem to a QP: x = (x(0),x(1),...,x(N),u(0),...,u(N-1))
# - quadratic objective
P = sparse.block_diag([sparse.kron(sparse.eye(N), Qk), Qf,
                       sparse.kron(sparse.eye(N), Rk)], format='csc')
# - linear objective
q = np.hstack([np.kron(np.ones(N), -Qk.dot(dxg) - qk), -Qf.dot(dxg) - qf,
               np.kron(np.ones(N), -rk)])
# - linear dynamics
Ax = sparse.kron(sparse.eye(N+1), -sparse.eye(nx)) + \
    sparse.kron(sparse.eye(N+1, k=-1), Ad)
Bu = sparse.kron(sparse.vstack([sparse.csc_matrix((1, N)), sparse.eye(N)]), Bd)
Aeq = sparse.hstack([Ax, Bu], format='csc')
leq = np.hstack([-dx0, np.zeros(N*nx)])
ueq = leq
# - input and state constraints
use_inequality_constraints = False 
if use_inequality_constraints:
    Aineq = sparse.eye((N+1)*nx + N*nu)
    lineq = np.hstack([np.kron(np.ones(N+1), xmin), np.kron(np.ones(N), umin)])
    uineq = np.hstack([np.kron(np.ones(N+1), xmax), np.kron(np.ones(N), umax)])

    A = sparse.vstack([Aeq, Aineq], format='csc')
    l = np.hstack([leq, lineq])
    u = np.hstack([ueq, uineq])
else:
    A = Aeq.copy()
    l = leq.copy()
    u = ueq.copy()


# Create an OSQP object
prob = osqp.OSQP()

# Setup workspace
prob.setup(P, q, A, l, u, warm_start=True, verbose=0)

# Generate C code
commondir = dirname
target_dir = commondir 
codegen_workspace_files(prob, target_dir)
c = 0.0
np.set_printoptions(edgeitems=30, linewidth=1000)
print(ue)
render_problem_data(Ad.toarray(), Bd.toarray(), fd, Qk,qk,Rk,rk,Qf,qf,c, xe,ue,xg, N, target_dir)

# target_dir_codegen = os.path.join(target_dir, "codegen")
# prob.codegen(target_dir_codegen, parameters='matrices', force_rewrite=True, LONG=False)

# Solve once
res = prob.solve()
u = res.x[-N*nu:-(N-1)*nu]
x0 = res.x[:nx]
print(x0)
print(u)