#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>
#define Inf 1.0e30
#define maxItr 1000

using namespace std;

bool with_dual;

class LP {
	double ** M; // matrix of LP
	int m; // constraints #
	int n; // non-basic variables #
	double *_data;
	string *non_basis, *basis;

	void get_M_size(FILE* fp) {
		int c=' ';
		// read first line to get n
		while( isspace(c) ) c=fgetc(fp);
		n = 0;
	
		while(1) {
			while( !isspace(c) ) { // eat a number
				c = fgetc(fp);
			}
			++n;

			int end_of_line = 0;
			while( isspace(c) ) { // eat all blank space
				if(c=='\n') {
					end_of_line = 1;
					break;
				}
				c = fgetc(fp);
			}
			if(end_of_line) break;
		}

		c = fgetc(fp); //eat \n of first line
		// count the non-empty line followed

		m = 0;
		while(c!=EOF) {
			// find \n or any number
			while(isspace(c) && c!='\n') {
				if(c==EOF) return;
				c = fgetc(fp);
			}
			if(c!='\n') {
				++m;
				while( c!='\n') { // eat until \n
					if(c==EOF) return;
					c = fgetc(fp);
				}
			}
			c = fgetc(fp); // next line
		}
	}

	enum exit_type {
		OPTIMAL = 0,
		UNBOUNDED, 
		INFEASIBLE,
	};

public:
	// construct function
	LP() {
		M = NULL;
		_data = NULL;
	}

	int init(const char* fname) {
		FILE* fp = fopen(fname, "r");
		if(!fp) {
			printf("Failed to open input file\n");
			return -1;
		}
		get_M_size(fp);
		_data = new double[(m+1)*(n+2)]; // 1 column more space for aux problem
		M = new double*[m+1];
		for(int i=0; i<=m; i++) M[i] = &_data[i*(n+2)];

		fseek(fp, 0, SEEK_SET);

		M[0][0] = 0.0;
		for(int j=1; j<=n; j++) {
			fscanf(fp, "%lf ", &M[0][j]);
		}

		for(int i=0; i<m; i++) {
			for(int j=1; j<=n; j++) {
				fscanf(fp, "%lf ", &M[i+1][j]);
				M[i+1][j] *= -1;
			}
			fscanf(fp, "%lf ", &M[i+1][0]);
		}
		non_basis = new string[n+2];
		basis = new string[m+2];
		for(int i=1; i<=n; i++) non_basis[i-1] = string("x_")+std::to_string(i);
		for(int i=1; i<=m; i++) basis[i-1] = string("w_")+std::to_string(i);
		return 0;
	}

	void set_dictionary(double** M_, int m_, int n_, string* nbs, string* bs) {
		M = M_;
		m = m_;
		n = n_;
		non_basis = nbs;
		basis = bs;
	}

	void show_dictionary() {
		printf("zeta = %g", M[0][0]);
		for(int i=1; i<=n; i++) printf("+ %g %s", M[0][i], non_basis[i-1].c_str());
		printf("\n");
		for(int i=1; i<=m; i++) {
			printf(" %s = %g ", basis[i-1].c_str(), M[i][0]);
			for(int j=1; j<=n; j++) 
				printf("+ %g %s ", M[i][j], non_basis[j-1].c_str());
			printf("\n");
		}
	}

	void restore_objective_function(double *c) {
		// restore the objective function with c_1 x_1 + c_2 x_2 + ... + c_n x_n
		// the result is stored in first line of M
		for(int i=0; i<=n; i++) M[0][i] = 0.0;
		// if xi in basis
		for(int i=1; i<=m; i++) {
			if(basis[i-1][0] == 'x') {
				int cid;
				sscanf(basis[i-1].c_str(), "x_%d", &cid);
				for(int j=0; j<=n; j++)
					M[0][j] += c[cid-1]*M[i][j];
			}
		}
		// if xi in non_basis
		for(int i=1; i<=n; i++) {
			if(non_basis[i-1][0] == 'x') {
				int cid;
				sscanf(non_basis[i-1].c_str(), "x_%d", &cid);
				M[0][i] += c[cid-1];
			}
		}
	}

