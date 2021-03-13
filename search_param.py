import optuna
import os
import pwn
import sys

pwn.context.log_level = "error"

if len(sys.argv)>=2:
  prog = sys.argv[1]
else:
  prog = "./a.out"

def objective(trial):
  temp_start = trial.suggest_float("temp_start", 0.1, 0.01, log=True)
  temp_end = trial.suggest_float("temp_end", 1e-6, 1e-4, log=True)
  square = trial.suggest_int("square", 0, 1024)
  shrink = trial.suggest_int("shrink", 0, 1024)

  os.system(
    f"clang++ -std=c++17 -O2 -DNDEBUG -DLOCAL -DOPTUNA "+
    f"-DPARAM_TEMP_START={temp_start} "+
    f"-DPARAM_TEMP_END={temp_end} "+
    f"-DPARAM_SQUARE={square} "+
    f"-DPARAM_SHRINK={shrink} "+
    f"-o {prog} ./ahc001.cpp")

  s = pwn.process(prog)
  while True:
    l = s.recvline().decode()[:-1]
    if l.startswith("sum: "):
      return int(l[5:])

study = optuna.create_study(
  study_name="ahc001",
  storage="sqlite:///db.sqlite3",
  load_if_exists=True,
  direction="maximize")
study.optimize(objective, n_trials=25)
print(study.best_params)
print(study.best_value)
