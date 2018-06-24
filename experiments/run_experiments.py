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
  params["info"] = paramsList[4]
  params["oci"] = paramsList[5]
  f.close()
  return params

def clearFolder(folder):
  os.system("rm -f %s/*.info %s/*.est %s/*.sol"%(folder, folder, folder))

def runInstancesFolder(folder, params):
  clearFolder(folder)
  for inst in sorted(glob.glob("%s/*.gcc"%(folder))):
    print(inst)
    if os.system("%s %s %s %s %s %s %s %s"%(BNC_EXEC,
                   params["typeExec"], params["model"], params["timeLimit"],
                   params["heur"], inst, params["info"], params["oci"])):
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
  exp_list = ["bnc_clique_oci", "bnc_clique_oci_heur",  "bnc_clique_semoci",
  "bnc_clique_semoci_heur",  "bnc_pair_oci",  "bnc_pair_semoci"]
  for exp in exp_list:
    if not runExperiment(exp):
      print("aborted")
      break

if __name__ == "__main__":
  main()
