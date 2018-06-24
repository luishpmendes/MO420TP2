import glob
import os
BNC_EXEC = "../RA117801-RA134796/CODIGO/bnc"



def getParams(folder):
  f = open("%s/params"%(folder), "r")
  paramsList = f.readline().split()
  
  params = {}
  params["typeExec"] = paramsList[0]
  params["model"] = paramsList[1]
  params["timeLimit"] = paramsList[2]
  params["heur"]  = paramsList[3]
  params["oci"] = paramsList[4]
  params["preproc"] = paramsList[5]
  params["info"] = "y"
  f.close()
  return params

def clearFolder(folder):
  os.system("rm %s/*.info %s/*.est %s/*.sol"%(folder, folder, folder))

def runInstancesFolder(folder, params):
  clearFolder(folder)
  for inst in sorted(glob.glob("%s/*.gcc"%(folder))):
    print(inst)
    if os.system("%s %s %s %s %s %s %s %s %s"%(BNC_EXEC,
                   params["typeExec"], params["model"], params["timeLimit"],
                   params["heur"], inst, params["oci"], params["preproc"],
                   params["info"])):
      return False
  return True

def runExperiment(folder):
  params = getParams(folder)
  if not runInstancesFolder(folder+"/type1", params):
    return False
  if not runInstancesFolder(folder+"/type2", params):
    return False
  return True

def main():
  exp_list = ["test"]
  for exp in exp_list:
    if not runExperiment(exp):
      print("aborted")
      break

if __name__ == "__main__":
  main()