	int make_initial_feasible_with_aux_problem() {
		// bak the objective function first
		double* bakc = new double[n];
		for(int i=1; i<=n; i++) bakc[i-1] = M[0][i];

		for(int i=1; i<=n; i++) M[0][i] = 0.0;
		M[0][n+1] = -1.0;
		for(int i=1; i<=m; i++) M[i][n+1] = 1.0;
		// find smallest b<0 and set initial Q as -b_min
		double minV = 0;
		int minId = 0;
		for(int i=1; i<=m; i++) {
			if(M[i][0]<minV) {
				minV = M[i][0];
			    minId = i;
			}
		}

		non_basis[n] = string("Q");
		LP aux_lp; // these LP share the same underlie dictionary with *this
		aux_lp.set_dictionary(M, m, n+1,non_basis, basis);
		aux_lp.pivot(n+1, minId); // feasible now
		aux_lp.solve_from_feasible();
		for(int i=1; i<=m; i++) {
			if( basis[i-1] == string("Q") ) {
				return -1; // not feasible for original problem, as Q is not 0
			}
		}
		// remove Q and restore the objective function
		int Qid = 0;
		for(Qid=1; Qid<=n+1; Qid++) {
			if( non_basis[Qid-1]==string("Q") ) {
				break;
			}
		}
		for(int i=Qid; i+1<=n+1; i++) {
			non_basis[i-1] = non_basis[i];
			for(int j=1; j<=m; j++) M[j][i] = M[j][i+1];
		}

		restore_objective_function(bakc);

		delete []bakc;
		return 0;
	}

	int choose_enter_with_bland() { // here
		// smallest positive coefficient
		double minV = Inf;
		int minId = 0;
		for(int i=1; i<=n; i++) 
			if(M[0][i]>0 && M[0][i]<minV) {
				minV = M[0][i];
				minId = i;
			}

		if(minV>=Inf) return 0;
		return minId;
	}


	int choose_enter_dual() { // here
		// smallest coefficient, <0
		double minV = 1;
		int minId = 0;
		for(int i=1; i<=m; i++) 
			if(M[i][0]<minV) {
				minV = M[i][0];
				minId = i;
			}
		if(minV>0) return 0;
		return minId;
	}

	int choose_leave_dual(int i) {
		double minV = Inf;
		int minId = 0;
		for(int j=1; j<=n; j++) {
			if(M[i][j]>0) {
				double tmp = -M[i][0]/M[i][j];
				if(tmp<minV) {
					minV = tmp;
					minId = j;
				}
			}
		}
		return minId;
	}

	exit_type dual_simplex_with_feasible() {
		int itr = 0;
		while( itr < maxItr ) {
			itr++;
			int i = choose_enter_dual();
			if(i==0) { // optimal
				return OPTIMAL;
			}
			int j = choose_leave_dual(i);
			if(j==0) { // unbounded
				return UNBOUNDED;
			}
			pivot(j, i);
		}
		printf("Max iteration reached!\n");
	}
	
	int make_initial_feasible_with_dual_lp() {
		// bak the objective function first
		double* bakc = new double[n];
		for(int i=1; i<=n; i++) bakc[i-1] = M[0][i];
		for(int i=1; i<=n; i++) M[0][i] = 0.0;

		exit_type flag = dual_simplex_with_feasible();
		if(flag==UNBOUNDED) return -1; // dual unbounded, primal infeasible

		restore_objective_function(bakc);

		return 0;
	}

