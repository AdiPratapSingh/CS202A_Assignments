import argparse
import csv
from pysat.card import CardEnc, EncType
from pysat.solvers import Solver


def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description='Solves sudoku pair puzzles.')
    parser.add_argument('-k', '--k', type=int, default=2, help='k value for k-sudoku')
    parser.add_argument('-p', '--path', type=str, default="TestCases/test_case1.csv",
                        help='Path to sudoku puzzle csv file.')
    return parser.parse_args()


def read_csv(path):
    """Reads and returns data from csv file from line s to e"""
    rows = []
    with open(path, 'r') as file:
        csvreader = csv.reader(file)
        for row in csvreader:
            rows.append(row)
    return rows


def add_value_constraints(k, solver, start = 0):
    """Adds constraints that each box should have exactly one value"""
    for i in range(k*k):
        for j in range(k*k):
            box_lits = []
            for m in range(1, k*k+1):
                box_lits.append(start + i*(k**4) + j*(k**2) + m)
            cnf = CardEnc.equals(lits=box_lits, bound=1, encoding=EncType.pairwise)
            solver.append_formula(cnf)


def add_horl_and_vert_constraints(k, solver, start=0):
    """Adds horizontal and vertical constraints"""
    for i in range(k*k):
        for m in range(1,k*k+1):
            hor_lits, ver_lits = [], []
            for j in range(k*k):
                hor_lits.append(start + i*(k**4) + j*(k**2) + m)
                ver_lits.append(start + j*(k**4) + i*(k**2) + m)

            cnf1 = CardEnc.equals(lits=hor_lits, bound=1, encoding=EncType.pairwise)
            cnf2 = CardEnc.equals(lits=ver_lits, bound=1, encoding=EncType.pairwise)
            solver.append_formula(cnf1)
            solver.append_formula(cnf2)


def add_block_constraints(k, solver, start=0):
    """Adds block constraints"""
    for I in range(k):
        for J in range(k):
            for m in range(1,k*k+1):
                block_lits = []
                for i in range(k):
                    for j in range(k):
                        block_lits.append(start + (I*k+i)*(k**4) + (J*k+j)*(k**2) + m)

                cnf = CardEnc.equals(lits=block_lits, bound=1, encoding=EncType.pairwise)
                solver.append_formula(cnf)


def get_assumptions(k, rows, soln, start=0):
    """Fix the positions that have given values"""
    assumptions = []
    for i in range(k*k):
        for j in range(k*k):
            if rows[i][j] != '0':
                assumptions.append(start + i*(k**4) + j*(k**2) + int(rows[i][j]))
                soln[i][j] = int(rows[i][j])
    return assumptions


def print_soln(k, soln):
    """Prints the final solution"""
    for i in range(k*k):
        for j in range(k*k):
            print(soln[i][j], end=" ")
            if (j+1) % k == 0 and j != k*k-1:
                print("|", end=" ")
        print()
        if (i+1) % k == 0 and i != k*k-1:
            for j in range(2*k*k+2*k-3):
                print('-', end="")
            print()



args = parse_args()
k = args.k
solver = Solver()

# list to store solved sudoku - start with 0s
soln = [[0 for _ in range(k*k)] for _ in range(k*k)]

# add constraints
add_value_constraints(k, solver)
add_horl_and_vert_constraints(k, solver)
add_block_constraints(k, solver)

# get assumptions by reading from given file
rows = read_csv(args.path)
assumptions = get_assumptions(k, rows, soln)

# solve the encoded problem
if solver.solve(assumptions=assumptions):
    print("Solved")
    # get the value of each box
    for val in solver.get_model():
        if val > 0:
            i = int((val-1) / (k**4))
            j = int((val-1) % (k**4) / (k**2))
            m = int((val-1) % (k**4) % (k**2) + 1)
            soln[i][j] = m
    print_soln(k, soln)
else:
    print("No solution exists")

# delete the solver after use
solver.delete()