import sys
from Processing.LogParser import split_log

split_log(sys.argv[1], sys.argv[2] if len(sys.argv)>2 else '_');