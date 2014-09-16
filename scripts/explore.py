from optparse import OptionParser
import subprocess
import itertools
import re
import datetime

def countConfigs(paramValues):
  n = 1
  for i in paramValues:
    n *= len(i)
  return n

def runConfig(config):
  cmd = "make AUTOTUNER_PPCG_FLAGS=\"--sizes '{kernel[i] -> tile[%s]; kernel[i] -> grid[%s]; kernel[i] -> block[%s]}' --target=opencl\" ppcg >> LOG.int && make build >> LOG.int && make run" % (config[0], config[1], config[2])
  print cmd
  if not options.dryrun:
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    m = re.search("(\d+.\d+)", str(stdout))
    print str(stdout)
    print str(stderr)
    print "----- Result: " + (m.group(1) if m != None else '??') + " seconds; config was " + str(config)

def readParamValues():
  f = open('explore-params.py', 'r')
  paramValues = eval(f.read())
  f.close()
  return paramValues

def getDate():
  return str(datetime.datetime.now())

def parseCommandLine():
  global options
  global args
  usage = 'Usage: %prog [options]'
  parser = OptionParser(usage=usage, description='Explore different configurations.')
  parser.add_option('-n', '--dry-run', dest='dryrun', action='store_true', help="don't actually run commands")
  (options, args) = parser.parse_args()

def main():
  parseCommandLine()
  paramValues = readParamValues()
  print 'Starting at ' + getDate()
  cnt = 0
  combs = itertools.product(*paramValues)
  print 'Parameter values to be explored: ' + str(paramValues)
  print 'Number of configurations: ' + str(countConfigs(paramValues))
  for i in combs:
    print '----- Configuration ' + str(cnt) + ': ' + str(i)
    runConfig(i)
    cnt += 1
  print 'Finished at ' + getDate()

if __name__ == "__main__":
  main()
