import sys


theVersion=sys.argv[1]
uploadedFullFileBase=sys.argv[2]
uploadedAsdfFileBase=sys.argv[3]
uploadedDevFileBase=sys.argv[4]

uploadedFullFileName=uploadedFullFileBase+".tar.gz"
uploadedAsdfFileName=uploadedAsdfFileBase+".tar.gz"
uploadedDevFileName=uploadedDevFileBase+".tar.gz"

fullLink="http://rl-glue-ext.googlecode.com/files/"+uploadedFullFileName
fullDetailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+uploadedFullFileName
asdfLink="http://rl-glue-ext.googlecode.com/files/"+uploadedAsdfFileName
asdfDetailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+uploadedAsdfFileName
devLink="http://rl-glue-ext.googlecode.com/files/"+uploadedDevFileName
devDetailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+uploadedDevFileName

fileName="Lisp.wiki.template"
outfileName="Lisp.wiki"

subs={}
subs['LISP-CODEC-VERSION']=theVersion
subs['LISP-FULL-LINK']=fullLink
subs['LISP-FULL-DETAILS-LINK']=fullDetailsLink
subs['LISP-FULL-FILE-BASE']=uploadedFullFileBase
subs['LISP-ASDF-LINK']=asdfLink
subs['LISP-ASDF-DETAILS-LINK']=asdfDetailsLink
subs['LISP-ASDF-FILE-BASE']=uploadedAsdfFileBase
subs['LISP-DEV-LINK']=devLink
subs['LISP-DEV-DETAILS-LINK']=devDetailsLink
subs['LISP-DEV-FILE-BASE']=uploadedDevFileBase
f = file(fileName)
newlines = []
for line in f:
	for key,value in subs.iteritems():
		if key in line:
			line=line.replace(key,value)
	newlines.append(line)

outfile = file(outfileName, 'w')
outfile.writelines(newlines)
