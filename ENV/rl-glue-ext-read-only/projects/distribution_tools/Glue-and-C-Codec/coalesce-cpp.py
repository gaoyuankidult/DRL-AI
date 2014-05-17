import sys

theDir="wiki/current";
theFiles=['CandCPP.wiki.header','CandCPP.wiki.fromsource','RLGlueCore.wiki.macbinary','CandCPP.wiki.linux','CandCPP.wiki.debian32','CandCPP.wiki.debian64','CandCPP.wiki.rpm32','CandCPP.wiki.rpm64','CandCPP.wiki.footer'];

outfileName="wiki/current/CandCPP.wiki"
outfile = file(outfileName, 'w')

for whichFile in theFiles:
	f = file(theDir+'/'+whichFile)
	for line in f:
		outfile.writelines(line)
