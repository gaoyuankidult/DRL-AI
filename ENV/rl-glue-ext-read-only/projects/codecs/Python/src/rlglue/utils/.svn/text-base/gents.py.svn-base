#-------------------------------------------------------------------------------
# Name:        gents.py
# Description: A module of Reinforment Learning tools for generating taskspecs
# more programatically.
#
# Author:      Jose Antonio Martin H. (JAMH)
# Contact:     <jamartin@dia.fi.upm.es>
#
# Created:     15/03/2010
# Copyright:   (c) Jose Antonio Martin H. 2010
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
#-------------------------------------------------------------------------------
#!/usr/bin/env python


from UserDict import UserDict

class odict(UserDict):
    def __init__(self, dict = None):
        self._keys = []
        UserDict.__init__(self, dict)

    def __delitem__(self, key):
        UserDict.__delitem__(self, key)
        self._keys.remove(key)

    def __setitem__(self, key, item):
        UserDict.__setitem__(self, key, item)
        if key not in self._keys: self._keys.append(key)

    def clear(self):
        UserDict.clear(self)
        self._keys = []

    def copy(self):
        dict = UserDict.copy(self)
        dict._keys = self._keys[:]
        return dict

    def items(self):
        return zip(self._keys, self.values())

    def keys(self):
        return self._keys

    def popitem(self):
        try:
            key = self._keys[-1]
        except IndexError:
            raise KeyError('dictionary is empty')

        val = self[key]
        del self[key]

        return (key, val)

    def setdefault(self, key, failobj = None):
        UserDict.setdefault(self, key, failobj)
        if key not in self._keys: self._keys.append(key)

    def update(self, dict):
        UserDict.update(self, dict)
        for key in dict.keys():
            if key not in self._keys: self._keys.append(key)

    def values(self):
        return map(self.get, self._keys)


def dict2ts(ts):
    a= str(ts.items())[3:-3]
    a = a.replace(',','')
    a = a.replace("'",'')
    a = a.replace('{','')
    a = a.replace('}','')
    a = a.replace('(','')
    a = a.replace(')','')
    a = a.replace('[[','(')
    a = a.replace('[','(')
    a = a.replace(']]',']')
    a = a.replace(']',')')
    a = a.replace(':','')
    return a


# try to generate a taskspec 'programatically'

ts = odict()
ts['VERSION'] = 'RL-Glue-3.0'
ts['PROBLEMTYPE'] = 'episodic'
ts['DISCOUNTFACTOR'] = 1.0

ts['OBSERVATIONS']   = odict()
ts['OBSERVATIONS']['DOUBLES']= []
ts['OBSERVATIONS']['DOUBLES'].append(['-1.6' , '1.5']) # the decmial separator requires enclosing the number as a string


ts['ACTIONS']   = odict()
ts['ACTIONS']['INTS']= []
ts['ACTIONS']['INTS'].append([0 , 2])

ts['OBSERVATIONS']['DOUBLES'].append(['-0.07' , '0.07']) # the decmial separator requires enclosing the number as a string

ts['REWARDS'] = []
ts['REWARDS'].append([-1, 0])

ts['EXTRA'] = 'three little monkeys jumping on the bed'

a = dict2ts(ts)


print
print
print "Generated via Code"
print a
print
print

print "Manually"
print 'VERSION RL-Glue-3.0 PROBLEMTYPE episodic DISCOUNTFACTOR 1.0 OBSERVATIONS DOUBLES (-1.6 1.5) (-0.07 0.07) ACTIONS INTS (0 2)  REWARDS (-1 0)'


