# 
# Copyright (C) 2008, Brian Tanner
# 
#http://rl-glue-ext.googlecode.com/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
#  $Revision$
#  $Date$
#  $Author$
#  $HeadURL$

class glue_test:
	callerName = "not set"
	failCount = 0
	testCount = 0
	def __init__(self, callerName):
		self.callerName = callerName
		self.failCount = 0
		self.testCount = 0
		
	def check_fail(self, didFail):
		self.testCount=self.testCount+1
		if didFail:
			self.failCount=self.failCount+1
			print "Failed test "+str(self.testCount)
			
	def getFailCount(self):
		return self.failCount
		
	def get_summary(self):
		if self.failCount>0:
			return "Failed "+str(self.failCount)+" / "+str(self.testCount)+" in "+self.callerName;
		else:
	   		return "Passed all "+str(self.testCount)+" checks in "+self.callerName;

