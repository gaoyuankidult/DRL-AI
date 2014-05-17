import sys


theVersion=sys.argv[1]
uploadedDevFileBase=sys.argv[2]
uploadedUserFileBase=sys.argv[3]
uploadedDevFileName=uploadedDevFileBase+".tar.gz"
uploadedUserFileName=uploadedUserFileBase+".tar.gz"

userLink="http://rl-glue-ext.googlecode.com/files/"+uploadedUserFileName
devLink="http://rl-glue-ext.googlecode.com/files/"+uploadedDevFileName
userDetailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+uploadedUserFileName
devDetailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+uploadedDevFileName

fileName="Python.wiki.template"
outfileName="Python.wiki"

subs={}
subs['PYTHON-USER-VERSION']=theVersion
subs['PYTHON-USER-LINK']=userLink
subs['PYTHON-USER-DETAILS-LINK']=userDetailsLink
subs['PYTHON-USER-FILE-BASE']=uploadedUserFileBase
subs['PYTHON-DEV-VERSION']=theVersion
subs['PYTHON-DEV-LINK']=devLink
subs['PYTHON-DEV-DETAILS-LINK']=devDetailsLink
subs['PYTHON-DEV-FILE-BASE']=uploadedDevFileBase
f = file(fileName)
newlines = []
for line in f:
	for key,value in subs.iteritems():
		if key in line:
			line=line.replace(key,value)
	newlines.append(line)

outfile = file(outfileName, 'w')
outfile.writelines(newlines)