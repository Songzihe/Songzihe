## How to run my code

A `make` to compile the C++ code in `lp.cpp` will produce executive file `lp`. Type,

`./lp dataFile.inp [-Dual]`

to solve the linear program system configured in file of `dataFile.inp` . Then the solution is printed. The default dataFile is `data.inp` if no datafile is specified. If `-Dual` parameter is specified, the dual simplex method is used when the $x=0$ is infeasible.

Some data input file and `demo.sh` is attached. Run the `demo.sh` with show the output of examples.

## Introduction to the code

The total optimization problem, the dictionary, is configured with a matrix, a two dimensional array, in C++.

$$
\max c^T x \\
\begin{split}
s.t. & \\
& A x \le b \\ 
& x \ge 0\\
\end{split}
$$

$$
M = 
\begin{bmatrix}
\zeta & c_1 & c_2 & \cdots & c_n \\
b_1 & -a_{1,1} & -a_{1,2} & \cdots & -a_{1,n} \\
b_2 & -a_{2,1} & -a_{2,2} & \cdots & -a_{2,n} \\
\cdots & & & & \\
b_m & -a_{m,1} & -a_{m,2} & \cdots & -a_{m,n} \\
\end{bmatrix}
$$

The non-basic and basic variables are stored in two `string*`, `non_basis` and `basis`.

If initial LP is infeasible, auxiliary LP is introduced to obtain the initial feasible dictionary in function `make_initial_feasible_with_aux_problem`. Besides, Dual Simplex Method is used to solve initial infeasible problem with function `make_initial_feasible_with_dual_lp()`, which is listed in extra features.

The solver uses a performance oriented pivoting rule of the largest coefficient rule. But I Specified the  `maxItr`, which limit the maximum pivot implemented by the largest coefficient rule. If exceeded switch to Bland rule, which can avoid circle loop.



## Extra  Features

Primal-Dual Methods is used to solve initial infeasible for $x=0$ problem in my implementation.