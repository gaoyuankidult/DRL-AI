#
#  $Revision: 738 $
#  $Date: 2009-02-13 00:32:12 +0200 (Fri, 13 Feb 2009) $
#  $Author: brian@tannerpages.com $
#  $HeadURL: http://rl-glue-ext.googlecode.com/svn/trunk/projects/codecs/Python/src/rlglue/versions.py $

def get_svn_codec_version():
	SVN_GLUE_VERSION="$Revision: 738 $"
	justTheNumber=SVN_GLUE_VERSION[11:len(SVN_GLUE_VERSION)-2]
	return justTheNumber

def get_codec_version():
	return "2.02"