	int make_initial_feasible() {
		int isOk = true;
		for(int i=1; i<=m; i++) {
			if(M[i][0]<0) isOk = false;
		}
		if(isOk) return 0;
		
		if(with_dual) {
			// solve with primal dual problem
			return make_initial_feasible_with_dual_lp();
		} else {
			// solve with auxiliary problem
			return make_initial_feasible_with_aux_problem();
		}

		return 0;
	}

	int choose_enter() {
		// large coefficient
		double maxV = -1;
		int maxId = 0;
		for(int i=1; i<=n; i++) 
			if(M[0][i]>maxV) {
				maxV = M[0][i];
				maxId = i;
			}
		if(maxV<0) return 0;
		return maxId;
	}

	int choose_leave(int j) {
		double minV = Inf;
		int minId = 0;
		for(int i=1; i<=m; i++) {
			if(M[i][j]<0) {
				double tmp = -M[i][0]/M[i][j];
				if(tmp<minV) {
					minV = tmp;
					minId = i;
				}
			}
		}
		return minId;
	}

	void pivot(int enter, int leave) {
		swap(non_basis[enter-1], basis[leave-1]);
		// express non_basis as basis
		double tmpv = M[leave][enter];
		M[leave][enter] = 1.0/tmpv;
		for(int j=0; j<=n; j++) {
			if(j==enter) continue;
			M[leave][j] /= -tmpv;
		}

		// subs
		for(int j=0; j<=n; j++) {
			if(j==enter) continue;
			for(int i=0; i<=m; i++) {
				if(i==leave) continue;
				M[i][j] = M[i][j] + M[i][enter]*M[leave][j];
			}
		}

		// calc coefficient of leave
		for(int i=0; i<=m; i++) {
			if(i==leave) continue;
			M[i][enter] *= M[leave][enter];
		}

	}

	void show_result(exit_type et) {
		switch( et ) {
			case OPTIMAL:
				{
					printf("optimal\n");
					printf("%g\n", M[0][0]);
					vector<double> ans(n);
					for(int i=0; i<m; i++) {
						if(basis[i][0]=='x') {
							int id;
							sscanf(basis[i].c_str(), "x_%d", &id);
							ans[id-1] = M[i+1][0];
						}
					}	
					for(int i=0; i<n; i++)
						printf("%g ", ans[i]);
					printf("\n");
					return;
				}
			case UNBOUNDED:
				printf("unbounded\n");
				break;
			case INFEASIBLE:
				printf("Infeasible\n");
				break;
			default:
				break;
		}
	}

	exit_type solve_from_feasible() {
		int itr = 0;
		while( itr < maxItr ) {
			itr++;
			int j = choose_enter();
			if(j==0) { // optimal
				return OPTIMAL;
			}
			int i = choose_leave(j);
			if(i==0) { // unbounded
				return UNBOUNDED;
			}
			pivot(j, i);
		}
		//printf("Max iteration reached!\n");
		return solve_from_feasible_with_bland();
	}

	exit_type solve_from_feasible_with_bland() {
		while( 1 ) {
			int j = choose_enter_with_bland();
			if(j==0) { // optimal
				return OPTIMAL;
			}
			int i = choose_leave(j);
			if(i==0) { // unbounded
				return UNBOUNDED;
			}
			pivot(j, i);
		}
	}
	void solve() {
		if( make_initial_feasible() != 0) {
			show_result( INFEASIBLE );
			return ;
		}
		
#ifdef DEBUG
		printf("initial feasible dictionary\n");
		show_dictionary();
#endif

		exit_type flag = solve_from_feasible();
		show_result( flag );
	}
};

int main(int argc, char**argv) {
	with_dual = false;
	LP lp;
	if(argc==1) 
		lp.init("data.inp");
	else
		lp.init(argv[1]);

	if(argc==3 && strcmp(argv[2], "-Dual")==0) with_dual = true;
#ifdef DEBUG
	printf("initial dictionary\n");
	lp.show_dictionary();
#endif

	lp.solve();
#ifdef DEBUG
	lp.show_dictionary();
#endif
	return 0;
}

