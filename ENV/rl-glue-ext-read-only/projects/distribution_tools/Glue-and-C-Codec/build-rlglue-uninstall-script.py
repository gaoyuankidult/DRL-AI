import os
basePath = './install_root'
RLGlue_Files = []

pythonTemplateFileName='uninstall-rlglue-template.py'
pythonUninstallFileName='uninstall-resources/uninstall-rlglue.py'

for root, dirs, files in os.walk(basePath):
	for f in files:
		if f.endswith('.h') or f.endswith('.dylib') or f.endswith('a'):
			thisName=os.path.join(root, f)
			nameWithoutBase=thisName[len(basePath):]
			RLGlue_Files.append(nameWithoutBase)

subs={}
subs['RLGLUE_FILE_REPLACE_HERE']=str(RLGlue_Files)
f = file(pythonTemplateFileName)
newlines = []
for line in f:
	for key,value in subs.iteritems():
		if key in line:
			line=line.replace(key,value)
	newlines.append(line)

outfile = file(pythonUninstallFileName, 'w')
outfile.writelines(newlines)