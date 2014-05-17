import sys

theDir="wiki/current";
theFiles=['Python.wiki.header','Python.wiki.fromsource.user','Python.wiki.windows','Python.wiki.fromsource.dev','Python.wiki.footer'];

outfileName="wiki/current/Python.wiki"
outfile = file(outfileName, 'w')

for whichFile in theFiles:
	f = file(theDir+'/'+whichFile)
	for line in f:
		outfile.writelines(line)
