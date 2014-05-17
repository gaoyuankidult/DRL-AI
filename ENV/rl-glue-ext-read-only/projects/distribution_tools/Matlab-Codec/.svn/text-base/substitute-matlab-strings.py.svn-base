import sys


theVersion=sys.argv[1]
uploadedFileBase=sys.argv[2]
uploadedFileName=uploadedFileBase+".tar.gz"
theLink="http://rl-glue-ext.googlecode.com/files/"+uploadedFileName
detailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+uploadedFileName

fileName="Matlab.wiki.template"
outfileName="Matlab.wiki"

subs={}
subs['MATLAB-USER-DEV-VERSION']=theVersion
subs['MATLAB-USER-DEV-LINK']=theLink
subs['MATLAB-USER-DEV-DETAILS-LINK']=detailsLink
subs['MATLAB-USER-DEV-FILE-BASE']=uploadedFileBase
f = file(fileName)
newlines = []
for line in f:
	for key,value in subs.iteritems():
		if key in line:
			line=line.replace(key,value)
	newlines.append(line)

outfile = file(outfileName, 'w')
outfile.writelines(newlines)