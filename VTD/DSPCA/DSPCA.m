function [U,X,x,F,k,dualitygap,cputime,perceigs]=DSPCA(A,rho,gapchange,maxiter,info,algo)
% Wrapper function for sparse_rank_one_mex
% DSPCA finds a sparse rank-one approximation to 
% a given symmetric matrix A, by solving the SDP
% min_U lambda_max(A+X) : X = X', abs(X(i,j)) <= rho, 1<=i,j<= n
% and its dual:
% max_X Tr(UA) - rho sum_ij |U_ij| : U=U', U \succeq 0, Tr(U)=1
% ***	inputs: ***
% A                 nxn symmetric matrix (left unchanged)
% n                 problem size
% rho              non-negative scalar 
% gapchange required change in gap from first gap (default: 1e-4) 
% MaxIter		maximum number of iterations
% info             controls verbosity: 0 silent, n>0 frequency of progress report
% k0               0 if cold start, k0 if WarmStart (total number of iterations in previous run)
% F0               Average gradient (for warm start, Fmat is updated)
% X0               symmetric matrix that solves the above SDP 
 
% ***	outputs: ***
% U                 symmetric matrix that solves the above SDP 
% X                 dual variable, solves the dual SDP 
% x                 largest eigenvector of U 
% F                 Average gradient
% k                 number of iterations run
% dualitygap  vector of duality gaps at designated iterations
% cputime      vector of cumulative cpu times at designated iterations
% perceigs     vector of percentage of eigenvalues used (in partial
%                       eigenvalue decomposition) at designated iterations

% Refer to d’Aspremont, A., El Ghaoui, L., Jordan, M. & Lanckriet, G. R. G. (2005), 
% ‘A direct formulation for sparse PCA using semidefinite programming’, 
% Advances in Neural Information Processing Systems 17, 41–48.

% Compute sparsest pseudo eigenvector of A
if nargin < 5
    disp('Error: DSPCA must take at least 5 arguments');
    U=[];X=[];x=[];F=[];k=[];
    return;
elseif nargin == 5
    algo=1;
elseif nargin == 6
    % argument number is fine - nothing to do
elseif nargin == 9
    if isempty(algo)
        algo=1;
    end
    if isempty(U0) || isempty(F0) || isempty(k0)
        disp('Error: DSPCA needs values for U,F,and k0 for warm Startup');
        U=[];X=[];x=[];F=[];k=[];
        return;
    end
else
    disp('Error: DSPCA can take either 5,6, or 9 arguments');
    U=[];X=[];x=[];F=[];k=[];
    return;
end

% Test if A is 2 dimensional
if (length(size(A))~= 2)
    disp('Error: A is not 2 dimensional');
    U=[];X=[];x=[];F=[];k=[];
    return;
end
% Test if A is square
n=size(A,1);
m=size(A,2);
if (m~=n) 
    disp('Error: A is not square');
    U=[];X=[];x=[];F=[];k=[];
    return;
end

% Test if A is symmetric
checksymm = sum(sum(A-A'));
if (checksymm > 1e-10)
    disp('Error: A is not symmetric');
    U=[];X=[];x=[];F=[];k=[];
    return;
end

% check that variables make sense
if rho <= 0
    disp('Error: rho must be > 0');
    U=[];X=[];x=[];F=[];k=[];
    return;
end
if gapchange <= 0
    disp('Error: gapchange must be > 0');
    U=[];X=[];x=[];F=[];k=[];
    return;
end
if maxiter < 1
    disp('Error: maxiter must be >= 1');
    U=[];X=[];x=[];F=[];k=[];
    return;
end 
if info < 0
    disp('Error: info must be >= 0');
    U=[];X=[];x=[];F=[];k=[];
    return;
end
if (nargin == 6 || nargin == 9) && (algo~=1 && algo ~=3 && algo ~=5 && algo ~=7 && algo ~=9)
    disp('Error: the algorithm must be 1,3,5,7, or 9');
    U=[];X=[];x=[];F=[];k=[];
    return;
end
    
% Parameters that can be modified here and affect the matrix exponential computation using partial eigenvalue decomposition

% TODO: make addeigs a percentage of the dimension
numeigs=4;      % # of eigenvalues to take at a time in partial eig decomposition
addeigs=4;      % # of eigenvalues to add at a time when taking more than numeigs
checkgap=5;     % # of iterations between checking gap (can be more often than info)
propeigs=.10;   % percentage of eigenvalues partial eig decomposition will retrieve before reverting to full eig decomposition
check_for_more_eigs=10;     % # of iterations between checks for using more eigenvalues when only one eigenvalue currently is being used (checked in util.c)

[U,X,x,F,k,dualitygap,cputime,perceigs] = sparse_rank_one_mex(A,eye(size(A)),[1],rho,gapchange,maxiter,info,algo,numeigs,addeigs,checkgap,propeigs,check_for_more_eigs);

clear sparse_rank_one_mex;