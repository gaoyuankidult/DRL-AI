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

fileName="Java.wiki.template"
outfileName="Java.wiki"

subs={}
subs['JAVA-USER-VERSION']=theVersion
subs['JAVA-USER-LINK']=userLink
subs['JAVA-USER-DETAILS-LINK']=userDetailsLink
subs['JAVA-USER-FILE-BASE']=uploadedUserFileBase
subs['JAVA-DEV-VERSION']=theVersion
subs['JAVA-DEV-LINK']=devLink
subs['JAVA-DEV-DETAILS-LINK']=devDetailsLink
subs['JAVA-DEV-FILE-BASE']=uploadedDevFileBase
f = file(fileName)
newlines = []
for line in f:
	for key,value in subs.iteritems():
		if key in line:
			line=line.replace(key,value)
	newlines.append(line)

outfile = file(outfileName, 'w')
outfile.writelines(newlines)