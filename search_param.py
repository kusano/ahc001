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
  test_num = 100
  temp_start_50 = trial.suggest_float("temp_start_50", 1e-3, 1., log=True)
  temp_start_200 = trial.suggest_float("temp_start_200", 1e-3, 1., log=True)
  temp_end_50 = trial.suggest_float("temp_end_50", 1e-7, 1e-4, log=True)
  temp_end_200 = trial.suggest_float("temp_end_200", 1e-7, 1e-4, log=True)
  square_50 = trial.suggest_int("square_50", 0, 0x10000)
  square_200 = trial.suggest_int("square_200", 0, 0x10000)
  shrink_50 = trial.suggest_int("shrink_50", 0, 0x10000)
  shrink_200 = trial.suggest_int("shrink_200", 0, 0x10000)
  vanish_50 = trial.suggest_int("vanish_50", 0, 0x10000)
  vanish_200 = trial.suggest_int("vanish_200", 0, 0x10000)

  os.system(
    f"clang++ -std=c++17 -O2 -DNDEBUG -DLOCAL " +
    f"-DPARAM_TEST_NUM={test_num} "+
    f"-DPARAM_TEMP_START_50={temp_start_50} "+
    f"-DPARAM_TEMP_START_200={temp_start_200} "+
    f"-DPARAM_TEMP_END_50={temp_end_50} "+
    f"-DPARAM_TEMP_END_200={temp_end_200} "+
    f"-DPARAM_SQUARE_50={square_50} "+
    f"-DPARAM_SQUARE_200={square_200} "+
    f"-DPARAM_SHRINK_50={shrink_50} "+
    f"-DPARAM_SHRINK_200={shrink_200} "+
    f"-DPARAM_VANISH_50={vanish_50} "+
    f"-DPARAM_VANISH_200={vanish_200} "+
    f"-o {prog} ./ahc001.cpp")

  s = pwn.process(prog)
  s.readuntil("sum: ")
  score = float(s.recvline().decode()[:-1])/test_num*50
  s.close()
  return score

study = optuna.create_study(
  study_name="ahc001_9",
  storage="sqlite:///db.sqlite3",
  load_if_exists=True,
  direction="maximize")
study.optimize(objective, n_trials=100)
print(study.best_params)
print(study.best_value)
