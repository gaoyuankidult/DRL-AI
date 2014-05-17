import sys

#Saves us from writing lots of scripts
def make_basic_subs(version, downloadName,wikiInfileName, wikiOutfileName):
	Link="http://rl-glue-ext.googlecode.com/files/"+downloadName
	detailsLink="http://code.google.com/p/rl-glue-ext/downloads/detail?name="+downloadName

	subs={}
	subs['VERSION']=version
	subs['FILE-LINK']=Link
	subs['DETAILS-LINK']=detailsLink

	f = file(wikiInfileName)
	newlines = []
	for line in f:
		for key,value in subs.iteritems():
			if key in line:
				line=line.replace(key,value)
		newlines.append(line)

	outfile = file(wikiOutfileName, 'w')
	outfile.writelines(newlines)

theVersion=sys.argv[1]
uploadedFile=sys.argv[2]
wikiInFileName=sys.argv[3]
wikiOutFileName=sys.argv[4]
make_basic_subs(theVersion,uploadedFile,wikiInFileName,wikiOutFileName)


