import types

class TopParser:
    def __init__(self):
        pass


def groupBy_n( x,n):
    if n <= 1 :
       yield x
    for i in range(1,len(x)):

         if n <= 2  :
             yield [x[:i] , x[i:]]
         if n > 2:
            for y in groupBy(x[i:],n-1 ):
                 yield [x[:i]] +  y

def SNormalize(s):
    if isinstance(s,list):
       if len(s) == 1 :
           return SNormalize(s[0])
    return s

def groupBy( x,n):
    for g in groupBy_n(x,n):
        yield [SNormalize(gi) for  gi in g]

class MResult:
    def __init__(self, state, bindings ={}):
        self.state = state
        self.bindings = bindings

    def combine(self, other):

        s = self.state and other.state
        if  (self.state == False or other.state == False ) :
            return MResult(False)

        bs = {**self.bindings, **other.bindings }

        return MResult(s,bs)

    def __repr__(self):
        if len(self.bindings) > 0:
           return  "MRES:"+str(self.state) + ":" + str(self.bindings)
        return  "MRES:"+str(self.state)

class MVar:
    def __init__(self, v):
        self.v = v
    def match(self,x):
        yield MResult(True , { self.v : x})
    def __repr__(self):
        return  "V("+self.v+")"


class MOptional:
    def __init__(self, v):
        self.v = v
    def match(self,x):
        for y in  self.v.match(x):
            yield y
    def __repr__(self):
        return  "?"+str(self.v)

class MAtom:
    def __init__(self, v):
        self.v = v
    def match(self,x):
        yield MResult( x == self.v)
    def __repr__(self):
        return  "'"+self.v

class MOr:
    def __init__(self, vs):
        self.v = vs
    def match(self,x):
        raise Exception("?")
        for v in self.v:
            for y in v.match(x): yield y
    def __repr__(self):
        return  "OR("+ (",".join([ str(y) for y in self.v]))+")"



def getCombMatchs(acc, vvs ):
        #print("imput:", acc, vvs)
        if len(vvs) == 0:
           print("ret:", acc)
           yield  acc+[]
        else:
            # obtem aquele com e sem os optionals
            if isinstance(vvs[0], MOr):
               for q in vvs[0].v:
                   for y2 in getCombMatchs(acc + [], (q,) + vvs[1:]):
                       yield y2
            elif isinstance( vvs[0], MOptional ):
                    for y0 in getCombMatchs(acc + [], (vvs[0].v,) + vvs[1:] ):
                        yield y0
                    for y1 in getCombMatchs(acc +[] , vvs[1:]):
                        yield y1
            else:
                   for y3 in getCombMatchs(acc + [vvs[0]], vvs[1:]):
                       yield y3

def match_rem( macc,   mss , xss ):
        #print(mss, xss)
        if len(xss) == 0:
            yield  macc
        else:
            mm_s = mss[0].match(xss[0])
            for mj in mm_s :
                if mj.state :
                    mq = macc.combine(mj)
                    if mq.state :
                       for  y in  match_rem( mq , mss[1:], xss[1:]):
                            yield y


class MList:
    def __init__(self, *vs):
        self.v = vs



    def match(self, xss):
        #print("m1 ",[ str(k) for k in self.v])
        dlist = [d for d in getCombMatchs([], self.v)]
        dlist.sort(key=lambda s: -len(s))
        for d in dlist:
            ymm = d
            n = len(ymm)
            for xs in groupBy(xss, n):
                mm = MResult(True)
                #print("M  ",list(ymm))
                for y in match_rem(mm,ymm,xs ):
                        yield y


    def match_old(self,xss):
        n = len(self.v)
        for xs in groupBy(xss, n):
            print(" ")
            mm = MResult(True)
            for i in range(n):
                mmi = self.v[i].match(xs[i])
                mm = mm.combine( mmi)
                if mm.state == False: break
            if mm.state : yield mm

    def __repr__(self):
        s = ','.join([" "+str(x)  for x in self.v])
        return  "MList( "+s+" )"



def kindOfPatt( s):
    if s[0] =="?":
        return MOptional(kindOfPatt(s[1:]))
    if '/' in s :
        sor = [kindOfPatt(ss) for ss in  s.split('/')]
        return MOr(sor)
    if len(s) <= 2:
       if s[0] == s[0].upper() : return  MVar( s )
    return MAtom(s)

def processPatten(patt):
    p = patt.split()
    return MList( *[kindOfPatt(pp) for pp in p] )

def splitPhaser(s):
    sbreak = 0
    acc = []
    a=""
    for i in range(len(s)):
        if s[i] =="(" :
            sbreak = sbreak +1
        elif s[i] == ")":
            sbreak = sbreak - 1
        elif s[i] == " " and sbreak <= 0:
           acc.append(a)
           a=""
        else:
            a = a+ s[i]
    acc.append(a)
    return acc




def match(s , patt):
    pm = processPatten(patt)
    print( "PATT: ",pm)
    sph = splitPhaser(s)
    for y in  pm.match(sph):
        yield y

def assertBase( parse, s  ):
    m = match(s , "X is an Y")


#for z in groupBy([1,2,3,4,5,6,7,8,9], 4):    print( ">",z)

s = "The brightnesses are guttering, weak, radiant and blazing."
s = "The box is an container"


#for ms in match(s , "X is a/an Y"): print("::", ms)
#for ms in match("(red box) is an kind of thing", "X is a/an kind of K"): print("::", ms)
#for ms in match("(red box) is an kind of thing", " ?the/a/an X is/are a/an kind of K"): print("::", ms)
for ms in match("a handle is part of every door", " ?the/a/an X is/are Y of every K"): print("::", ms)
for ms in match("in the Geometry Lab are three triangles and two squares", " in the/some X have/is/are Y "): print("::", ms)


